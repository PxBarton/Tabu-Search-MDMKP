// Knapsack2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <valarray>
#include <chrono>
#include <random>
#include "ProblemCoefficients.h"
#include "Solution.h"
#include "TabuList.h"


using namespace std;

bool openFile(ifstream& file, string fileName);

vector<vector<int>> readLHS(ifstream& LHSfile, string fileName, int length);

vector<int> readCoeffs(ifstream &file, string fileName);

Solution exploreSpaces(Solution& solution, ProblemCoefficients& coeff, 
    TabuList& tabuList, vector<vector<int>>& pairs);

Solution exploreSpacesNoTabu(Solution& solution, ProblemCoefficients& coeffs, vector<vector<int>>& pairs);


//Solution K_Solution_Gen(int K);

int main()
{
    
    int numVars = 100;

    int numRowsK = 15;

    int numRowsD = 15;

    ifstream inf;

    // a set of five files for an MDMKP problem
    // two files for the LHS, two for the RHS, one for the objective function coeff's
    string file1 = "case11_3/LHSknapsack2.csv";
    string file2 = "case11_3/LHSdemand11_3.csv";
    string file3 = "case11_3/RHSknapsack11_3.csv";
    string file4 = "case11_3/RHSdemand11_3.csv";
    string file5 = "case11_3/ObjCoeffs11_3.csv";
    
    /*
    string file1 = "MDMKP/LHS_k.txt";
    string file2 = "MDMKP/LHS_d.txt";
    string file3 = "MDMKP/RHS_k.txt";
    string file4 = "MDMKP/RHS_d.txt";
    string file5 = "MDMKP/objFuncC.txt";
    */

    // a small sample set of MDMKP coeff's
    vector<vector<int>> A_k = { {1, 2, 3, 4, 5, 6, 7, 8}, {5, 6, 7, 8, 9, 10, 11, 12} };
    vector<vector<int>> A_d = { {2, 4, 6, 8, 10, 12, 14, 16}, {10, 12, 14, 16, 18, 20, 22, 24} };
    vector<int> B_k = { 50, 30 };
    vector<int> B_d = { 80, 20 };
    vector<int> C_obj = { 11, 22, 33, 44, 55, 66, 77, 88};
    vector<int> testSol;

    // set up random number generator for testing a small instance
    double seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine eng(seed);
    uniform_int_distribution<int> dis(0, 500);

    for (int k = 0; k < numVars; k++) {
        int entry = dis(eng);
        testSol.push_back(entry);
    }
    
    // vectors to hold coefficients extracted from files
    vector<vector<int>> Ak1 = readLHS(inf, file1, numVars);
    vector<vector<int>> Ad1 = readLHS(inf, file2, numVars);
    vector<int> Bk1 = readCoeffs(inf, file3);
    vector<int> Bd1 = readCoeffs(inf, file4);
    vector<int> C = readCoeffs(inf, file5);

    // create a ProblemCoefficients object to store the MDMKP coeff's
    // populate with the coefficients extracted from files
    ProblemCoefficients Prob;
    Prob.loadCoeffs(Ak1, Ad1, Bk1, Bd1, C, numRowsK, numRowsD);

    // create a Solution object 
    Solution Sol(numVars);
    

    // generate a list of all the pairs of indices of a solution 
    // to explore the swap space (pairList size is 'n choose 2')
    vector<vector<int>> pairList = Sol.createPairList();
    cout << "no. pairs: " << pairList.size() << endl << endl;

    // create the empty Tabu list using three hash vectors of length l
    // multipliers y1, y2, and y3 to prevent collisions
    double y1 = 1.5;
    double y2 = 1.8;
    double y3 = 2.2;
    int l = 100000000;

    TabuList Tabu(l, y1, y2, y3);
    cout << endl << "solution tabu? " << Tabu.checkTabu(Sol) << endl;

    cout << endl << "RHS knapsack rows in file: " << Bk1.size() << endl;
    cout << endl << "RHS demand rows in file: " << Bd1.size() << endl;
    cout << endl << "LHS cols: " << Ak1[0].size() << endl;
    cout << endl;
    
    cout << "Knapsack LHS --------------------------------" << endl << endl;
    Prob.printAk();
    cout << "Demand LHS ----------------------------------" << endl << endl;
    Prob.printAd();
    cout << "Objective Function Coeffs -------------------" << endl << endl;
    Prob.printC();
    cout << endl;
    cout << "Knapsack RHS --------------------------------" << endl << endl;
    Prob.printBk();
    cout << endl;
    cout << "Demand RHS ----------------------------------" << endl << endl;
    Prob.printBd();
    cout << endl;


    // generate a random solution, vector of 0's and 1's, length = numVars 
    //Sol.generate();
    Sol.K_Solution_Gen(5);
    Sol.printSolution();
    cout << endl << Sol.getZ() << "  " << Sol.calcZ(Prob) << "  " << Sol.getZ() << endl;
    Solution solCopy = Sol; 
    cout << solCopy.getZ() << "  " << solCopy.calcZ(Prob) << "  " << solCopy.getZ() << endl << endl;

    cout << Sol.calcZ(Prob) << endl << endl;

    

    /////////////////////////////////////////////////
    // Tabu search algorithm 
    // based on algorithm from Lai et al, 2019

    auto start6 = chrono::high_resolution_clock::now();

    Solution Init(numVars);
    Init.K_Solution_Gen(26);
    //Init.generate();

    Init.setMulti(10);

    Solution bestSol = Init;
    Solution nextSol = Init;
    Solution bestFeas = Init;
    bestFeas.violAmounts(Prob);
    Tabu.insertTabu(bestSol);
    Tabu.insertTabu(nextSol);
    int count = 0;

    cout << "bestSol multiplier: " << bestSol.getMulti() << endl << endl;

    // set 'true' for Tabu search using Tabu list of hash vectors
    // set 'false' for local search without Tabu list
    bool useTabuList = true;

    while (count < 100) {
        Solution newSol(numVars);
        if (useTabuList == true) {
            Solution Result = exploreSpaces(nextSol, Prob, Tabu, pairList);
            newSol = Result;
        }
        else {
            Solution Result = exploreSpacesNoTabu(nextSol, Prob, pairList);
            newSol = Result;
        }
        newSol.violAmounts(Prob);
        bestSol.violAmounts(Prob);
        bestFeas.violAmounts(Prob);
        if (newSol.evalFit(Prob) > bestSol.evalFit(Prob)) {
            if (useTabuList == true) {
                if (!Tabu.checkTabu(newSol)) {
                    bestSol = newSol;
                    nextSol = newSol;
                    if (newSol.isFeasible()) {
                        bestFeas = newSol;
                        cout << "feasible" << endl;
                    }
                    Tabu.insertTabu(newSol);
                    cout << "improve  " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(Prob) << endl;
                }
                else
                    cout << "improve  " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(Prob) << " Tabu" << endl;
            }
            else {
                bestSol = newSol;
                nextSol = newSol;
                if (newSol.isFeasible()) {
                    bestFeas = newSol;
                    cout << "feasible" << endl;
                }
                cout << "improve  " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(Prob) << endl;
            }

        }
        else if (newSol.evalFit(Prob) <= bestSol.evalFit(Prob)) {
            if (useTabuList == true) {
                if (!Tabu.checkTabu(newSol)) {
                    nextSol = newSol;
                    Tabu.insertTabu(newSol);
                    if (newSol.isFeasible()) {
                        if (newSol.evalFit(Prob) > bestFeas.evalFit(Prob))
                            bestFeas = newSol;      // best feasible sol might be less than bestSol
                        cout << "feasible" << endl;
                    }
                    cout << "no improve " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(Prob) << endl;
                }
                else 
                    cout << "no improve " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(Prob) << " Tabu" << endl;
            }
            else {
                nextSol = newSol;
                if (newSol.isFeasible()) {
                    if (newSol.evalFit(Prob) > bestFeas.evalFit(Prob))
                        bestFeas = newSol;
                    cout << "feasible" << endl;
                }
                cout << "no improve " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(Prob) << endl;
            }
        }
        //else
            //break;
            //cout << "no improve " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(Prob) << endl;
        cout << count << endl;
        count++;

    }
    cout << "best Z: " << bestSol.getZ() << endl;       // might be less than best feasible Z due to penalty
    bestFeas.violAmounts(Prob);
    if (bestFeas.isFeasible()) {
        cout << "best feasible Z: " << bestFeas.calcZ(Prob) << "      " << "k: " << bestFeas.calcK() << endl;
        bestFeas.printSolution();
    }
    else
        cout << "no feasible solution found" << endl;

    auto finish6 = chrono::high_resolution_clock::now();
    auto ticks6 = chrono::duration_cast<chrono::microseconds>(finish6 - start6);
    double result6 = ticks6.count() / 1000000.0;
    if (useTabuList == true)
        cout << endl << "Using Tabu List" << endl;
    else
        cout << endl << "Not using Tabu List" << endl;
    cout << numRowsK << " and " << numRowsD << ": time in seconds : " << result6 << endl;
    //cout << "bestSol multiplier: " << bestSol.getMulti() << endl << endl;
}

//------------------------------------------------------------------------------
// end Main


// opens a file of coefficients
bool openFile(ifstream& file, string fileName) {
    file.open(fileName.c_str());
    if (file.fail())
        return(false);
    return(true);
}

// extracts zero or more rows of LHS coefficients
vector<vector<int>> readLHS(ifstream& LHSfile, string fileName, int length) {
    vector<vector<int>> tempMatrix;
    vector<int> rowVector;
    vector<int> tempRow;
    string token;

    openFile(LHSfile, fileName);
    LHSfile.seekg(0, LHSfile.end);
    int tokenCount = LHSfile.tellg();
    LHSfile.seekg(0, LHSfile.beg);
    int coeffCount = 0;
    int numRows = tokenCount / length;

    for (int i = 0; i < numRows; i++) {
        while (LHSfile >> token && coeffCount < length) {
            tempRow.push_back(stoi(token));
            coeffCount++;
            //cout << token << endl;
        }
        rowVector = tempRow;
        tempMatrix.push_back(rowVector);
        tempRow.clear();
        coeffCount = 0;
    }
    LHSfile.close();
    return tempMatrix;
}

// extracts RHS and objective function coefficients
vector<int> readCoeffs(ifstream &file, string fileName) {
    vector<int> temp;
    openFile(file, fileName);
    string token;
    while (file >> token) {
        temp.push_back(stoi(token));
    }
    file.close();
    return temp;
}


// performs the search of the flip space and swap space for a given solution
// returning the best solution
// Tabu list enabled
Solution exploreSpaces(Solution& Sol, ProblemCoefficients& coeffs, TabuList& tList, vector<vector<int>> &pairs) {
    
    int size = Sol.getLength();
    Solution Best(size);
    Best.clearSolution();
    
    for (int i = 0; i < size; i++) {
        Sol.flipBit(i);
        if (!tList.checkTabu(Sol)) {
            Sol.violAmounts(coeffs);
            Best.violAmounts(coeffs);
            if (Sol.evalFit(coeffs) > Best.evalFit(coeffs))
                Best = Sol;
        }
        //else
            //cout << "Tabu solution found" << endl;
        Sol.flipBit(i);
    }
    for (int i = 0; i < pairs.size(); i++) {
        Sol.swapBit(pairs[i]);
        if (!tList.checkTabu(Sol)) {
            Sol.violAmounts(coeffs);
            Best.violAmounts(coeffs);
            if (Sol.evalFit(coeffs) > Best.evalFit(coeffs))
                Best = Sol;
        }
        //else 
            //cout << "Tabu solution found" << endl;
        Sol.swapBit(pairs[i]);
    }
    return Best;
}

// performs the search of the flip space and swap space for a given solution
// returning the best solution
// Tabu list disabled
Solution exploreSpacesNoTabu(Solution& Sol, ProblemCoefficients& coeffs, vector<vector<int>>& pairs) {

    int size = Sol.getLength();
    Solution Best(size);
    Best.clearSolution();

    for (int i = 0; i < size; i++) {
        Sol.flipBit(i);
        Sol.violAmounts(coeffs);
        Best.violAmounts(coeffs);
        if (Sol.evalFit(coeffs) > Best.evalFit(coeffs))
            Best = Sol;

        Sol.flipBit(i);
    }
    for (int i = 0; i < pairs.size(); i++) {
        Sol.swapBit(pairs[i]);
        Sol.violAmounts(coeffs);
        Best.violAmounts(coeffs);
        if (Sol.evalFit(coeffs) > Best.evalFit(coeffs))
            Best = Sol;

        Sol.swapBit(pairs[i]);
    }
    return Best;
}

// alternate version for extracting LHS coefficients from csv files
// different signature and needs testing with txt files
/*
vector<vector<int>> readLHS(ifstream& LHSfile, string fileName, int length, int numRows) {
    vector<vector<int>> tempMatrix;
    vector<int> rowVector;
    string tempRow;
    string line;
    int rowCount = 0;

    openFile(LHSfile, fileName);

    while (getline(LHSfile, line, '\r') && rowCount < numRows) {
        string token;
        stringstream sin(line);
        rowVector.clear();
        while (sin >> token) {
            if (rowVector.size() < length)
                rowVector.push_back(stoi(token));
        }
        tempMatrix.push_back(rowVector);
        rowCount++;

    }

    LHSfile.close();
    return tempMatrix;
*/

//--------------------------------------------------------------------------

// 30 seconds for 1,000,000 dot products 100 variables
// using vectors and vector<bool> for solution
// 2 seconds for 1,000,000 dots with vector<int> solution

////////////////////////////////////////
    // timing tests of various processes

    /*
    auto start = chrono::high_resolution_clock::now();

    int i = 0;
    while (i < 1000000) {
        Sol.calcZ(Prob);
        i++;
    }

    auto finish = chrono::high_resolution_clock::now();
    auto ticks = chrono::duration_cast<chrono::microseconds>(finish - start);
    double result = ticks.count() / 1000000.0;
    cout << endl <<  "compute z, 100000 times: time in seconds: " << result  << endl;

    vector<vector<int>> allPairs = Sol.createPairList();

    auto start3 = chrono::high_resolution_clock::now();

    for (int i = 0; i < allPairs.size(); i++)
        Sol.swapBit(allPairs[i]);

    auto finish3 = chrono::high_resolution_clock::now();
    auto ticks3 = chrono::duration_cast<chrono::microseconds>(finish3 - start3);
    double result3 = ticks3.count() / 1000000.0;
    cout << endl << "swapping elements: time in seconds: " << result3 << endl;

    Sol.violAmounts(Prob);
    cout << Sol.evalFit(Prob) << endl;
    cout << "Z: " << Sol.getZ() << endl;
    cout << Sol.isFeasible() << endl;

    auto start2 = chrono::high_resolution_clock::now();

    int l = 100000000;
    TabuList Tabu(l);

    auto finish2 = chrono::high_resolution_clock::now();
    auto ticks2 = chrono::duration_cast<chrono::microseconds>(finish2 - start2);
    double result2 = ticks2.count() / 1000000.0;
    cout << endl << "generating hash vectors: time in seconds: " << result2 << endl;

    Tabu.insertTabu(Sol);
    cout << Tabu.checkTabu(Sol) << endl;

    auto start4 = chrono::high_resolution_clock::now();

    for (int i = 0; i < 10000; i++)
        Tabu.checkTabu(Sol);

    auto finish4 = chrono::high_resolution_clock::now();
    auto ticks4 = chrono::duration_cast<chrono::microseconds>(finish4 - start4);
    double result4 = ticks4.count() / 1000000.0;
    cout << endl << "checking Tabu List: time in seconds: " << result4 << endl;

    auto start5 = chrono::high_resolution_clock::now();

    exploreSpaces(Sol, Prob, Tabu, pairList);

    auto finish5 = chrono::high_resolution_clock::now();
    auto ticks5 = chrono::duration_cast<chrono::microseconds>(finish5 - start5);
    double result5 = ticks5.count() / 1000000.0;
    cout << endl << "1 explore space: time in seconds: " << result5 << endl;


    for (int i = 0; i < Ak1.size(); i++) {
        for (int j = 0; j < Ak1[i].size(); j++)
            cout << Ak1[i][j] << ' ';
        cout << endl;
    }


    for (int i = 0; i < Bk1.size(); i++)
        cout << Bk1[i] << ' ';

    cout << endl;
    Solution Sol2(numVars);
    Sol2.printSolution();
    Sol.printSolution();
    Sol2 = Sol;
    Sol2.printSolution();
    */

//--------------------------------------------------------------------------

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
