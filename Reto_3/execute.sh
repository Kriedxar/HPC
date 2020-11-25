#!/bin/bash

# Command for compile c mat with MPI
# bash execute.sh compile run matMPI
compile='compile' # compile file 
run='run' # run file 
#if you use both can compile and run
args=("$@")

exec(){
	for i in {1..10}
	do
		for k in 16
			do
			mpirun -np 8 -machinefile mfile ./"$1" $k
			done
	done
}

echo " ${args[0]} ${args[1]} ${args[2]}"

if [ "${args[0]}" == "$compile" ] &&  [ "${args[1]}" == "$run" ];
then
	mpicc cellautoCC-MPI.c -o execCC
	exec ./execCC
else
	echo "revisa los comandos de entrada"

fi