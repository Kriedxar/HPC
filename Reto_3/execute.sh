#!/bin/bash

# Command for compile c mat with MPI
# bash execute.sh compile run matMPI
compile='compile' # compile file 
run='run' # run file 
#if you use both can compile and run
args=("$@")

exec(){
	for i in {1..5}
	do
		for j in 10 100 1000 10000 100000
			do
			for k in 10 100 1000
				do
				mpirun -np 1 -hosts wn1 ./"$1" $j $k
				mpirun -np 8 -machinefile mfile ./"$1" $j $k
				done
			done
	done
}

echo " ${args[0]} ${args[1]} ${args[2]}"

if [ "${args[0]}" == "$compile" ] &&  [ "${args[1]}" == "$run" ];
then
	mpicc cellautoCC-MPI.c -o execP2P
	exec ./execP2P
	echo "success"
else
	echo "revisa los comandos de entrada"

fi