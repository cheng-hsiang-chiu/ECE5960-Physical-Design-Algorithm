#! /bin/bash

files=('1' '2' '3' '4' '5' '6' '7' '8' '100000' '200000' '500000')

for i in ${files[@]}
do
  echo "---------------------------------"
  echo "-----------  "$i"   -------------"
  echo "---------------------------------"
  time ./st "../input_pa3/case"$i"" "./case"$i".out"
done
