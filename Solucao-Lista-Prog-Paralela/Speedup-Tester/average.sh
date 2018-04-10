#!/bin/bash

count=0;
total=0;
filename=$1

for i in $( awk '{ print $1; }' $filename )
   do 
     total=$(echo $total+$i | bc )
     ((count++))
   done
echo "scale=10; $total / $count" | bc
