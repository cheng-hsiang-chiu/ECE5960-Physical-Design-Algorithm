# Fiduccia–Mattheyses (FM) Partition Algorithm


## What is this about?
This is a program assianment of ECE5960-Physical-Design-Algorithm.
The purpose of this assignment is to implement FM algorithm
and vefity the correctness using seven benchmarks.
Refer to [Problem Statement](#Problem Statement),
[Input Format](#Input Format) and [Output Format](#Output Format)
for more information.

## Problem Statement
Let $C = c_1, c_2, c_3, ..., c_n$ be a set of $n$ cells and $N = n_1, n_2, n_3, ..., n_m$ be a set of $m$ nets. Each net $n_i$ connects a subset of the cells in $C$. Your job in this programming assignment is to implement the 2-way [Fiduccia-Mattheyses algorithm](./paper/A_Linear-Time_Heuristic_for_Improving_Network_Partitions.pdf) that partitions the set $C$ of $n$ cells into two disjoing, balanced groups, $G_1$ and $G_2$, such that the overall cut size is minimized. No cell replication is allowed. The cust size $s$ is given by the number of nets among $G_1$ and $G_2$. For a given balance factor $r$, where $0 < r < 1$, the objective is to minimize the cut size $s$ while satisfying the following constraint:

$$
n\times(1-r)/2 \leq |G_1|, |G_2| \leq n\times(1+r)/2 
$$

## Input Format

Each input file starts with a balance factor $r$, followed by the description of $m$ nets. Each net description contains the keyword `NET`, followed by the net name and a list of the connected cells, and finally the symbol `;` denoting the end of the net description. 

| Input Format | Example |
| ------------ | ------- |
| $r$ <br> NET NET_NAME [CELL_NAME]+; | 0.5 <br> NET n1 c2 c3 c4 ; <br> NET n2 c3 c6 ; <br> NET n3 c3 c5 c6 ; <br> NET n4 c1 c3 c5 c6 ; <br> NET n5 c2 c4 ; <br> NET n6 c4 c6 ; <br> NET n7 c5 c6 ;|

In the example circuit, we have a balance factor of `0.5` and six nets `n1`, `n2`, and `n3`, where net `n1` has three cells `c1`, `c2`, `c3`, and `c4`, net `n2` has two cells `c3` and `c6`, net `n3` has three cells `c3`, `c5`, and `c6`, and so on.

## Output Format

In the program output, you are asked to give the cut size, the sizes of $G_1$ and $G_2$, and the contents of $G_1$ and $G_2$ (i.e., cells). The following table gives the output format and a sample output:

| Input Format | Example |
| ------------ | ------- |
| Cutsize = $s$<br> G1 size <br> [cells]+ <br> G2 size <br> [cells]+ | Cutsize = 5 <br> G1 3 <br> c1 c2 c3 ; <br> G2 3 <br> c4 c5 c6;|

Note that the example solution may not be the optimal one.




## Repository structure
- src : source code
- benchmark : seven input files of dat extension
- unittest : unit test
- 3rd-party : third party library for unit test usage only
- paper : papers
- CMakeLists.txt : cmake file
- checker_linux : correctness checking

## Build
To build the executable, please follow the instructions below. The default compiler is clang++.
```
mkdir build
cd build
make
```

## Run
To run the executable, please follow the instructions below. Change the input files and output files according to your needs.
```
cd build
./fm ../benchmark/input_6.dat ./output_6.dat
```

## Unit Test
To run the unit tests, please follow the instructions below.
```
cd build
make test
```

## Verify correctness
To verify the correctness of the executable, please follow the instructions below. Modify the input files and output files according to your needs.
```
./checker_linux ./benchmark/input_6.dat ./build/output_6.dat
```


## Reference
- C.M.Fiduccia and R.M. Mattheyse, "A Linear-Time Heuristic for Improving Network Partitions," in "*19th Design Automation Conference*," 1982 [[link](./paper/A_Linear-Time_Heuristic_for_Improving_Network_Partitions.pdf)]
- https://en.wikipedia.org/wiki/Fiduccia–Mattheyses_algorithm
