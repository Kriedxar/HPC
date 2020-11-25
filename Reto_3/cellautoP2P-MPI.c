#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

//mpicc cellautoP2P-MPI.c -o execP2P
//mpirun -np 8 -hosts wn1,wn2,wn3,wn4,wn5,wn6,wn7,wn8 ./execP2P 16 1
//mpirun -np 8 -machinefile mfile ./execP2P 16 1

int main(int argc, char *argv[]){
	int n = atoi(argv[1]);
	int t = atoi(argv[2]);

	int numranks, rank, len;
	int tag = 0;
	char hostname[MPI_MAX_PROCESSOR_NAME];

	double startTime;
	double endTime;
	double tiempo;

	int *street1 = (int *)malloc((n+2)*sizeof(double));

	srand(time(NULL));

	for(int i = 1; i <= n; i++){
		street1[i] = rand() % 2;
	}
	street1[0] = street1[n];
	street1[n+1] = street1[1];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numranks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(hostname, &len);

	int *scatterStreet = (int *)malloc((n/numranks+2)*sizeof(double));
	int *gatherStreet = (int *)malloc((n/numranks+2)*sizeof(double));
	int *street2 = (int *)malloc((n+2)*sizeof(double));

	startTime = MPI_Wtime();

	//for(int p = 0; p < numranks; p++){
		MPI_Scatter(&street1[rank*n/numranks], (n/numranks)+2, MPI_INT,scatterStreet, (n/numranks)+2, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
	//}
	for(int r = 0; r < t; r++){
		//for(int p = 0; p < numranks; p++){
		for(int i = 1; i < n/numranks+1; i++){
			if(scatterStreet[i] == 0){
				if(scatterStreet[i-1] == 1){
					gatherStreet[i] = 1;
				}
				else{
					gatherStreet[i] = 0;
				}
			}
			else{
				if(scatterStreet[i+1] == 0){
					gatherStreet[i] = 0;
				}
				else if(scatterStreet[i+1] == 1){
					gatherStreet[i] = 1;
				}
			}
		}
		int prev = rank - 1;
		int next = rank + 1;
		if(prev < 0){
			prev = numranks - 1;
		}
		if(next == numranks){
			next = 0;
		}
		MPI_Send(&gatherStreet[1], 1, MPI_INT, prev, tag+5, MPI_COMM_WORLD);
		MPI_Send(&gatherStreet[n/numranks], 1, MPI_INT, next, tag, MPI_COMM_WORLD);
		
		MPI_Recv(&gatherStreet[0], 1, MPI_INT, prev, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&gatherStreet[n/numranks+1], 1, MPI_INT, next+5, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Barrier(MPI_COMM_WORLD);

		tag = tag + 1;
		scatterStreet = gatherStreet;
		//}
	}
	
	//for(int p = 0; p < numranks; p++){
		MPI_Gather(gatherStreet, n/numranks+1, MPI_INT, &street2[(n/numranks)*rank], n/numranks+1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
	//}
	street2[n+1] = street2[1];

	endTime = MPI_Wtime();
	if(rank == 0){
		tiempo = endTime - startTime;
	}

	if(rank == 0){
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
		printf("procesos: %d\ttiempo: %f\n", numranks, tiempo);
	}

	MPI_Finalize();
}