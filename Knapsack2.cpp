// Knapsack2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Knapsack.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
    TabuList& tabuList, vector<vector<int>> pairs);



int main()
{
    double seed = chrono::system_clock::now().time_since_epoch().count();
    //random_device rd;
    //mt19937 eng(rd());
    default_random_engine eng(seed);
    uniform_int_distribution<int> dis(0, 500);

    int numVars = 100;

    int numRowsK;

    int numRowsD;

    ifstream inf;

    
    string file1 = "coeffs2/LHSknapsack2.txt";
    string file2 = "coeffs2/LHSdemand2.txt";
    string file3 = "coeffs2/RHSknapsack2.csv";
    string file4 = "coeffs2/RHSdemand2.csv";
    string file5 = "coeffs2/ObjCoeffs2.csv";
    
    /*
    string file1 = "MDMKP/LHS_k.txt";
    string file2 = "MDMKP/LHS_d.txt";
    string file3 = "MDMKP/RHS_k.txt";
    string file4 = "MDMKP/RHS_d.txt";
    string file5 = "MDMKP/objFuncC.txt";
    */

    vector<vector<int>> A_k = { {1, 2, 3, 4, 5, 6, 7, 8}, {5, 6, 7, 8, 9, 10, 11, 12} };
    vector<vector<int>> A_d = { {2, 4, 6, 8, 10, 12, 14, 16}, {10, 12, 14, 16, 18, 20, 22, 24} };
    vector<int> B_k = { 50, 30 };
    vector<int> B_d = { 80, 20 };
    vector<int> C_obj = { 11, 22, 33, 44, 55, 66, 77, 88};
    vector<int> fake;

    vector<vector<int>> Ak1 = readLHS(inf, file1, numVars);
    vector<vector<int>> Ad1 = readLHS(inf, file2, numVars);
    vector<int> Bk1 = readCoeffs(inf, file3);
    vector<int> Bd1 = readCoeffs(inf, file4);
    vector<int> C = readCoeffs(inf, file5);

    for (int k = 0; k < numVars; k++) {
        int entry = dis(eng);
        fake.push_back(entry);
    }
    //eng.reset();

    ProblemCoefficients Prob;
    Prob.loadCoeffs(Ak1, Ad1, Bk1, Bd1, C);

    Solution Sol(numVars);
    vector<vector<int>> pairList = Sol.createPairList();
    cout << "no. pairs: " << pairList.size() << endl;
   
    //Prob.printAk();
    //Prob.printC();
    //cout << endl;

    Sol.generate();
    //Sol.printSolution();


    cout << Sol.calcZ(Prob);

    auto start = chrono::high_resolution_clock::now();

    int i = 0;
    while (i < 200) {
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
    

    cout << endl << "RHS knapsack rows: " << Bk1.size() << endl;
    cout << endl << "RHS demand rows: " << Bd1.size() << endl;
    cout << endl << "LHS cols: " << Ak1[0].size() << endl;
    cout << endl;

    /*
    for (int i = 0; i < Ak1.size(); i++) {
        for (int j = 0; j < Ak1[i].size(); j++)
            cout << Ak1[i][j] << ' ';
        cout << endl;
    }
    */

    for (int i = 0; i < Bk1.size(); i++)
        cout << Bk1[i] << ' ';

    cout << endl;
    Solution Sol2(numVars);
    Sol2.printSolution();
    Sol.printSolution();
    Sol2 = Sol;
    Sol2.printSolution();
}

//------------------------------------------------------------------------------

bool openFile(ifstream& file, string fileName) {
    file.open(fileName.c_str());
    if (file.fail())
        return(false);
    return(true);
}

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

Solution exploreSpaces(Solution& Sol, ProblemCoefficients& coeffs, TabuList& tList, vector<vector<int>> pairs) {
    // Solution Init(solutionSize);
    // Init = Sol;
    int size = Sol.getLength();
    Solution Best(size);
    Best.clearSolution();
    // tList.insertTabu(Init);
    for (int i = 0; i < size; i++) {
        Sol.flipBit(i);
        if (!tList.checkTabu(Sol)) {
            Sol.violAmounts(coeffs);
            Best.violAmounts(coeffs);
            if (Sol.evalFit(coeffs) > Best.evalFit(coeffs))
                Best = Sol;
        }
        Sol.flipBit(i);
    }
    for (int i = 0; i < 100; i++) {
        Sol.swapBit(pairs[i]);
        if (!tList.checkTabu(Sol)) {
            Sol.violAmounts(coeffs);
            Best.violAmounts(coeffs);
            if (Sol.evalFit(coeffs) > Best.evalFit(coeffs))
                Best = Sol;
        }
        Sol.swapBit(pairs[i]);
    }
    
    auto start = chrono::high_resolution_clock::now();

    Sol.swapBit(pairs[5]);
    if (!tList.checkTabu(Sol)) {
        Sol.violAmounts(coeffs);
        Best.violAmounts(coeffs);
        if (Sol.evalFit(coeffs) > Best.evalFit(coeffs))
            Best = Sol;
    }
    Sol.swapBit(pairs[5]);

    auto finish = chrono::high_resolution_clock::now();
    auto ticks = chrono::duration_cast<chrono::microseconds>(finish - start);
    double result = ticks.count() / 1000000.0;
    cout << endl << "swap and checks: time in seconds: " << result << endl;
    

    auto start2 = chrono::high_resolution_clock::now();

    for (int i = 0; i < 100; i++) {
        Sol.swapBit(pairs[i]);
        if (!tList.checkTabu(Sol)) {
            Sol.violAmounts(coeffs);
            Best.violAmounts(coeffs);
            if (Sol.evalFit(coeffs) > Best.evalFit(coeffs))
                Best = Sol;
        }
        Sol.swapBit(pairs[i]);
    }

    auto finish2 = chrono::high_resolution_clock::now();
    auto ticks2 = chrono::duration_cast<chrono::microseconds>(finish2 - start2);
    double result2 = ticks2.count() / 1000000.0;
    cout << endl << "100 swaps and checks: time in seconds: " << result2 << endl;

    return Best;
}





//--------------------------------------------------------------------------

// 30 seconds for 1,000,000 dot products 100 variables
// using vectors and vector<bool> for solution
// 2 seconds for 1,000,000 dots with vector<int> solution



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
