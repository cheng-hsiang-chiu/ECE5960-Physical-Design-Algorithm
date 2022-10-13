#! /bin/bash

files=('1' '2' '3' 'ami33' 'ami49' 'apte' 'hp' 'xerox')

for i in ${files[@]}
do
  echo "---------------------------------"
  echo "-----------  "$i"   -------------"
  echo "---------------------------------"
  time ./fp 0.6 "../input_pa2/"$i".block" "../input_pa2/"$i".nets" "./"$i".out"
  python3 checker.py $i $i".out"
done
