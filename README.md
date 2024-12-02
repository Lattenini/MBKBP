# Efficient Maximum Balanced k-biplex Search over Bipartite Graphs
This is an official implementation of "Efficient Maximum Balanced k-biplex Search over Bipartite Graphs"


## Source code info
Programming Language: `C++`
 
Compiler Info: `clang++`

Packages/Libraries Needed: `CMake 2.8`, `makefile`

## Datasets info
All datasets used in our experiments can be download from the [KONECT](http://konect.cc) website and the [Networkrepository](https://networkrepository.com/) website. We also provide two example datasets, i.e. Example.g and divorce.g, in the folder "./dataset".



## Setup
```shell
make clean 
make
```

## Usage
  ./MBKBPZigzag {OPTIONS}

    MBKBPZigzag, an algorithm for finding the maximum δ-balanced k-biplex

  OPTIONS:

      -h            Display this help menu
      -f            Path to dataset
      -d            The parameter delta
      -k            The parameter k
      -l            The lower bound of number of vertices at left side
      -r            The lower bound of number of vertices at right side


## Running Example

```shell
> make clean
> make
>
> ./MBKBPZigzag -f "./dataset/Example.g" -k 1 -d 5 -l 3 -r 3
Update MBKBP: 8
|L|: 5
|R|: 3
Res_L:0 3 4 1 6
Res_R:10 7 9
Update MBKBP: 9
|L|: 5
|R|: 4
Res_L:1 3 2 4 6
Res_R:7 9 10 11
Running Time: 9.3e-05 sec
>
> ./MBKBPZigzag -f "./dataset/divorce.g" -k 1 -d 20 -l 3 -r 3
Update MBKBP: 23
|L|: 5
|R|: 18
Res_L:0 4 5 1 2
Res_R:9 10 13 16 18 11 21 23 25 26 28 33 34 35 37 40 41 46
Update MBKBP: 25
|L|: 3
|R|: 22
Res_L:5 4 1
Res_R:44 9 21 25 26 28 29 10 23 33 34 35 13 37 11 39 40 16 42 18 19 46
Update MBKBP: 26
|L|: 3
|R|: 23
Res_L:4 1 2
Res_R:43 38 23 21 25 26 28 29 10 9 33 34 35 13 37 11 39 40 16 42 18 19 46
Running Time: 0.000682 sec
```


## Input Graph Format
The input graph  should follow the following format.

 Example.graph

    14 7 49
    0 7 11
    1 7 8 11 12 13
    2 7 9 11 13
    3 7 11 12
    ...
    (File ends with an empty line)

(1) Given an input graph G=(L,R), vertices are represented by non-negtive integers from 0 to |V(G)|. By default, {0,1,...,|L|-1} denotes the left side and {|L|,|L|+1,...,|L|+|R|-1} denotes the right side. 

(2) The first line includes 3 non-neigtive integers, e.g., 14 7 49, that denote the number of vertices, the id of the first vertex in the right side and the number of edges, respectively. To illustrate, consider the first line 14 7 49. The input graph has 14 (7+7=14) vertices,  and 49 edges. {0-6} denotes the left side and {7-13} denotes the right sie.

(3) The following lines represent an adjacent list of the input graph. To illustrate, consider the second line 0 7 11. The vertex with id 0 is adjacent with two vertices 7 and 11.

