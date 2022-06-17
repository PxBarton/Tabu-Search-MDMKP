# Tabu-Search-MDMKP
Tabu Search metaheuristic designed to find feasible solutions for highly constrained knapsack problems

Knapsack problems are well known in a number of industries and scientific disciplines. This project deals with 
the 0/1 Multi-dimensional Multi-demand Knapsack Problem (MDMKP). This title refers to the nature of the additional constraints; 
the '0/1' indicates that for a given solution, a specific item may be selected or not selected. Consider the case of a 
truck being loaded: the classic single constraint (using a truck instead of a knapsack) would be weight; but also, a single 
object cannot be too tall or too long or too wide to fit in the truck. This is an example of additional 'dimensional' constraints.
Also consider a minimum of some characteristic of the items; this would represent a demand constraint. 
This project uses a 'solution-based' tabu-search algorithm modified from the two-stage version presented in the 2019 paper
"Two-stage solution-based tabu search for the multidimensional knapsack problem" by Lai, Hao, and Yue. 
