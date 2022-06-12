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

vector<vector<int>> readLHS(ifstream& LHSfile, string fileName, int length, int numRows);

vector<int> readCoeffs(ifstream &file, string fileName);

Solution exploreSpaces(Solution& solution, ProblemCoefficients& coeff,
    TabuList& tabuList, vector<vector<int>>& pairs, int multi);

vector<int> tabuSearch(ProblemCoefficients& coeff, TabuList& tabuList, vector<vector<int>>& pairs, int multi, int iterations);

int main()
{
    
    int numVars = 100;

    int numRowsK = 30;

    int numRowsD = 30;

    ifstream inf;

    // a set of five files for an MDMKP problem
    // two files for the LHS, two for the RHS, one for the objective function coeff's
    string file1 = "MDMKPcase3&6_all/MDMKP_1_3/LHSknapsack_1_3.csv";
    string file2 = "MDMKPcase3&6_all/MDMKP_1_3/LHSdemand_1_3.csv";
    string file3 = "MDMKPcase3&6_all/MDMKP_1_3/RHSknapsack_1_3.csv";
    string file4 = "MDMKPcase3&6_all/MDMKP_1_3/RHSdemand_1_3.csv";
    string file5 = "MDMKPcase3&6_all/MDMKP_1_3/ObjCoeffs_1_3.csv";
    
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
    vector<vector<int>> Ak1 = readLHS(inf, file1, numVars, numRowsK);
    vector<vector<int>> Ad1 = readLHS(inf, file2, numVars, numRowsD);
    vector<int> Bk1 = readCoeffs(inf, file3);
    vector<int> Bd1 = readCoeffs(inf, file4);
    vector<int> C = readCoeffs(inf, file5);

    // create a ProblemCoefficients object to store the MDMKP coeff's
    // populate with the coefficients extracted from files
    ProblemCoefficients Prob(numVars, numRowsK, numRowsD);
    Prob.loadCoeffs(Ak1, Ad1, Bk1, Bd1, C);

    // create a Solution object 
    Solution *Sol1 = new Solution(numVars);
    

    // generate a list of all the pairs of indices of a solution 
    // to explore the swap space (pairList size is 'n choose 2')
    vector<vector<int>> pairList = Sol1->createPairList();
    cout << "no. pairs: " << pairList.size() << endl << endl;

    // create the empty Tabu list using three hash vectors of length l
    // multipliers y1, y2, and y3 to prevent collisions
    double y1 = 1.5;
    double y2 = 1.8;
    double y3 = 2.2;
    int l = 100000000;

    TabuList Tabu(l, y1, y2, y3);
    cout << endl << "solution tabu? " << Tabu.checkTabu(*Sol1) << endl;

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
    Sol1->K_Solution_Gen(5);
    Sol1->printSolution();
    cout << endl << Sol1->getZ() << "  " << "  "  << endl;
    Solution solCopy = *Sol1; 
    cout << solCopy.getZ() << "  " << solCopy.calcZ(Prob) << "  " << solCopy.getZ() << endl << endl;

    cout << Sol1->getZ() << endl << endl;
    Sol1->violAmounts(Prob);
    cout << Sol1->getP() << endl << endl;

    delete(Sol1);

    
    
    /////////////////////////////////////////////////
    // automating runs of tabu search algorithm
    // experimenting with multiplier values and number of iterations

    vector<int> multipliers = { 10, 100, 200};
    vector<int> iterCounts = { 5, 10, 20 };
    vector<vector<int>> searchResults;
    //Solution Start(100);

    //int multiplier = 10;
    //tabuSearch(Start, Prob, Tabu, pairList, multiplier, 500);

    
    for (int i = 0; i < multipliers.size(); i++)
        for (int j = 0; j < iterCounts.size(); j++)
            searchResults.push_back(tabuSearch(Prob, Tabu, pairList, multipliers[i], iterCounts[j]));

    cout << endl << "results:" << endl << endl;
    for (int i = 0; i < multipliers.size(); i++)
        for (int j = 0; j < iterCounts.size(); j++)
            cout << multipliers[i] << "   " << iterCounts[j]
            << "   " << searchResults[i + j + (i * 2)][0] 
            << "   " << searchResults[i + j + (i * 2)][1] << endl;
    


    
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

// extracts LHS coefficients
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

// solution-based tabu search algorithm 
// based on algorithm from Lai et al, 2019
vector<int> tabuSearch(ProblemCoefficients& coeff, TabuList& tabuList, vector<vector<int>>& pairs, int multi, int iterations) {
    auto start = chrono::high_resolution_clock::now();
    vector<int> returnVals = { 0, 0 };
    tabuList.clearTabuList();

    //Solution *Init = new Solution(100);
    Solution Init(100);
    int multiplier = multi;
    Init.K_Solution_Gen(25);
    //Init.generate();

    Init.setMulti(multiplier);

    Solution bestSol(Init);
    //bestSol = init;
    Solution nextSol(Init);
    //nextSol = init;
    Solution bestFeas(Init);
    //bestFeas = init;
    bestFeas.violAmounts(coeff);
    tabuList.insertTabu(bestSol);
    tabuList.insertTabu(nextSol);
    int count = 0;
    cout << "here" << endl;

    while (count < iterations) {
        //Solution newSol = init;
        //newSol.setMulti(multi);
        //Solution Result(100);
        Solution Result(exploreSpaces(nextSol, coeff, tabuList, pairs, multi));
        Solution newSol(Result);
        //newSol = Result;
        
        newSol.violAmounts(coeff);
        bestSol.violAmounts(coeff);
        bestFeas.violAmounts(coeff);
        if (newSol.evalFit(coeff) > bestSol.evalFit(coeff)) {
            if (!tabuList.checkTabu(newSol)) {
                bestSol = newSol;
                nextSol = newSol;
                if (newSol.isFeasible()) {
                    bestFeas = newSol;
                    cout << "feasible" << endl;
                }
                tabuList.insertTabu(newSol);
                cout << "improve  " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(coeff) << " " << endl;
            }
            else {
                nextSol = newSol;
                cout << "improve  " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(coeff) << " Tabu" << endl;
            }
        }
        else if (newSol.evalFit(coeff) <= bestSol.evalFit(coeff)) {

            if (!tabuList.checkTabu(newSol)) {
                nextSol = newSol;
                tabuList.insertTabu(newSol);
                if (newSol.isFeasible()) {
                    if (newSol.evalFit(coeff) > bestFeas.evalFit(coeff))
                        bestFeas = newSol;
                    cout << "feasible" << endl;
                }
                cout << "no improve " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(coeff) << endl;
            }
            else {
                nextSol = newSol; 
                cout << "no improve " << newSol.getZ() << "  " << newSol.getP() << "  " << newSol.evalFit(coeff) << " Tabu" << endl;
            }

        }

        cout << count << endl;
        count++;
    }
    
    auto finish = chrono::high_resolution_clock::now();
    auto ticks = chrono::duration_cast<chrono::microseconds>(finish - start);
    double result = ticks.count() / 1000000.0;
    cout << "multiplier: " << multi << "    " << "iterations: " << iterations << "    " << "time in seconds : " << result << endl;
    cout << "best Z: " << bestSol.getZ() << endl;       
    bestFeas.violAmounts(coeff);
    if (bestFeas.isFeasible()) {
        cout << "best feasible Z: " << bestFeas.calcZ(coeff) << "      " << "k: " << bestFeas.calcK() << endl;
        bestFeas.printSolution();
        returnVals[0] = bestFeas.getZ();
        returnVals[1] = bestFeas.calcK();
        return returnVals;
    }
    else
        cout << "no feasible solution found" << endl;
    return returnVals;


}

// performs the search of the flip space and swap space for a given solution
// returning the best solution
// Tabu list enabled
Solution exploreSpaces(Solution& Sol, ProblemCoefficients& coeffs, TabuList& tList, vector<vector<int>> &pairs, int multi) {
    
    int size = Sol.getLength();
    Solution Best(size);
    Best.clearSolution();
    Best.setMulti(multi);
    cout << "here" << endl;
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
