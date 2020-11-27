#!/bin/bash

# Command for compile c mat with MPI
# bash execute.sh compile run matMPI
compile='compile' # compile file 
run='run' # run file 
#if you use both can compile and run
args=("$@")

exec(){
	for i in {1..2} #trys
	do
		for j in 10 100  #tam vector
			do
			for k in 10 100 	#iterations
				do
				mpirun -np 1 -machinefile mfile ./"$1" $j $k
				#mpirun -np 4 -machinefile mfile ./"$1" $j $k
				mpirun -np 8 -machinefile mfile ./"$1" $j $k
				done
			done
	done
}

if [ "${args[0]}" == "$compile" ] &&  [ "${args[1]}" == "$run" ];
then
	mpicc cellautoP2P-MPI.c -o execP2P
	exec ./execP2P
	echo "success"
else
	echo "revisa los comandos de entrada"
fi