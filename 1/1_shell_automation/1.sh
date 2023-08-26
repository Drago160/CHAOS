#!/usr/bin/env bash

IFS=,

matr=()
arr=()
counter=1

while IFS=, read line; do
	counter=$(($counter+1))
done < $1 

column_number=$counter
string_number=$(($counter-1))
el_number=$(($column_number*$string_number))






while IFS=, read line; do
	for el in $line
	do
		matr+=( $el )
	done
done < $1

IFS=" "

function print_matr {
	echo "----------------------"
	for (( i=0; i<$el_number; i++ ))
	do
		if [ $(($i % $column_number)) -eq 0 ]
		then
			echo
		fi
		echo ${matr[$i]}
	done
	echo "----------------------"
}

function sub_str_with_koef {
#1-Уменьшаемое
#2-Вычитаемое
#3-кооф вычитание
	mult1=$(($(($1 - 1)) * $column_number))
	mult2=$(($(($2 - 1)) * $column_number))

	for (( j=1; j<=$column_number; j++ ))
	do
		prev_element=${matr[$(($mult1 + $j-1))]}

		delta=$(echo "${matr[$(($mult2 + $j-1))]}*($3)" | bc)

		if [ -z $delta ]
		then
			delta=0
		fi

		new_element=$(echo "$prev_element-($delta)" | bc -l)

		matr[$(($mult1+$j-1))]=$new_element
	done
}

function normalize {
#1 - номер строки, которую нужно нормировать
	mult=$(($(($1 - 1)) * $column_number))
	norm_factor=1
	first_notnull=0

	for (( i=1; i<=$column_number; i++ ))
	do
		if [ ${matr[$(($mult + $i-1))]} != 0 ]
		then
			norm_factor=${matr[$(($mult + $i-1))]}
			first_notnull=$i
			break
		fi
	done

	new=
	counter=0
	for (( i=$first_notnull; i <= $column_number; i++ ))
	do	
		new=$(echo "${matr[$(($mult + $i-1))]}/($norm_factor)" | bc -l)
		matr[$(($mult + $i-1))]=$new
	done
}

function normalize_matr {
	for (( j=1; j<=$string_number; j++ ))
	do
		normalize $j
	done
}


function col_to_zeros {
#1 - какой строкой чистим столбик

	not_null_index=0
	while true
	do
		if [ ${matr[$(($(($1-1))*$column_number+$not_null_index))]} == 0 ]
		then
			not_null_index=$(($not_null_index+1))
		else
			break
		fi
	done

	normalize $1


	for (( i=1; i<=$string_number; i++ ))
	do
		if [ $1 -eq $i ]
		then
			continue
		fi

		sub_str_with_koef $i $1 ${matr[$((($i-1)*$column_number+$not_null_index))]}
	done
}




#arr[i][j] = matr[(i-1)*column_number+j-1]
for (( k=1; k<=string_number; k++))
do
	col_to_zeros $k
done

ans_arr=
for ((k=1; k<=string_number; k++))
do
	ans_arr+=( 0 )
done

for ((i=1; i<=string_number; i++))
do
	for ((j=1; j<=string_number; j++))
	do
		if [ ${matr[((($i-1)*$string_number+$j-1))]} != 0 ]
		then
			ans=${matr[ (((($j*$column_number))-1))  ]}
			ans_arr[j-1]=$ans
		fi
	done
done

for ((k=1; k<= string_number; k++))
do
	echo $(echo ${ans_arr[k-1]} | sed -n 's/\..*//p')
done
