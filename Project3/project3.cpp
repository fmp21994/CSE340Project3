#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

extern "C"
{
#include "lexer.h"
}

class nonTerminalRule
{
public:
    
    string nonTerminal;
    vector<vector<string> > rhsVector;
    vector<string> firstSet;
    vector<string> followSet;
    
    nonTerminalRule(char* nonTerminal)
    {
        this->nonTerminal = nonTerminal;
    }
    
    void addRHS(vector<string> rhsVec)
    {
        rhsVector.push_back(rhsVec);
    }
};

vector<nonTerminalRule> ruleSet;

vector<string> nonTerminals;
vector<string> terminals;

void parseGrammar();
void findTerminals();
void findFirstSets();
void findFollowSets();
bool isNonTerminal(string candidate);
int getNonTerminalIndex(string nonTerminal);
bool isEpsilon(int index);
bool rightAreEpsilon(vector<string> rhs, int startIndex);
int rightIsNotEpsilon(vector<string> rhs, int startIndex);
int getIndexBeforeHash(vector<string>);

int main(int argc, char* argv[])
{
    int task;
    
    if (argc < 2)
    {
        printf("Error: missing argument\n");
        return 1;
    }
    
    task = atoi(argv[1]);
    
    switch (task)
    {
        case 0:
        {
                // TODO: Output information about the input grammar
            parseGrammar();
            
            for (auto i = 0; i < static_cast<int>(terminals.size()); i++)
            {
                cout << terminals[i] + " ";
            }
            
            cout << endl;
            
            for (auto i = 0; i < static_cast<int>(ruleSet.size()); i++)
            {
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
            
            for (auto i = 0; i < static_cast<int>(ruleSet.size()); i++)
            {
                cout << "FIRST(" << ruleSet[i].nonTerminal << ") = { ";
                for (auto j = 0; j < static_cast<int>(ruleSet[i].firstSet.size()); j++)
                {
                    sort(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end());
                    if (j == static_cast<int>(ruleSet[i].firstSet.size()) - 1)
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
            
            for (auto i = 0; i < static_cast<int>(ruleSet.size()); i++)
            {
                cout << "FOLLOW(" << ruleSet[i].nonTerminal << ") = { ";
                for (auto j = 0; j < static_cast<int>(ruleSet[i].followSet.size()); j++)
                {
                    sort(ruleSet[i].followSet.begin(), ruleSet[i].followSet.end());
                    if (j == static_cast<int>(ruleSet[i].followSet.size()) - 1)
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

void parseGrammar()
{
    getToken();
    
    while (t_type != HASH)
    {
        ruleSet.push_back(*new nonTerminalRule(current_token));
        nonTerminals.push_back(current_token);
        getToken();
    }
        // Parse the input into the data structure.
    getToken();
    while (t_type != DOUBLEHASH)
    {
        while (t_type != HASH && t_type != DOUBLEHASH)
        {
            for (auto i = 0; i < static_cast<int>(ruleSet.size()); i++)
            {
                if (ruleSet[i].nonTerminal.compare(current_token) == 0)
                {
                    getToken();
                    if (t_type == ARROW)
                    {
                        vector<string> temp;
                        getToken();
                        while (t_type != HASH)
                        {
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

void findTerminals()
{
    vector<string>::iterator it1;
    vector<string>::iterator it2;
    for (auto i = 0; i < static_cast<int>(ruleSet.size()); i++)
    {
        for (auto j = 0; j < static_cast<int>(ruleSet[i].rhsVector.size()); j++)
        {
            for (auto k = 0; k < static_cast<int>(ruleSet[i].rhsVector[j].size()); k++)
            {
                it1 = find(nonTerminals.begin(), nonTerminals.end(), ruleSet[i].rhsVector[j][k]);
                it2 = find(terminals.begin(), terminals.end(), ruleSet[i].rhsVector[j][k]);
                if (it1 == nonTerminals.end() && it2 == terminals.end() && (ruleSet[i].rhsVector[j][k] != "#"))
                {
                    terminals.push_back(ruleSet[i].rhsVector[j][k]);
                }
            }
        }
    }
}

void findFirstSets()
{
    auto changed = true;
    vector<string>::iterator it1;
    
    while (changed)
    {
        changed = false;
        for (auto i = 0; i < static_cast<int>(ruleSet.size()); i++)
        {
                // Rule 2.
            for (auto j = 0; j < static_cast<int>(ruleSet[i].rhsVector.size()); j++)
            {
                if (ruleSet[i].rhsVector[j].front() == "#")
                {
                    it1 = find(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end(), "#");
                    if (it1 == ruleSet[i].firstSet.end())
                    {
                        ruleSet[i].firstSet.push_back("#");
                        changed = true;
                    }
                }
            }
                // Rule 1 & 3.
            for (auto j = 0; j < static_cast<int>(ruleSet[i].rhsVector.size()); j++)
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
                else if (isNonTerminal(ruleSet[i].rhsVector[j].front()))
                { // if non terminal
                    auto index = getNonTerminalIndex(ruleSet[i].rhsVector[j].front());
                    for (auto k = 0; k < static_cast<int>(ruleSet[index].firstSet.size()); k++)
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
            for (auto j = 0; j < static_cast<int>(ruleSet[i].rhsVector.size()); j++)
            {
                auto epsilonPrevious = true;
                for (auto k = 0; k < static_cast<int>(ruleSet[i].rhsVector[j].size()); k++)
                {
                    auto token = ruleSet[i].rhsVector[j][k];
                    if (isNonTerminal(ruleSet[i].rhsVector[j][k]))
                    {
                        auto index = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                        if (isEpsilon(index))
                        {
                            if (epsilonPrevious)
                            {
                                for (auto l = 0; l < static_cast<int>(ruleSet[index].firstSet.size()); l++)
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
                    else if (epsilonPrevious)
                    {
                        epsilonPrevious = false;
                        it1 = find(ruleSet[i].firstSet.begin(), ruleSet[i].firstSet.end(), ruleSet[i].rhsVector[j][k]);
                        if (it1 == ruleSet[i].firstSet.end())
                        {
                            ruleSet[i].firstSet.push_back(ruleSet[i].rhsVector[j][k]);
                            changed = true;
                        }
                    }
                }
            }
        }
    }
}

void findFollowSets()
{
    findFirstSets();
    auto changed = true;
    auto setStart = false;
    
    while (changed)
    {
        changed = false;
        for (auto i = 0; i < static_cast<int>(ruleSet.size()); i++)
        {
                // Rule 1.
            if (i == 0 && !setStart)
            {
                ruleSet[i].followSet.push_back("$");
                setStart = true;
                changed = true;
            }
                // Rule 2.
            for (auto j = 0; j < static_cast<int>(ruleSet[i].rhsVector.size()); j++)
            {
                if (static_cast<int>(ruleSet[i].rhsVector[j].size()) > 0)
                {
                    auto kIndex = getIndexBeforeHash(ruleSet[i].rhsVector[j]);
                    if (kIndex >= 0)
                    {
                        auto back = ruleSet[i].rhsVector[j][kIndex];
                        if (isNonTerminal(back))
                        {
                            auto index = static_cast<int>(getNonTerminalIndex(ruleSet[i].rhsVector[j][kIndex]));
                            for (auto k = 0; k < static_cast<int>(ruleSet[i].followSet.size()); k++)
                            {
                                auto it1 = find(ruleSet[index].followSet.begin(), ruleSet[index].followSet.end(), ruleSet[i].followSet[k]);
                                if (it1 == ruleSet[index].followSet.end())
                                {
                                    ruleSet[index].followSet.push_back(ruleSet[i].followSet[k]);
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
                // Rule 3.
            for (auto j = 0; j < static_cast<int>(ruleSet[i].rhsVector.size()); j++)
            {
                auto previousEpsilon = true;
                for (auto k = 0; k < static_cast<int>(ruleSet[i].rhsVector[j].size()); k++)
                {
                    if (isNonTerminal(ruleSet[i].rhsVector[j][k]))
                    {
                        if (previousEpsilon)
                        {
                            if (rightAreEpsilon(ruleSet[i].rhsVector[j], k))
                            {
                                auto index = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                                for (auto l = 0; l < static_cast<int>(ruleSet[i].followSet.size()); l++)
                                {
                                    auto it1 = find(ruleSet[index].followSet.begin(), ruleSet[index].followSet.end(), ruleSet[i].followSet[l]);
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
            for (auto j = 0; j < static_cast<int>(ruleSet[i].rhsVector.size()); j++)
            {
                for (auto k = 0; k < static_cast<int>(ruleSet[i].rhsVector[j].size()); k++)
                {
                    if (isNonTerminal(ruleSet[i].rhsVector[j][k]))
                    {
                        if ((k + 1) <= static_cast<int>(ruleSet[i].rhsVector[j].size()))
                        {
                            if (isNonTerminal(ruleSet[i].rhsVector[j][k + 1]))
                            {
                                auto currentIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                                auto nextIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][k + 1]);
                                auto nextIndexFirstSetSize = static_cast<int>(ruleSet[nextIndex].firstSet.size());
                                for (auto l = 0; l < nextIndexFirstSetSize; l++)
                                {
                                    auto it1 = find(ruleSet[currentIndex].followSet.begin(), ruleSet[currentIndex].followSet.end(), ruleSet[nextIndex].firstSet[l]);
                                    if (it1 == ruleSet[currentIndex].followSet.end() && ruleSet[nextIndex].firstSet[l] != "#")
                                    {
                                        ruleSet[currentIndex].followSet.push_back(ruleSet[nextIndex].firstSet[l]);
                                        changed = true;
                                    }
                                }
                            }
                            else if (!isNonTerminal(ruleSet[i].rhsVector[j][k + 1]))
                            {
                                auto currentIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                                auto it1 = find(ruleSet[currentIndex].followSet.begin(), ruleSet[currentIndex].followSet.end(), ruleSet[i].rhsVector[j][k + 1]);
                                if (it1 == ruleSet[currentIndex].followSet.end() && ruleSet[i].rhsVector[j][k + 1] != "#")
                                {
                                    ruleSet[currentIndex].followSet.push_back(ruleSet[i].rhsVector[j][k + 1]);
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
                // Rule 5.
            for (auto j = 0; j < static_cast<int>(ruleSet[i].rhsVector.size()); j++)
            {
                for (auto k = 0; k < static_cast<int>(ruleSet[i].rhsVector[j].size()); k++)
                {
                    if (isNonTerminal(ruleSet[i].rhsVector[j][k]))
                    {
                        auto rightIndex = rightIsNotEpsilon(ruleSet[i].rhsVector[j], k);
                        if (rightIndex != -1)
                        {
                            if (isNonTerminal(ruleSet[i].rhsVector[j][rightIndex]))
                            {
                                auto currentIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                                auto nextIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][rightIndex]);
                                auto nextIndexFirstSize = static_cast<int>(ruleSet[nextIndex].firstSet.size());
                                for (auto l = 0; l < nextIndexFirstSize; l++)
                                {
                                    auto it1 = find(ruleSet[currentIndex].followSet.begin(), ruleSet[currentIndex].followSet.end(), ruleSet[nextIndex].firstSet[l]);
                                    if (it1 == ruleSet[currentIndex].followSet.end() && ruleSet[nextIndex].firstSet[l] != "#")
                                    {
                                        ruleSet[currentIndex].followSet.push_back(ruleSet[nextIndex].firstSet[l]);
                                        changed = true;
                                    }
                                }
                            }
                            else if (!isNonTerminal(ruleSet[i].rhsVector[j][rightIndex]))
                            {
                                auto currentIndex = getNonTerminalIndex(ruleSet[i].rhsVector[j][k]);
                                auto it1 = find(ruleSet[currentIndex].followSet.begin(), ruleSet[currentIndex].followSet.end(), ruleSet[i].rhsVector[j][rightIndex]);
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

int getIndexBeforeHash(vector<string> rhs)
{
    for (auto i = 0; i < static_cast<int>(rhs.size()); i++)
    {
        if (rhs[i] == "#")
        {
            return i - 1;
        }
    }
    return -1;
}

bool isEpsilon(int index)
{
    vector<string>::iterator iterator;
    iterator = find(ruleSet[index].firstSet.begin(), ruleSet[index].firstSet.end(), "#");
    if (iterator != ruleSet[index].firstSet.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isNonTerminal(string candidate)
{
    if (getNonTerminalIndex(candidate) != -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int getNonTerminalIndex(string nonTerminal)
{
    for (auto i = 0; i < static_cast<int>(ruleSet.size()); i++)
    {
        if (ruleSet[i].nonTerminal == nonTerminal)
        {
            return i;
        }
    }
    return -1;
}

bool rightAreEpsilon(vector<string> rhs, int startIndex)
{
    for (auto i = startIndex + 1; i < static_cast<int>(rhs.size()); i++)
    {
        if (isNonTerminal(rhs[i]))
        {
            if (!isEpsilon(getNonTerminalIndex(rhs[i])))
            {
                return false;
            }
        }
        else if (!isNonTerminal(rhs[i]) && rhs[i] != "#")
        {
            return false;
        }
    }
    return true;
}

int rightIsNotEpsilon(vector<string> rhs, int startIndex)
{
    for (auto i = startIndex + 1; rhs.size(); i++)
    {
        if (isNonTerminal(rhs[i]))
        {
            auto nonTerminalIndex = getNonTerminalIndex(rhs[i]);
            if (!isEpsilon(nonTerminalIndex))
            {
                return i;
            }
        }
        else if (!isNonTerminal(rhs[i]))
        {
            return i;
        }
    }
    return -1;
}