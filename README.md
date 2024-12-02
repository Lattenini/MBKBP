# Efficient Maximum Balanced k-biplex Search over Bipartite Graphs
This is an official implementation of "Efficient Maximum Balanced k-biplex Search over Bipartite Graphs"


## Source code info
Programming Language: `C++`
 
Compiler Info: `clang++`

Packages/Libraries Needed: `CMake 2.8`, `makefile`

## Datasets info
All datasets used in our experiments can be download from the [KONECT](http://konect.cc) website and the [Networkrepository](https://networkrepository.com/) website. We also provide an example dataset in "./dataset/Example.g"



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
Running Time: 0.001859 sec
|E|: 87  |L|: 5  |R|: 18
L: 4 0 5 2 1
R: 40 34 37 25 26 21 11 23 33 16 35 9 18 41 10 46 13 28

|E|: 87  |L|: 6  |R|: 15
L: 6 0 1 4 2 5
R: 34 35 16 11 33 23 21 40 9 26 10 25 28 13 37
>
> ./FastBB -f "../dataset/writer.g" -k 1 -u 3 -v 3 -K 2
Running Time: 0.060531 sec
|E|: 125  |L|: 32  |R|: 4
L: 1683 3644 3972 4165 4255 4385 4591 4638 4667 4590 4675 4676 4678 4679 4680 2503 4682 2549 4691 5739 5764 5978 7058 8722 8747 9970 11840 2892 2897 3481 3621 22775
R: 27542 27544 27543 27541

|E|: 136  |L|: 34  |R|: 4
L: 1616 1620 1621 1627 1629 2172 2332 3290 3864 4483 4775 1617 1619 13832 13975 14626 15570 15745 15832 17009 17036 19608 20875 21888 22121 22122 22483 22757 23021 23023 23035 23036 23037 23038
R: 27472 27473 27474 27475
>
>
```



## Input Graph Format
The input graph  should follow the following format.

 Example.graph

    3 1 2
    0 1 2
    1 0
    2 0
    (File ends with an empty line)

(1) Given an input graph G=(L,R), vertices are represented by non-negtive integers from 0 to |V(G)|. By default, {0,1,...,|L|-1} denotes the left side and {|L|,|L|+1,...,|L|+|R|-1} denotes the right side. 

(2) The first line includes 3 non-neigtive integers, e.g., 3 1 2, that denote the number of vertices, the id of the first vertex in the right side and the number of edges, respectively. To illustrate, consider the first line 3 2 1. The input graph has three vertices {0, 1, 2}, and two edges (0, 1) and (0, 2). {0} denotes the left side and {1,2} denotes the right sie.

(3) The following lines represent an adjacent list of the input graph. To illustrate, consider the second line 0 1 2. The vertex with id 0 is adjacent with two vertices 1 and 2.

