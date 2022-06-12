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
	
	ProblemCoefficients(int length, int kRowNum, int dRowNum); 

	~ProblemCoefficients();

	/**
	 * assigns problem coefficients from vectors 
	 */
	void loadCoeffs(vector<vector<int>> Ak, vector<vector<int>> Ad, vector<int> Bk, vector<int> Bd, vector<int> C);

	//void loadMKP(vector<int> A, vector<int> B, vector<int> C);

	/**
	 * returns LHS knapsack constraint coefficients
	 */
	vector<int*> getAk();

	/** 
	 * returns LHS demand constraint coefficients
	 */
	vector<int*> getAd();

	/**
	 * returns knapsack RHS
	 */
	int* getBk();

	/** 
	 * returns demand RHS
	 */
	int* getBd();

	/**
	 * returns objective function coefficients
	 */
	int* getC();

	int getLen();

	int getKRows();

	int getDRows();

	void printAk();

	void printAd();

	void printBk();

	void printBd();

	void printC();

private:
	/*
	vector<vector<int>> LHS_knapsack;

	vector<vector<int>> LHS_demand;

	vector<int> RHS_knapsack;

	vector<int> RHS_demand;

	vector<int> objCoeffs; 
	*/
	vector<int*> LHS_knapsack;

	vector<int*> LHS_demand;

	int *RHS_knapsack;

	int *RHS_demand;

	int *objCoeffs;

    int cLen = 0;

	int kRows = 0;

	int dRows = 0;
	
};

ProblemCoefficients::ProblemCoefficients(int length, int kRowNum, int dRowNum) {
	cLen = length;
	kRows = kRowNum;
	dRows = dRowNum;
	RHS_knapsack = new int[kRows];
	RHS_demand = new int[dRows];
	objCoeffs = new int[cLen];
	
}

ProblemCoefficients::~ProblemCoefficients() {
	for (int i = 0; i < kRows; i++)
		delete []LHS_knapsack[i];
	for (int i = 0; i < dRows; i++)
		delete []LHS_demand[i];
	delete []RHS_knapsack;
	delete []RHS_demand;
	delete []objCoeffs;
}


void ProblemCoefficients::loadCoeffs(vector<vector<int>> Ak, vector<vector<int>> Ad, vector<int> Bk, vector<int> Bd, vector<int> C) {
	
	for (int i = 0; i < kRows; i++) {
		int* kRow;
		kRow = new int[cLen];
		for (int j = 0; j < cLen; j++)
			kRow[j] = Ak[i][j];
		LHS_knapsack.push_back(kRow);
	}

	for (int i = 0; i < dRows; i++) {
		int* dRow;
		dRow = new int[cLen];
		for (int j = 0; j < cLen; j++)
			dRow[j] = Ad[i][j];
		LHS_demand.push_back(dRow);
	}

	for (int i = 0; i < kRows; i++) {
		RHS_knapsack[i] = Bk[i];
	}

	for (int i = 0; i < dRows; i++) {
		RHS_demand[i] = Bd[i];
	}

	for (int i = 0; i < cLen; i++) {
		objCoeffs[i] = C[i];
	

	}

	
}
vector<int*> ProblemCoefficients::getAk() {
	return LHS_knapsack;
}

vector<int*> ProblemCoefficients::getAd() {
	return LHS_demand;
}

int* ProblemCoefficients::getBk() {
	return RHS_knapsack;
}

int* ProblemCoefficients::getBd() {
	return RHS_demand;
}

int* ProblemCoefficients::getC() {
	return objCoeffs;
}

int ProblemCoefficients::getLen() {
	return cLen;
}

int ProblemCoefficients::getKRows() {
	return kRows;
}

int ProblemCoefficients::getDRows() {
	return dRows;
}

void ProblemCoefficients::printC() {
	for (int i = 0; i < cLen; i++)
		cout << objCoeffs[i] << ' ';
	cout << endl;
}

void ProblemCoefficients::printAk() {
	for (int i = 0; i < kRows; i++) {
		for (int j = 0; j < cLen; j++)
			cout << LHS_knapsack[i][j] << ' ';
		cout << endl;
	cout << endl;
	}	
}

void ProblemCoefficients::printAd() {
	for (int i = 0; i < dRows; i++) {
		for (int j = 0; j < cLen; j++)
			cout << LHS_demand[i][j] << ' ';
		cout << endl;
		cout << endl;
	}
}

void ProblemCoefficients::printBk() {
	for (int i = 0; i < kRows; i++) {
		cout << RHS_knapsack[i] << ' ';
	}
	cout << endl << endl;
}

void ProblemCoefficients::printBd() {
	for (int i = 0; i < dRows; i++) {
		cout << RHS_demand[i] << ' ';
	}
	cout << endl << endl;
}