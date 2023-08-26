#!/bin/bash

while read func
do
	ans="---"
	pathes=$MANPATH
	pathes=$(echo $pathes | sed -n 's/:/ /pg')
	
	for path in $pathes
	do
		if [ ! -d "$path/man3" ]
		then
			continue
		fi	
		if [ -f "$path/man3/$func.3.gz" ]
		then
			includes=$(gunzip -dc "$path/man3/$func.3.gz" | sed -n 's/.*#include .//p' | sed -n 's/>.*//p')

			for lib in $(echo "$includes")
			do
				ans=$lib
				break
			done
		else
			if [ -f "$path/man3/$func.3" ]
			then
				file=$(echo $(ls | sed -n /^$func.3/p))
				includes=$(cat "$path/man3/$func.3" | sed -n 's/.*#include .//p' | sed -n 's/>.*//p')

				for lib in $(echo "$includes")
				do
					ans=$lib
					break
				done
			fi
		fi
			
	done

echo $ans
done
