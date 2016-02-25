//
//  nonTerminalRule.cpp
//  Project3
//
//  Created by Frank Palmisano on 2/19/16.
//  Copyright © 2016 Frank Palmisano. All rights reserved.
//

#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <set>

using namespace std;

class nonTerminalRule {
public:
    
    string nonTerminal;
    int index;
    vector<vector<string> > rhsVector;
    vector<string> firstSet;
    vector<string> followSet;
    
    nonTerminalRule(char* nonTerminal, int index) {
        this->nonTerminal = nonTerminal;
        this->index = index;
    }
    
    void addRHS(vector<string> rhsVec) {
        rhsVector.push_back(rhsVec);
    }
};
//            for (int i = 0; i < ruleSet.size(); i++) {
//                for (int j = 0; j < ruleSet[i].rhsVector.size(); j++) {
//                    cout << ruleSet[i].nonTerminal << " -> ";
//                    for (int k = 0; k < ruleSet[i].rhsVector[j].size(); k++) {
//                        cout << ruleSet[i].rhsVector[j][k] << " ";
//                    }
//                    cout << endl;
//                }
//            }

// Rule 3.
//            for (int j = 0; j < ruleSet[i].rhsVector.size(); j++)
//            {
//                if (isNonTerminal(ruleSet[i].rhsVector[j].front()))
//                {
//                    int index = getIndex(ruleSet[i].rhsVector[j].front());
//                    for (int k = 0; k < ruleSet[index].firstSet.size(); k++)
//                    {
//                        if (ruleSet[index].firstSet[k] != "#")
//                        {
//                            ruleSet[i].firstSet.push_back(ruleSet[index].firstSet[k]);
//                        }
//                    }
//                }
//            }

//for (int k = 0; k < (int)ruleSet[i].rhsVector[j].size() ; k++)
//{
//    if (isNonTerminal(ruleSet[i].rhsVector[j][k]))
//    {
//        if (isEpsilon(ruleSet[i].rhsVector[j][k]))
//        {
//            int index = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
//            int setIndexSize = (int)ruleSet[index].firstSet.size();
//            for (int l = 0; l < setIndexSize; l++) {
//                if (ruleSet[index].firstSet[l] != "#") {
//                    it1 = find(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end(), ruleSet[index].firstSet[l]);
//                    if (it1 == ruleSet[i].firstSet.end()) {
//                        string temp = ruleSet[index].firstSet[l];
//                        ruleSet[i].firstSet.push_back(temp);
//                        changed = true;
//                    }
//                }
//            }
//        }
//    }
//}


//
//for (int j = 0; j < (int)ruleSet[i].rhsVector.size(); j++)
//{
//    if (!isNonTerminal(ruleSet[i].rhsVector[j].front()))
//    {
//        it1 = find(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end(), ruleSet[i].rhsVector[j].front());
//        if (it1 == ruleSet[i].firstSet.end())
//        {
//            ruleSet[i].firstSet.push_back(ruleSet[i].rhsVector[j].front());
//            changed = true;
//        }
//    }
//}
//for (int j = 0; j < (int)ruleSet[i].rhsVector.size(); j++) {    // For all RHS; A -> a #  A -> # ... etc
//    if (isNonTerminal(ruleSet[i].rhsVector[j].front()))         // if non terminal
//    {
//        int index = getNonTerminalIndex(ruleSet[i].rhsVector[j].front());           // Find nonterminal
//        vector<string> firstSet = ruleSet[index].firstSet;
//        vector<string> currentFirstSet = ruleSet[i].firstSet;
//        //add first(nonterminal) to first(ruleSet[i])
//
//        for (int setIndex = 0; setIndex < firstSet.size(); setIndex++)     // Iterate through First(nonterminal)
//        {
//            it1 = find(currentFirstSet.begin(), currentFirstSet.end(), firstSet[setIndex]);     // Append first(nt) to first(ruleSet[i])
//            if (it1 == currentFirstSet.end() )
//            {
//                currentFirstSet.push_back(firstSet[setIndex]);                 // add what is not there.
//                changed = true;
//            }
//
//        }

