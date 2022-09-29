#! /bin/bash

for i in {0..6}
do
  echo "input_"$i
  time ./fm ../benchmark/input_$i.dat ./output_$i.dat
  ../checker_linux ../benchmark/input_$i.dat ./output_$i.dat
done
