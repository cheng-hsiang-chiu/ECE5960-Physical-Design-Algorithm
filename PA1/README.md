# Fiduccia–Mattheyses (FM) Partition Algorithm


## What is this about?
This is a program assianment of ECE5960-Physical-Design-Algorithm..
The purpose of this assignment is to implement FM algorithm
and vefity the correctness using seven benchmarks. 


## Repository structure
- src : source code
- benchmark : seven input files of dat extension
- unittest : unit test
- 3rd-party : third party library for unit test usage only

## Build
```
mkdir build
cd build
make
```

## Run
```
cd build
./fm ../benchmark/input_6.dat ./output_6.dat
```

## Reference
- C.M.Fiduccia and R.M. Mattheyse, "A Linear-Time Heuristic for Improving Network Partitions," in "*19th Design Automation Conference*," 1982 [link](https://web.eecs.umich.edu/~mazum/fmcut1.pdf)
- https://en.wikipedia.org/wiki/Fiduccia–Mattheyses_algorithm
