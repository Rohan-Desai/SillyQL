// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <vector>
#include <iostream>
#include <deque>
#include <string>
#include <stdio.h>
#include <string.h>
#include <array>
#include <algorithm>
#include <getopt.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include "table.h"

using namespace std;

class Silly
{
private:
public:
    void processArgs(int argc, char *argv[], bool &quietMode)
    {
        if (argc == 1)
        {
            return;
        }
        if (argc == 2)
        {
            if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
            {
                cout << "helpful message" << "\n";
                exit(0);
            }
            else if (!strcmp(argv[1], "-q") || !strcmp(argv[1], "--quiet"))
            {
                quietMode = true;
            }
        }
    }
    Table create(string &tableName)
    {
        // read in table name
        cin >> tableName;
        int size = 0;
        cin >> size;
        Table table;
        string dType;
        table.resizeVecs(size);
        // read in each of the type and name for each col
        for (int i = 0; i < size; i++)
        {
            cin >> dType;
            //cout << dType << " " << endl;
            EntryType type = EntryType::String;
            if (dType == "string")
            {
                type = EntryType::String;
            }
            else if (dType == "int")
            {
                type = EntryType::Int;
            }
            else if (dType == "double")
            {
                type = EntryType::Double;
            }
            else if (dType == "bool")
            {
                type = EntryType::Bool;
            }
            table.addType(type);
        }

        for (int i = 0; i < size; i++)
        {
            string rowName;
            cin >> rowName;
            //cout << rowName << endl;
            table.addName(rowName, i);
        }
        return table;
    }
    vector<TableEntry> createRow(Table &currTable)
    {
        vector<TableEntry> row;
        int cols = currTable.getNumCols();
        EntryType type;
        row.clear();
        for (int j = 0; j < cols; j++)
        {
            type = currTable.getDType(j);
            row.emplace_back(declareEntry(type));
        }
        return row;
    }
    void joinTables(Table &table1, Table &table2, string colName1, string colName2, vector<std::pair<string, int>> selectedCols, int &rowsJoined, bool quietMode)
    {
        table2.generateHashJoin(colName2);
        unordered_map<TableEntry, vector<int>> table2Hash = table2.getHash();
        // table2.printHash();
        for (int i = 0; i < table1.getNumRows(); i++)
        {
            TableEntry entry1 = table1.getEntry(i, table1.getColIndex(colName1));
            if (table2Hash.count(entry1) != 0)
            {
                for (int j : table2Hash[entry1])
                {
                    if (!quietMode)
                    {
                        for (auto k : selectedCols)
                        {
                            //cout <<"k ="<<  k.second << " " << k.first << " " << "\n";
                            switch (k.second)
                            {
                            case 0:
                                cout << table1.getEntry(i, table1.getColIndex(k.first)) << " ";
                                break;
                            case 1:
                                cout << table2.getEntry(j, table2.getColIndex(k.first)) << " ";
                                break;
                            }
                        }
                        cout << "\n";
                    }
                    rowsJoined += 1;
                }
            }
        }
    }

    TableEntry declareEntry(EntryType type)
    {

        string enter;
        switch (type)
        {
        case EntryType::Double:
            double dEntry;
            cin >> dEntry;
            return TableEntry(dEntry);
            break;
        case EntryType::Int:
            int iEntry;
            cin >> iEntry;
            return TableEntry(iEntry);
            break;
        case EntryType::String:
            cin >> enter;
            return TableEntry(enter);
            break;
        case EntryType::Bool:
            bool entry;
            cin >> entry;
            return TableEntry(entry);
            break;
        default:
            cin >> enter;
            return TableEntry(enter);
            break;
        }
    }
};