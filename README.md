# CptS 350: Design of Algorithms

Includes the primary course project: implementation of a symbolic algorithm to solve the following:

Let G be a graph over 32 nodes (namely, node 0, · · ·, node 31). 
For all 0 ≤ i, j ≤ 31, there is an edge from node i to node j iff (i + 3)%32 = j%32 or (i + 8)%32 = j%32. 
A node i is even if i is an even number. A node i is prime if i is a prime number.
In particular, we define [even] as the set {0, 2, 4, 6, · · · , 30} and 
[prime] as the set {3, 5, 7, 11, 13, 17, 19, 23, 29, 31}. We use R to denote the set of all edges in G.

for each node u in [prime], is there a node v in [even] such that u can reach v in even number of steps?
