#!/bin/bash

TIMES=$(grep "Time elapsed" times.dat | cut -d ' ' -f 4)
PROC=1
MAXN=7
COUNTER=1
FILES=""

for TIME in $TIMES; do
	if [ $COUNTER -eq 8 ]; then
		FILES="$FILES $PROC.dat"
		let COUNTER=1
		let PROC=PROC*2
	fi
	echo $TIME >> $PROC.dat
	let COUNTER=COUNTER+1
done
FILES="$FILES $PROC.dat"

COUNTER=4
for i in $(seq 1 7); do
	echo $COUNTER >> results.dat
	let COUNTER=COUNTER*2
done

paste results.dat $FILES > resultsParallel.dat
rm -f $FILES
rm -f results.dat
