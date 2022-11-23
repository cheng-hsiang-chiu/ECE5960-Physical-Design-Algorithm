# Steiner-Tree Construction

## Problem Description

This programming assignment asks you to implement a Steiner-tree router 
that can connect pins for a single net on a single-layer chip. 
Given a set of pins for a single net, the Steiner-tree router routes all pins within a chip. 
Pins are connected by horizontal lines (H-line) and/or vertical lines (V-line). 
Steiner points are allowed to be used during routing.
The objective of Steiner-tree routing is to minimize the total routing wirelength. 
The total routing wirelength $W$ of a set of $P$ can be computed by the following:

$$
W = \sum_{pi \in P} w(p_i) + d
$$

where $p_i$ denotes an H-line or a V-line in the line segment set $P$ and $w(p_i)$ denotes the real routing wirelength of $p_i$. Here, $d$ denotes the disjoing cost evaluated by the following:

$$
d = 2 \times U \times H
$$

where $U$ is the number of unconnected pins and $H$ is the half perimeter wirelength (HPWL) of the chip boundary.

Note that a route which has any net routed out of the chip boundary is a failed result.

## Input

The input file starts with the chip boundary, followed by the description of pins. The description of each pin contains the keyword PIN, followed by the name and the coordinate of the pin. 

| Input Format | Example |
| ------------ | ------- |
| Boundary = (llx,lly), (urx,ury)<br> NumPins = number <br> Pin name (x,y) <br> ...| Boundary = (0,0), (100,100) <br> NumPins = 3 <br> PIN p1 (20,30) <br> PIN p2 (50,30) <br> PIN p3 (50,90)|

The sample input for the format defines a net with three pins, `p1`, `p2`, and `p3`.

## Output

| Input Format | Example |
| ------------ | ------- |
| NumRoutedPins = number <br> Wirelength = number <br> H-line (x1,y) (x2,y) <br> V-line (x,y1) (x,y2) <br> ... | NumRoutedPins = 3 <br> WireLength = 90 <Br> V-line (50,30) (50,90) <br> H-line (20,30) (50,30) |


In the program output, you are asked to give the number of pins, the routing wirelength, and the coordinates of routed net segments. Note that you can output the H-line/V-line in any order.

## Language

You can implement this assignment using any language you like. However, we recommend `C` or `C++` for performance reason.

## Platform

You need to evaluate your program on the Linux server at `twhuang-server-01.ece.utah.edu`.

Please email Dr. Huang (tsung-wei.huang at utah.edu) for creating an account to log in.


## Program Command 

Your program should support the following command-line parameters:


```bash
[executable file name] [input file name] [output file name]
```

For example:

```bash
~$ ./router input_pa4/case1 output1
```


