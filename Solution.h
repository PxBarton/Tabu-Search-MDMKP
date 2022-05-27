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

	/**
	 * generates a random solution using default_random_engine
	 * where K is the number of 1's in the solution
	 * 'K items in the knapsack'
	 * K estimated through external means, such as Excel LP relaxation, etc
	 */
	void K_Solution_Gen(int K);

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

	int getP();

	/**
	 * returns the value (0 or 1) of the solution 
	 * at a particular index
	 */
	int getXval(int index);

	int getLength();

	int calcK();

	int getMulti();

	void setMulti(int multiplier);

	void printSolution();

	/**
	 * operator =
	 * deep copy of a solution
	 */
	Solution& operator=(Solution& sol);

private:

	int xLen;

	valarray<int> xItems;


	// vector<vector<int>> pairList; 

	vector<int> violations; 

	int penalty = 0;

	bool feasible = 0;

	int zScore = 0;

	int fitness = 0;

	int multi = 1;


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

void Solution::K_Solution_Gen(int K) {
	int index;
	bool filled = false;
	default_random_engine gen;
	gen.seed(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<int> dis(0, 100);
	for (int i = 0; i < K; i++) {
		index = dis(gen);
		if (xItems[index] == 0)
			xItems[index] = 1;
		else {
			while (xItems[index] == 1) {
				index = dis(gen);
			}
			xItems[index] = 1;
		}
	}
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
	//cout << endl << numPairs << endl;
	
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
	//cout << pList.size() << endl << endl;

	// pairList = pList;
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
	//int kRows = coeff.getBk().size();
	//int dRows = coeff.getBd().size();
	penalty = 0;
	//vector<int> violArray((kRows + dRows), 0);

	for (int i = 0; i < coeff.getBk().size(); i++) {
		if (inner_product(begin(xItems), end(xItems), coeff.getAk()[i].begin(), 0) < coeff.getBk()[i])
			//	violArray[i] = 0;
			penalty += 0;
		else
			//	violArray[i] = inner_product(begin(xItems), end(xItems), coeff.getAk()[i].begin(), 0) - coeff.getBk()[i];
			penalty += inner_product(begin(xItems), end(xItems), coeff.getAk()[i].begin(), 0) - coeff.getBk()[i];
	}
	for (int j = 0; j < coeff.getBd().size(); j++) {
		if (inner_product(begin(xItems), end(xItems), coeff.getAd()[j].begin(), 0) > coeff.getBd()[j])
			//violArray[kRows + j] = 0;
			penalty += 0;
		else
			//violArray[kRows + j] = coeff.getBd()[j] - inner_product(begin(xItems), end(xItems), coeff.getAd()[j].begin(), 0);
			penalty += coeff.getBd()[j] - inner_product(begin(xItems), end(xItems), coeff.getAd()[j].begin(), 0);
	}
	//penalty = accumulate(violArray.begin(), violArray.end(), 0);
	if (penalty == 0)
		feasible = true;
	else
		feasible = false;
}

int Solution::calcZ(ProblemCoefficients& coeff) {
	int z;
	z = inner_product(begin(xItems), end(xItems), coeff.getC().begin(), 0);
	zScore = z;
	return z;
	
	
}

int Solution::evalFit(ProblemCoefficients& coeff) {
	fitness = this->calcZ(coeff) - multi * penalty;
	return fitness;
}

bool Solution::isFeasible() {
	if (penalty == 0)
		return true;
	else
		return false;
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

int Solution::getP() {
	return penalty;
}

int Solution::calcK() {
	return xItems.sum();
}

int Solution::getMulti() {
	return multi;
}

void Solution::setMulti(int multiplier) {
	multi = multiplier;
}

Solution& Solution::operator=(Solution& right) {
	for (int i = 0; i < xLen; i++)
		xItems[i] = right.getXval(i);
	multi = right.getMulti();
	zScore = right.getZ();
	return *this;
}




