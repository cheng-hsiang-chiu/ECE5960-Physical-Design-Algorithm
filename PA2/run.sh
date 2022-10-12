#! /bin/bash

files=('1' '2' '3' 'ami33' 'ami49' 'apte' 'hp' 'xerox')

for i in ${files[@]}
do
  echo "---------------------------------"
  echo "-----------  "$i"   -------------"
  echo "---------------------------------"
  time ./fp 0.6 "../benchmarks/"$i".block" "../benchmarks/"$i".nets" "./"$1".out"
done

#for i in {0..6}
#do
#  echo "input_"$i
#  time ./fm ../benchmark/input_$i.dat ./output_$i.dat
#  ../checker_linux ../benchmark/input_$i.dat ./output_$i.dat
#done
