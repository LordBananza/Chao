#!/bin/bash

if [ $# -eq 0 ];
then
	echo "Error: Please specify the name of the file"
else	
	./chao $1
	aslc86k/aslc86k "$1.s"
fi
