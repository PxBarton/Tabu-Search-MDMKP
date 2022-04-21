#pragma once
#include <iostream>


using namespace std;

/**
 * class to create a tabu list for tabu search
 * using three hash vectors
 */
class TabuList {
public:
	/**
	 * constructor
	 * initializes the hash vectors and sets size = length
	 * multipliers set to 1.0
	 */
	TabuList(int length);

	/**
	 * constructor
	 * initializes the hash vectors and sets size = length
	 * sets multipliers as mult1, mult2, mult3
	 */
	TabuList(int length, double mult1, double mult2, double mult3);

	/**
	 * destructor
	 * destroys hash vectors
	 */
	~TabuList();

	/**
	 * hash function as provided in the Lei et al 2019 paper
	 */
	int hashFunction(Solution& sol, double multi);

	/**
	 * processes solution using hash function three times
	 * with a different multiplier 
	 */
	void insertTabu(Solution& sol);

	/** 
	 * check if a given solution is tabu
	 */
	bool checkTabu(Solution& sol);

	void setMultipliers(double mult1, double mult2, double mult3);

	void setListLength(int length);

	void clearTabuList();

private:

	int listLength;

	double multiplier1;

	double multiplier2;

	double multiplier3;

	bool *hashVector1;

	bool *hashVector2;

	bool *hashVector3;


};

TabuList::TabuList(int length) {
	listLength = length;
	multiplier1 = 1;
	multiplier2 = 1;
	multiplier3 = 1;
	hashVector1 = new bool[length];
	hashVector2 = new bool[length];
	hashVector3 = new bool[length];
}

TabuList::TabuList(int length, double mult1, double mult2, double mult3) {
	listLength = length;
	multiplier1 = mult1;
	multiplier2 = mult2;
	multiplier3 = mult3;
	hashVector1 = new bool[length];
	hashVector2 = new bool[length];
	hashVector3 = new bool[length];
}

TabuList::~TabuList() {
	delete[]hashVector1;
	delete[]hashVector2;
	delete[]hashVector3;
}

int TabuList::hashFunction(Solution& sol, double multi) {
	int hashIndex;
	int total = 0;
	for (int i = 0; i < sol.getLength(); i++)
		total = total + (floor(pow(i, multi)) * sol.getXval(i));
	hashIndex = static_cast<int>(total) % listLength;
	return hashIndex;
}

void TabuList::insertTabu(Solution& sol) {
	int index1;
	int index2;
	int index3;
	index1 = this->hashFunction(sol, multiplier1);
	index2 = this->hashFunction(sol, multiplier2);
	index3 = this->hashFunction(sol, multiplier3);
	hashVector1[index1] = 1;
	hashVector2[index2] = 1;
	hashVector3[index3] = 1;
}

bool TabuList::checkTabu(Solution& sol) {
	if (hashVector1[this->hashFunction(sol, multiplier1)] == 1
		&& hashVector2[this->hashFunction(sol, multiplier2)] == 1
		&& hashVector3[this->hashFunction(sol, multiplier3)] == 1)
		return true;
	else
		return false;
}

void TabuList::setMultipliers(double mult1, double mult2, double mult3) {
	multiplier1 = mult1;
	multiplier2 = mult2;
	multiplier3 = mult3;
}

void TabuList::setListLength(int length) {
	listLength = length;
	hashVector1 = new bool[length];
	hashVector2 = new bool[length];
	hashVector3 = new bool[length];
}

void TabuList::clearTabuList() {
	for (int i = 0; i < listLength; i++) {
		hashVector1[i] = 0;
		hashVector2[i] = 0;
		hashVector3[i] = 0;
	}
}

