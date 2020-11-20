#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

//mpicc cellautoMPI.c -o executable
//mpirun -np 4 -hosts head,wn1,wn2,wn3 ./executable $8

int main(int argc, char *argv[]){
	//int n = 16;
	int n = atoi(argv[1]);
	int numv = 0;
	int nummov = 0;
	float speed = 0;

	int numranks, rank, len;
	char hostname[MPI_MAX_PROCESSOR_NAME];

	double startTime;
	double endTime;
	double tiempo;

	int *street1 = (int *)malloc((n+2)*sizeof(int));

	srand(time(NULL));

	for(int i = 1; i <= n; i++){
		street1[i] = rand() % 2;
		if (street1[i] == 1){
			numv = numv + 1;
		}
	}
	street1[0] = street1[n];
	street1[n+1] = street1[1];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(hostname, &len);

	int *scatterStreet = (int *)malloc((n/numranks+2)*sizeof(int));
	int *gatherStreet = (int *)malloc((n/numranks)*sizeof(int));
	int *street2 = (int *)malloc((n+2)*sizeof(int));

	startTime = MPI_Wtime();

	for(int p = 0; p < numranks; p++){
		MPI_Scatter(&street1[p*n/numranks], (n/numranks)+2, MPI_INT,scatterStreet, (n/numranks)+2, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		for(int i = 1; i < n/numranks+1; i++){
			if(scatterStreet[i] == 0){
				if(scatterStreet[i-1] == 1){
					gatherStreet[i-1] = 1;
					nummov = nummov + 1;
				}
				else{
					gatherStreet[i-1] = 0;
				}
			}
			else{
				if(scatterStreet[i+1] == 0){
					gatherStreet[i-1] = 0;
				}
				else if(scatterStreet[i+1] == 1){
					gatherStreet[i-1] = 1;
				}
			}
		}
		MPI_Gather(gatherStreet, n/numranks, MPI_INT, &street2[(n/numranks)*p+1], n/numranks, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
	}
	street2[0] = street2[n];
	street2[n+1] = street2[1];

	endTime = MPI_Wtime();
	if(rank == 0){
		tiempo = endTime - startTime;
	}

	speed = 1.0*nummov/numv;

	if(rank == 0){
		/*
		printf("t:\t");
		for(int i = 1; i < n+1; i++){
			printf("%d ", street1[i]);
		}
		printf("\n");
		printf("t+1:\t");
		for(int i = 1; i < n+1; i++){
			printf("%d ", street2[i]);
		}
		printf("\n");
		*/
		printf("espacios: %d\tprocesos: %d\tvehiculos: %d\tmovimientos: %d\tvelocidad: %.2f\ttiempo: %f\n", n, numranks, numv, nummov, speed, tiempo);
	}

	MPI_Finalize();
}