#pragma once

#include <iostream>
#include <vector>
#include <valarray>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>


using namespace std;

/**
 * class to store and evaluate a solution to a 0-1 knapsack problem
 */
class Solution {
public:

	Solution(int n);

	/**
	 * generates a random solution using default_random_engine
	 */
	void generate();

	void clearSolution();

	/**
	 * creates a list of all unique pairs of indices
	 * where duplicates (x,y) = (y,x) are removed
	 * for the purpose of swapping the values of a pair of indices
	 * combination: 'n choose 2'
	 */
	vector<vector<int>> createPairList();

	/**
	 * flips a bit from 0 to 1 or from 1 to 0
	 * at a particular index
	 */
	void flipBit(int index);

	//void flipBits();

	/** 
	 * swaps the bits for a particular pair of indices
	 * provided from the list of pairs
	 */
	void swapBit(vector<int>& pair);

	/**
	 * calculates the z-score of a paricular solution
	 */
	int calcZ(ProblemCoefficients& coeff);

	/** 
	 * computes the total amaount by which a solution
	 * violates the LHS knapsack and demand constraints
	 */
	void violAmounts(ProblemCoefficients& coeff);

	/**
	 * evaluates the fitness of a given solution 
	 * using the function given in Lei et al 2019
	 */
	int evalFit(ProblemCoefficients& coeff);

	bool isFeasible();

	int getZ();

	/**
	 * returns the value (0 or 1) of the solution 
	 * at a particular index
	 */
	int getXval(int index);

	int getLength();

	void printSolution();

	/**
	 * operator =
	 * deep copy of a solution
	 */
	Solution& operator=(Solution& sol);

private:

	int xLen;

	valarray<int> xItems;

	vector<vector<int>> pairList; 

	vector<int> violations; 

	int penalty = 0;

	bool feasible = 0;

	int zScore = 0;


};

Solution::Solution(int n): xItems(n), xLen(n) {
	//xLen = n;
}

void Solution::generate() {
	default_random_engine gen;
	gen.seed(chrono::system_clock::now().time_since_epoch().count());
	bernoulli_distribution dist(0.5);
	for (int i = 0; i < xLen; i++)
		if (dist(gen))
			xItems[i] = 1;
		else
			xItems[i] = 0;
	dist.reset();
}

void Solution::clearSolution() {
	for (int i = 0; i < xLen; i++)
		xItems[i] = 0;
}

vector<vector<int>> Solution::createPairList() {
	vector<vector<int>> pList;
	vector<int> pair = { 0, 0 };
	int numPairs = 0;
	for (int i = 0; i < xLen; i++) {
		for (int j = 0; j < xLen; j++) {
			if (i != j) {
				pair[0] = i;
				pair[1] = j;
				pList.push_back(pair);
				//cout << pair[0] << ' ' << pair[1] << endl;
				numPairs++;
			}
		}
	}
	cout << endl << numPairs << endl;
	
	for (int i = 0; i < pList.size(); i++) {
		for (int j = 0; j < pList.size(); j++) {
			if ((pList[i][0] == pList[j][1]) && (pList[i][1] == pList[j][0]))
				pList.erase(pList.begin() + j);
		    
		}
	}
	/*
    for (int i = 0; i < pList.size(); i++)
		cout << pList[i][0] << ' ' << pList[i][1] << endl;
    */
	cout << pList.size() << endl << endl;
	return pList;
}

void Solution::flipBit(int index) {
	if (xItems[index] == 0)
		xItems[index] = 1;
	else if (xItems[index] == 1)
		xItems[index] = 0;
}


void Solution::swapBit(vector<int>& pair) {
	int temp;
	if (xItems[pair[0]] != xItems[pair[1]]) {
		temp = xItems[pair[0]];
		xItems[pair[0]] = xItems[pair[1]];
		xItems[pair[1]] = temp;
	}
}

void Solution::violAmounts(ProblemCoefficients& coeff) {
	int kRows = coeff.getBk().size();
	int dRows = coeff.getBd().size();
	valarray<int> violArray(0, (kRows + dRows));

	for (int i = 0; i < kRows; i++) {
		if (inner_product(begin(xItems), end(xItems), coeff.getAk()[i].begin(), 0) < coeff.getBk()[i])
			violArray[i] = 0;
		else
			violArray[i] = inner_product(begin(xItems), end(xItems), coeff.getAk()[i].begin(), 0) - coeff.getBk()[i];
	}
	for (int j = 0; j < dRows; j++) {
		if (inner_product(begin(xItems), end(xItems), coeff.getAd()[j].begin(), 0) > coeff.getBd()[j])
			violArray[kRows + j] = 0;
		else
			violArray[kRows + j] = coeff.getBd()[j] - inner_product(begin(xItems), end(xItems), coeff.getAd()[j].begin(), 0);
	}
	penalty = violArray.sum();
	if (penalty == 0)
		feasible = true;
	else
		feasible = false;
}

int Solution::calcZ(ProblemCoefficients& coeff) {
	if (xLen == coeff.getLen()) {
		int z;
		z = inner_product(begin(xItems), end(xItems), coeff.getC().begin(), 0);
		zScore = z;
		return z;
	}
	else
		cout << "Incompatible sizes" << endl;
	
}

int Solution::evalFit(ProblemCoefficients& coeff) {
	return (this->calcZ(coeff) - penalty);
}

bool Solution::isFeasible() {
	return feasible;
}

int Solution::getLength() {
	return xLen;
}

int Solution::getXval(int index) {
	return xItems[index];
}

void Solution::printSolution() {
	for (int i = 0; i < xLen; i++)
		cout << xItems[i] << ' ';
	cout << endl;
}

int Solution::getZ() {
	return zScore;
}

Solution& Solution::operator=(Solution& right) {
	for (int i = 0; i < xLen; i++)
		xItems[i] = right.getXval(i);
	return *this;
}




