// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

// EECS 281 Project 3 SillyQL TableEntry class
// Copyright 2020, Regents of the University of Michigan
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <deque>
#include <string>
#include <array>
#include <algorithm>
#include <getopt.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <map>
#include <unordered_map>
#include "TableEntry.h"
using namespace std;
enum class compareType : uint8_t
{
    greater,
    less,
    equal,
    error
};
enum class generateType : uint8_t
{
    None,
    Hash,
    BST
};

class comparator
{
public:
    // take the field to sort by in the constructor
    comparator(const TableEntry &val, compareType OP, int index) : compareVal(val), oper(OP), colIndex(index) {}
    bool operator()(const vector<TableEntry> &row)
    {
        switch (oper)
        {
        case compareType::greater:
            return row[(size_t)colIndex] > compareVal;
        case compareType::less:
            return row[(size_t)colIndex] < compareVal;
        case compareType::equal:
            return row[(size_t)colIndex] == compareVal;
        default:
            return false;
        }
    }

private:
    TableEntry compareVal;
    compareType oper;
    int colIndex;
};

struct compBST
{
    bool operator()(const TableEntry &lhs, const TableEntry &rhs)
    {
        return lhs < rhs;
    }
};

class Table
{
private:
    unordered_map<string, int> colNames;
    vector<EntryType> dataTypes;
    vector<vector<TableEntry>> data;
    int dataRows = 0;
    unordered_map<TableEntry, vector<int>> Hash;
    unordered_map<TableEntry, vector<int>> JoinHash;
    // compBST compbst;
    std::map<TableEntry, vector<int>> BST;
    generateType generated = generateType::None;
    string generateColname;
    int generateColIndex = -1;

public:
    unordered_map<TableEntry, vector<int>> getHash()
    {
        return JoinHash;
    }
    generateType getGenerated()
    {
        return generated;
    }
    int getNumDataRows()
    {
        return dataRows;
    }
    void resizeVecs(int size)
    {
        colNames.reserve((size_t)size);
        dataTypes.reserve((size_t)size);
    }
    void addName(string name, int index)
    {
        colNames.insert(make_pair(name, index));
    }
    void addType(EntryType type)
    {
        dataTypes.push_back(type);
    }
    void addDataRow(vector<TableEntry> row)
    {
        if (generated == generateType::Hash)
        {
            if (Hash.count(row[(size_t)generateColIndex]) == 0)
            {
                vector<int> indeces;
                indeces.push_back((int)data.size());
                Hash.insert(make_pair(row[(size_t)generateColIndex], indeces));
            }
            else
            {
                Hash[row[(size_t)generateColIndex]].push_back((int)data.size());
            }
        }

        if (generated == generateType::BST)
        {
            if (BST.count(row[(size_t)generateColIndex]) == 0)
            {
                vector<int> indeces;
                indeces.push_back((int)data.size());
                BST.insert(make_pair(row[(size_t)generateColIndex], indeces));
            }
            else
            {
                BST[row[(size_t)generateColIndex]].push_back((int)data.size());
            }
        }

        data.push_back(row);
    }
    void printCols()
    {
        for (int i = 0; i < (int)colNames.size(); i++)
        {
            for (auto x : colNames)
            {
                if (x.second == i)
                {
                    cout << x.first << " ";
                }
            }
        }
    }
    int getColIndex(string col)
    {
        return colNames[col];
    }
    EntryType getDType(int index)
    {
        return dataTypes[(size_t)index];
    }
    int getNumCols()
    {
        return (int)colNames.size();
    }
    int getNumRows()
    {
        return (int)data.size();
    }
    int resizeDataVec(int rowsToBeAdded)
    {
        int prevSize = dataRows;
        dataRows += rowsToBeAdded;
        data.reserve((size_t)rowsToBeAdded);
        return prevSize;
    }
    TableEntry getEntry(int row, int col)
    {
        return data[(size_t)row][(size_t)col];
    }
    bool checkCols(string selectedCols)
    {
        if (colNames.count(selectedCols) == 0)
        {
            return false;
        }
        return true;
    }
    void printAll(vector<string> selectedCols, int &rowsPrinted, bool quietMode)
    {
        size_t size = selectedCols.size();
        rowsPrinted = (int)data.size();
        if (!quietMode)
        {
            for (size_t i = 0; i < data.size(); i++)
            {
                for (size_t j = 0; j < size; j++)
                {
                    cout << data[i][(size_t)colNames[selectedCols[j]]] << " ";
                }
                cout << "\n";
            }
        }
    }
    void printWhere(vector<string> selectedCols, int &rowsPrinted, string conditionalCol, TableEntry Val, int colIndex, bool quietMode, string colName)
    {
        size_t size = selectedCols.size();
        rowsPrinted = 0;
        compareType oP = convertOP(conditionalCol);
        comparator comp(Val, oP, colIndex);
        if (generated == generateType::BST && generateColIndex == colIndex)
        {
            generateBST(colName);
            for (auto it = BST.begin(); it != BST.end(); it++)
            {
                for (size_t p = 0; p < it->second.size(); p++)
                {
                    int i = it->second[p];
                    if (!comp(data[(size_t)i]))
                    {
                        continue;
                    }
                    if (!quietMode)
                    {
                        for (size_t j = 0; j < size; j++)
                        {
                            cout << data[(size_t)i][(size_t)colNames[selectedCols[j]]] << " ";
                        }
                        cout << "\n";
                    }
                    rowsPrinted++;
                }
            }
        }
        /*else if (generated == generateType::Hash && generateColIndex == colIndex && oP == compareType::equal)
        {
            generateHash(colName);
            //cout << colName << endl;
            //printHash();
            //for (size_t i = 0; i < data.size(); i++)
            //{

                if (Hash.find(Val) == Hash.end())
                {
                    return;
                }

                for (size_t p = 0; p < Hash[Val].size(); p++)
                {
                    if (!quietMode)
                    {
                        int k = Hash[Val][p];
                        for (size_t j = 0; j < size; j++)
                        {
                            cout << data[(size_t)k][(size_t)colNames[selectedCols[j]]] << " ";
                        }
                        cout << "\n";
                        
                    }
                    rowsPrinted++;
                }
            //}
        }*/
        else
        {
            for (size_t i = 0; i < data.size(); i++)
            {

                if (!comp(data[i]))
                {
                    continue;
                }
                if (!quietMode)
                {
                    for (size_t j = 0; j < size; j++)
                    {
                        cout << data[i][(size_t)colNames[selectedCols[j]]] << " ";
                    }
                    cout << "\n";
                }
                rowsPrinted++;
            }
        }
    }
    void deleteRow(int &rowsDeleted, string conditionalCol, TableEntry Val, int colIndex)
    {
        compareType oP = convertOP(conditionalCol);
        comparator comp(Val, oP, colIndex);
        rowsDeleted = (int)data.size();
        data.erase(remove_if(data.begin(), data.end(), comp), data.end());
        rowsDeleted -= (int)data.size();
        dataRows = dataRows - rowsDeleted;
        if (generated == generateType::BST)
        {
            generateBST(generateColname);
        }
        if (generated == generateType::Hash)
        {
            generateHash(generateColname);
        }
    }
    compareType convertOP(string conditionalCol)
    {
        if (conditionalCol == "<")
        {
            return compareType::less;
        }
        else if (conditionalCol == ">")
        {
            return compareType::greater;
        }
        else if (conditionalCol == "=")
        {
            return compareType::equal;
        }
        return compareType::error;
    }
    bool checkCondition(string conditionalCol, TableEntry tableVal, TableEntry conditionalVal)
    {
        if (conditionalCol == "<")
        {
            if (tableVal < conditionalVal)
            {
                return true;
            }
        }
        else if (conditionalCol == ">")
        {
            if (tableVal > conditionalVal)
            {
                return true;
            }
        }
        else if (conditionalCol == "=")
        {
            if (tableVal == conditionalVal)
            {
                return true;
            }
        }
        return false;
    }
    void generateHash(string colName)
    {
        generated = generateType::Hash;
        // BST.clear();
        Hash.clear();
        generateColname = colName;
        generateColIndex = colNames[colName];
        for (size_t i = 0; i < data.size(); i++)
        {
            if (Hash.count(data[i][(size_t)generateColIndex]) == 0)
            {
                vector<int> indeces;
                indeces.push_back((int)i);
                Hash.insert(make_pair(data[i][(size_t)generateColIndex], indeces));
            }
            else
            {
                Hash[data[i][(size_t)generateColIndex]].push_back((int)i);
            }
        }
    }
    void generateHashJoin(string colName)
    {
        // BST.clear();
        JoinHash.clear();
        generateColIndex = colNames[colName];
        for (size_t i = 0; i < data.size(); i++)
        {
            if (JoinHash.count(data[i][(size_t)generateColIndex]) == 0)
            {
                vector<int> indeces;
                indeces.push_back((int)i);
                JoinHash.insert(make_pair(data[i][(size_t)generateColIndex], indeces));
            }
            else
            {
                JoinHash[data[i][(size_t)generateColIndex]].push_back((int)i);
            }
        }
    }
    void generateBST(string colName)
    {
        generated = generateType::BST;
        // Hash.clear();
        BST.clear();
        generateColname = colName;
        generateColIndex = colNames[colName];
        for (size_t i = 0; i < data.size(); i++)
        {
            if (BST.count(data[i][(size_t)generateColIndex]) == 0)
            {
                vector<int> indeces;
                indeces.push_back((int)i);
                BST.insert(make_pair(data[i][(size_t)generateColIndex], indeces));
            }
            else
            {
                BST[data[i][(size_t)generateColIndex]].push_back((int)i);
            }
        }
    }

    void printHash()
    {
        for (auto it = Hash.begin(); it != Hash.end(); it++)
        {
            cout << it->first << ": ";
            for (size_t i = 0; i < it->second.size(); i++)
            {
                cout << it->second[i] << " ";
            }
            cout << "\n";
        }
    }
    void printBST()
    {
        for (auto it = BST.begin(); it != BST.end(); it++)
        {
            cout << it->first << ": ";
            for (size_t i = 0; i < it->second.size(); i++)
            {
                cout << it->second[i] << " ";
            }
            cout << "\n";
        }
    }
};