#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include "nonTerminalRule.cpp"

extern "C" {
#include "lexer.h"
}

vector<nonTerminalRule> ruleSet;
vector<string> nonTerminals;
vector<string> terminals;

int j = 0;

void parseGrammar();
void findTerminals();
void findFirstSets();
void findFollowSets();
bool isNonTerminal(string candidate);
int getNonTerminalIndex(string nonTerminal);
bool isEpsilon(int index);
bool contains(vector<string> vect, string candidate);
bool rightAreEpsilon(vector<string> rhs, int startIndex);
int rightIsNotEpsilon(vector<string> rhs, int startIndex);
int getIndexBeforeHash(vector<string>);

using namespace std;
int main (int argc, char* argv[])
{
    int task;
    
            if (argc < 2)
            {
                printf("Error: missing argument\n");
                return 1;
            }

    task = atoi(argv[1]);

    
    switch (task) {
        case 0:
        {
            // TODO: Output information about the input grammar
            
            parseGrammar();
            
            for (int i = 0; i < (int)terminals.size(); i++) {
                cout << terminals[i] + " ";
            }
            
            cout << endl;
            
            for (int i = 0; i < (int)ruleSet.size(); i++) {
                cout << ruleSet[i].nonTerminal << ": " << ruleSet[i].rhsVector.size() << endl;
            }
            
            break;
        }
        case 1:
        {
             /*TODO: Calculate FIRST sets for the input grammar
             Hint: You better do the calculation in a function and call it here!
             TODO: Output the FIRST sets in the exact order and format required*/
            
            parseGrammar();
            findFirstSets();
            
            for (int i = 0; i < (int)ruleSet.size(); i++)
            {
                cout << "FIRST(" << ruleSet[i].nonTerminal << ") = { ";
                for (int j = 0; j < (int)ruleSet[i].firstSet.size(); j++)
                {
                    sort(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end());
                    if (j == (int)ruleSet[i].firstSet.size()-1)
                    {
                        cout << ruleSet[i].firstSet[j] + " ";
                    }
                    else
                    {
                        cout << ruleSet[i].firstSet[j] + ", ";
                    }
                }
                cout << "}" << endl;
            }
            break;
        }
        case 2:
        {
             /*TODO: Calculate FIRST sets for the input grammar
             TODO: Calculate FOLLOW sets for the input grammar
             TODO: Output the FOLLOW sets in the exact order and format required*/
            parseGrammar();
            findFollowSets();
            
            for (int i = 0; i < (int)ruleSet.size(); i++)
            {
                cout << "FOLLOW(" << ruleSet[i].nonTerminal << ") = { ";
                for (int j = 0; j < (int)ruleSet[i].followSet.size(); j++)
                {
                    sort(ruleSet[i].followSet.begin(), ruleSet[i].followSet.end());
                    if (j == (int)ruleSet[i].followSet.size()-1)
                    {
                        cout << ruleSet[i].followSet[j] + " ";
                    }
                    else
                    {
                        cout << ruleSet[i].followSet[j] + ", ";
                    }
                }
                cout << "}" << endl;
            }
            break;
        }
        default:
            printf("Error: unrecognized task number %d\n", task);
            break;
    }
    return 0;
    
}

void parseGrammar() {
    getToken();
    
    while (t_type != HASH) {
        ruleSet.push_back(*new nonTerminalRule(current_token, j));
        nonTerminals.push_back(current_token);
        getToken();
        j++;
    }
    // Parse the input into the data structure.
    getToken();
    while (t_type != DOUBLEHASH) {
        while (t_type != HASH && t_type != DOUBLEHASH) {
            for (int i = 0; i < (int)ruleSet.size(); i++) {
                if (ruleSet[i].nonTerminal.compare(current_token) == 0) {
                    getToken();
                    if (t_type == ARROW) {
                        vector<string> temp;
                        getToken();
                        while (t_type != HASH) {
                            temp.push_back(current_token);
                            getToken();
                        }
                        temp.push_back("#");
                        ruleSet[i].addRHS(temp);
                    }
                    getToken();
                }
            }
        }
    }
    findTerminals();
}

void findTerminals() {
    vector<string>::iterator it1;
    vector<string>::iterator it2;
    for (int i = 0; i < (int)ruleSet.size(); i++) {
        for (int j = 0; j < (int)ruleSet[i].rhsVector.size(); j++) {
            for (int k = 0; k < (int)ruleSet[i].rhsVector[j].size(); k++) {
                it1 = find(nonTerminals.begin(), nonTerminals.end(), ruleSet[i].rhsVector[j][k]);
                it2 = find(terminals.begin(), terminals.end(), ruleSet[i].rhsVector[j][k]);
                if (it1 == nonTerminals.end() && it2 == terminals.end() && (ruleSet[i].rhsVector[j][k] != "#")) {
                    terminals.push_back(ruleSet[i].rhsVector[j][k]);
                }
            }
        }
    }
}

void findFirstSets() {
    bool changed = true;
    vector<string>::iterator it1;
    
    while (changed)
    {
        changed = false;
        for (int i = 0; i < (int)ruleSet.size(); i++)
        {
            // Rule 2.
            for (int j = 0; j < (int)ruleSet[i].rhsVector.size(); j++)
            {
                if (ruleSet[i].rhsVector[j].front() == "#") {
                    it1 = find(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end(), "#");
                    if (it1 == ruleSet[i].firstSet.end()) {
                        ruleSet[i].firstSet.push_back("#");
                        changed = true;
                    }
                    
                }
            }
            // Rule 1 & 3.
            for (int j = 0; j < (int)ruleSet[i].rhsVector.size(); j++)
            {
                if (!isNonTerminal(ruleSet[i].rhsVector[j].front()))
                {
                    it1 = find(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end(), ruleSet[i].rhsVector[j].front());
                    if (it1 == ruleSet[i].firstSet.end())
                    {
                        ruleSet[i].firstSet.push_back(ruleSet[i].rhsVector[j].front());
                        changed = true;
                    }
                }
                else if (isNonTerminal(ruleSet[i].rhsVector[j].front()))         // if non terminal
                {
                    int index = getNonTerminalIndex(ruleSet[i].rhsVector[j].front());
                    for (int k = 0; k < (int)ruleSet[index].firstSet.size(); k++)
                    {
                        if (ruleSet[index].firstSet[k] != "#")
                        {
                            it1 = find(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end(), ruleSet[index].firstSet[k]);
                            if (it1 == ruleSet[i].firstSet.end())
                            {
                                ruleSet[i].firstSet.push_back(ruleSet[index].firstSet[k]);
                                changed = true;
                            }
                        }
                    }
                }
            }
            // rule 4 & 5.
            for (int j = 0; j < (int)ruleSet[i].rhsVector.size(); j++)
            {
                bool epsilonPrevious = true;
                for (int k = 0; k < (int)ruleSet[i].rhsVector[j].size(); k++)
                {
                    string token = ruleSet[i].rhsVector[j][k];
                    if (isNonTerminal(ruleSet[i].rhsVector[j][k]))
                    {
                        int index = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                        if (isEpsilon(index))
                        {
                            if (epsilonPrevious)
                            {
                                for (int l = 0; l < (int)ruleSet[index].firstSet.size(); l++)
                                {
                                    it1 = find(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end(), ruleSet[index].firstSet[l]);
                                    if (it1 == ruleSet[i].firstSet.end() && ruleSet[index].firstSet[l] != "#")
                                    {
                                        ruleSet[i].firstSet.push_back(ruleSet[index].firstSet[l]);
                                        changed = true;
                                    }
                                }
                            }
                        }
                        else
                        {
                            epsilonPrevious = false;
                        }
                    }
                    else if(epsilonPrevious)
                    {
                    epsilonPrevious = false;
                    it1 = find(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end(), ruleSet[i].rhsVector[j][k]);
                        if (it1 == ruleSet[i].firstSet.end()) {
                            ruleSet[i].firstSet.push_back(ruleSet[i].rhsVector[j][k]);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

void findFollowSets() {
    findFirstSets();
    bool changed = true;
    bool setStart = false;
    vector<string>::iterator it1;
    
    while (changed)
    {
        changed = false;
        for (int i = 0; i < (int)ruleSet.size(); i++)
        {
            // Rule 1.
            if (i == 0 && !setStart)
            {
                ruleSet[i].followSet.push_back("$");
                setStart = true;
                changed = true;
            }
            // Rule 2.
            for (int j = 0; j < (int)ruleSet[i].rhsVector.size(); j++)
            {
                if ((int)ruleSet[i].rhsVector[j].size() > 0)
                {
                    int kIndex = getIndexBeforeHash(ruleSet[i].rhsVector[j]);
                    string back = ruleSet[i].rhsVector[j][kIndex];
                    if (isNonTerminal(back)) {
                        int index = (int)getNonTerminalIndex(ruleSet[i].rhsVector[j][kIndex]);
                        for (int k = 0; k < (int)ruleSet[i].followSet.size(); k++)
                        {
                            it1 = find(ruleSet[index].followSet.begin(), ruleSet[index].followSet.end(), ruleSet[i].followSet[k]);
                            if (it1 == ruleSet[index].followSet.end())
                            {
                                
                                ruleSet[index].followSet.push_back(ruleSet[i].followSet[k]);
                                changed = true;
                            }
                        }
                    }
                }
            }
            // Rule 3.
            for (int j = 0; j < (int)ruleSet[i].rhsVector.size(); j++)
            {
                bool previousEpsilon = true;
                for (int k = 0; k < (int)ruleSet[i].rhsVector[j].size(); k++)
                {
                    if (isNonTerminal(ruleSet[i].rhsVector[j][k]))
                    {
                        if (previousEpsilon)
                        {
                            if (rightAreEpsilon(ruleSet[i].rhsVector[j], k))
                            {
                                int index = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                                for (int l = 0; l < (int)ruleSet[i].followSet.size(); l++)
                                {
                                    it1 = find(ruleSet[index].followSet.begin(), ruleSet[index].followSet.end(), ruleSet[i].followSet[l]);
                                    if (it1 == ruleSet[index].followSet.end())
                                    {
                                        ruleSet[index].followSet.push_back(ruleSet[i].followSet[l]);
                                        changed = true;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        previousEpsilon = false;
                    }
                }
            }
            // Rule 4.
            for (int j = 0; j < (int)ruleSet[i].rhsVector.size(); j++)
            {
                for (int k = 0; k < (int)ruleSet[i].rhsVector[j].size(); k++)
                {
                    if (isNonTerminal(ruleSet[i].rhsVector[j][k]))
                    {
                        if ((k+1) <= (int)ruleSet[i].rhsVector[j].size())
                        {
                            if (isNonTerminal(ruleSet[i].rhsVector[j][k+1]))
                            {
                                int currentIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                                int nextIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][k+1]);
                                int nextIndexFirstSetSize = (int)ruleSet[nextIndex].firstSet.size();
                                for (int l = 0; l < nextIndexFirstSetSize; l++)
                                {
                                    it1 = find(ruleSet[currentIndex].followSet.begin(), ruleSet[currentIndex].followSet.end(), ruleSet[nextIndex].firstSet[l]);
                                    if (it1 == ruleSet[currentIndex].followSet.end() && ruleSet[nextIndex].firstSet[l] != "#")
                                    {
                                        ruleSet[currentIndex].followSet.push_back(ruleSet[nextIndex].firstSet[l]);
                                        changed = true;
                                    }
                                }
                            }
                            else if(!isNonTerminal(ruleSet[i].rhsVector[j][k+1]))
                            {
                                int currentIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                                it1 = find(ruleSet[currentIndex].followSet.begin(), ruleSet[currentIndex].followSet.end(), ruleSet[i].rhsVector[j][k+1]);
                                if (it1 == ruleSet[currentIndex].followSet.end() && ruleSet[i].rhsVector[j][k+1] != "#") {
                                    ruleSet[currentIndex].followSet.push_back(ruleSet[i].rhsVector[j][k+1]);
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
            // Rule 5.
            for (int j = 0; j < (int)ruleSet[i].rhsVector.size(); j++)
            {
                for (int k = 0; k < (int)ruleSet[i].rhsVector[j].size(); k++)
                {
                    if (isNonTerminal(ruleSet[i].rhsVector[j][k]))
                    {
                        int rightIndex = rightIsNotEpsilon(ruleSet[i].rhsVector[j], k);
                        if (rightIndex != -1)
                        {
                            if (isNonTerminal(ruleSet[i].rhsVector[j][rightIndex]))
                            {
                                int currentIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                                int nextIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][rightIndex]);
                                int nextIndexFirstSize = (int)ruleSet[nextIndex].firstSet.size();
                                for (int l = 0; l < nextIndexFirstSize; l++)
                                {
                                    it1 = find(ruleSet[currentIndex].followSet.begin(), ruleSet[currentIndex].followSet.end(), ruleSet[nextIndex].firstSet[l]);
                                    if (it1 == ruleSet[currentIndex].followSet.end() && ruleSet[nextIndex].firstSet[l] != "#")
                                    {
                                        ruleSet[currentIndex].followSet.push_back(ruleSet[nextIndex].firstSet[l]);
                                        changed = true;
                                    }
                                }
                            }
                            else if(!isNonTerminal(ruleSet[i].rhsVector[j][rightIndex]))
                            {
                                int currentIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                                it1 = find(ruleSet[currentIndex].followSet.begin(), ruleSet[currentIndex].followSet.end(), ruleSet[i].rhsVector[j][rightIndex]);
                                if (it1 == ruleSet[currentIndex].followSet.end() && ruleSet[i].rhsVector[j][rightIndex] != "#")
                                {
                                    ruleSet[currentIndex].followSet.push_back(ruleSet[i].rhsVector[j][rightIndex]);
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
       
    }
}

int getIndexBeforeHash(vector<string> rhs) {
    for (int i = 0; i < (int)rhs.size(); i++) {
        if (rhs[i] == "#") {
            return i-1;
        }
    }
    return -1;
}

bool isEpsilon(int index) {
    vector<string>::iterator iterator;
    iterator = find(ruleSet[index].firstSet.begin(), ruleSet[index].firstSet.end(), "#");
    if (iterator != ruleSet[index].firstSet.end()) {
        return true;
    } else
    {
        return false;
    }
    
}

bool isNonTerminal(string candidate) {
    if (getNonTerminalIndex(candidate) != -1) {
        return true;
    } else {
        return false;
    }
}

int getNonTerminalIndex(string nonTerminal) {
    for (int i = 0; i < (int)ruleSet.size(); i++) {
        if (ruleSet[i].nonTerminal == nonTerminal) {
            return i;
        }
    }
    return -1;
}

bool rightAreEpsilon(vector<string> rhs, int startIndex) {
    for (int i = startIndex + 1; rhs.size(); i++)
    {
        if (isNonTerminal(rhs[i]))
        {
            if (!isEpsilon(getNonTerminalIndex(rhs[i])))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

int rightIsNotEpsilon(vector<string> rhs, int startIndex) {
    for (int i = startIndex + 1; rhs.size(); i++)
    {
        if (isNonTerminal(rhs[i]))
        {
            if (!isEpsilon(getNonTerminalIndex(rhs[i])))
            {
                return i;
            }
        }
        else
        {
            return i;
        }
    }
    return -1;
}