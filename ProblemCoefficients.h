#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <valarray>

using namespace std;

/**
 * class to load, store and access the coefficients
 * for MDMKP and MKP optimization problems
 */
class ProblemCoefficients {
public:
	
	ProblemCoefficients(); 

	/**
	 * assigns problem coefficients from vectors 
	 */
	void loadCoeffs(vector<vector<int>> Ak, vector<vector<int>> Ad, vector<int> Bk, vector<int> Bd, vector<int> C, int kRows, int dRows);

	//void loadMKP(vector<int> A, vector<int> B, vector<int> C);

	/**
	 * returns LHS knapsack constraint coefficients
	 */
	vector<vector<int>> getAk();

	/** 
	 * returns LHS demand constraint coefficients
	 */
	vector<vector<int>> getAd();

	/**
	 * returns knapsack RHS
	 */
	vector<int> getBk();

	/** 
	 * returns demand RHS
	 */
	vector<int> getBd();

	/**
	 * returns objective function coefficients
	 */
	vector<int> getC();

	int getLen();

	void printAk();

	void printAd();

	void printBk();

	void printBd();

	void printC();

private:

	vector<vector<int>> LHS_knapsack;

	vector<vector<int>> LHS_demand;

	vector<int> RHS_knapsack;

	vector<int> RHS_demand;

	vector<int> objCoeffs; 

	int cLen;


	
};

ProblemCoefficients::ProblemCoefficients() {

}

void ProblemCoefficients::loadCoeffs(vector<vector<int>> Ak, vector<vector<int>> Ad, vector<int> Bk, vector<int> Bd, vector<int> C, int kRows, int dRows) {
	for (int i = 0; i < kRows; i++)
		LHS_knapsack.push_back(Ak[i]);
	for (int i = 0; i < dRows; i++)
		LHS_demand.push_back(Ad[i]);
	for (int i = 0; i < kRows; i++)
		RHS_knapsack.push_back(Bk[i]);
	for (int i = 0; i < dRows; i++)
		RHS_demand.push_back(Bd[i]);
	objCoeffs = C;
	cLen = C.size();
	
}
vector<vector<int>> ProblemCoefficients::getAk() {
	return LHS_knapsack;
}

vector<vector<int>> ProblemCoefficients::getAd() {
	return LHS_demand;
}

vector<int> ProblemCoefficients::getBk() {
	return RHS_knapsack;
}

vector<int> ProblemCoefficients::getBd() {
	return RHS_demand;
}

vector<int> ProblemCoefficients::getC() {
	return objCoeffs;
}

int ProblemCoefficients::getLen() {
	return cLen;
}

void ProblemCoefficients::printC() {
	for (auto& i : objCoeffs)
		cout << i << ' ';
	cout << endl;
}

void ProblemCoefficients::printAk() {
	for (int i = 0; i < LHS_knapsack.size(); i++) {
		for (int j = 0; j < LHS_knapsack[i].size(); j++)
			cout << LHS_knapsack[i][j] << ' ';
		cout << endl;
	cout << endl;
	}

		
}

void ProblemCoefficients::printAd() {
	for (int i = 0; i < LHS_demand.size(); i++) {
		for (int j = 0; j < LHS_demand[i].size(); j++)
			cout << LHS_demand[i][j] << ' ';
		cout << endl;
		cout << endl;
	}