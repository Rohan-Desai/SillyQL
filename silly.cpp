// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

#include <vector>
#include <iostream>
#include <deque>
#include <unordered_map>
#include <string>
#include <array>
#include <algorithm>
#include <getopt.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include "silly.h"
using namespace std;

int main(int argc, char *argv[])
{
    ios_base::sync_with_stdio(false);
    cin >> std::boolalpha; // add these two lines
    cout << std::boolalpha;
    Silly database;
    bool quietMode = false;
    database.processArgs(argc, argv, quietMode);
    // cout << quietMode << "\n";
    char cmd;
    string junk;
    string tableName;
    string printCommand;
    int printCols;
    string selectedCol;
    Table currTable;
    int rowCount;
    string colName;
    string OP;
    unordered_map<string, Table> sillyQL;
    do
    {
        if (cin.fail())
        {
            cerr << "Error: Reading from cin has failed" << "\n";
            getline(cin, junk);
            exit(1);
        }
        cout << "% ";
        cin >> cmd;
        switch (cmd)
        {
        case '#':
            getline(cin, junk);
            break;
        case 'C':
            // create table and insert into database unordered map
            cin >> junk;
            //cout << junk << endl;
            currTable = database.create(tableName);
            if (sillyQL.count(tableName) != 0)
            {
                cout << "Error during CREATE: Cannot create already existing table " << tableName << "\n";
                getline(cin, junk);
                break;
            }
            sillyQL.insert(make_pair(tableName, currTable));
            cout << "New table " << tableName << " with column(s) ";
            currTable.printCols();
            cout << "created" << "\n";
            break;
        case 'R':
            // Remove table from database unordered map
            cin >> junk;
            cin >> tableName;
            if (sillyQL.count(tableName) == 0)
            {
                cout << "Error during REMOVE: " << tableName << " does not name a table in the database" << "\n";
                //getline(cin, junk);
                break;
            }
            sillyQL.erase(tableName);
            cout << "Table " << tableName << " deleted" << "\n";
            break;
        case 'Q':
            break;
        case 'I':
        {
            // Insert rows into table from database unordered map
            cin >> junk;
            cin >> junk;
            cin >> tableName;
            cin >> rowCount;
            cin >> junk;
            if (sillyQL.count(tableName) == 0)
            {
                cout << "Error during INSERT: " << tableName << " does not name a table in the database" << "\n";
                getline(cin, junk);
                break;
            }
            sillyQL[tableName].resizeDataVec(rowCount);
            int sizePrior = sillyQL[tableName].getNumDataRows();
            for (int i = 0; i < rowCount; i++)
            {
                sillyQL[tableName].addDataRow(database.createRow(sillyQL[tableName]));
            }
            cout << "Added " << rowCount << " rows to " << tableName << " from position " << sizePrior - rowCount << " to " << sizePrior - 1 << "\n";
            break;
        }
        case 'P':
        {
            cin >> junk;
            cin >> junk;
            cin >> tableName;
            cin >> printCols;
            vector<string> selectedCols;

            for (int i = 0; i < printCols; i++)
            {
                cin >> selectedCol;
                selectedCols.push_back(selectedCol);
            }
            if (sillyQL.count(tableName) == 0)
            {
                cout << "Error during PRINT: " << tableName << " does not name a table in the database" << "\n";
                getline(cin, junk);
                break;
            }
            for (auto x : selectedCols)
            {
                if (!sillyQL[tableName].checkCols(x))
                {
                    cout << "Error during PRINT: " << x << " does not name a column in " << tableName << "\n";
                    getline(cin, junk);
                    break;
                }
            }

            cin >> printCommand;
            int rowsPrinted = 0;
            if (printCommand == "ALL")
            {
                // print headers after retreiving them
                if (!quietMode)
                {
                    for (size_t j = 0; j < selectedCols.size(); j++)
                    {
                        cout << selectedCols[j] << " ";
                    }
                    cout << "\n";
                }
                sillyQL[tableName].printAll(selectedCols, rowsPrinted, quietMode);
                cout << "Printed " << rowsPrinted << " matching rows from " << tableName << "\n";
            }
            else if (printCommand == "WHERE")
            {
                cin >> colName;
                // cout << colName << "\n";
                if (!sillyQL[tableName].checkCols(colName))
                {
                    cout << "Error during PRINT: " << colName << " does not name a column in " << tableName << "\n";
                    getline(cin, junk);
                    break;
                }
                // print headers after retreiving them
                if (!quietMode)
                {
                    for (size_t j = 0; j < selectedCols.size(); j++)
                    {
                        cout << selectedCols[j] << " ";
                    }
                    cout << "\n";
                }
                cin >> OP;
                int colIndex = sillyQL[tableName].getColIndex(colName);
                EntryType valType = sillyQL[tableName].getDType(colIndex);
                TableEntry val = database.declareEntry(valType);
                sillyQL[tableName].printWhere(selectedCols, rowsPrinted, OP, val, colIndex, quietMode, colName);
                cout << "Printed " << rowsPrinted << " matching rows from " << tableName << "\n";
            }
            break;
        }
        case 'D':
        {
            cin >> junk;
            cin >> junk;
            cin >> tableName;
            if (sillyQL.count(tableName) == 0)
            {
                cout << "Error during DELETE: " << tableName << " does not name a table in the database" << "\n";
                getline(cin, junk);
                break;
            }
            cin >> junk;
            cin >> colName;
            if (!sillyQL[tableName].checkCols(colName))
            {
                cout << "Error during DELETE: " << colName << " does not name a column in " << tableName << "\n";
                getline(cin, junk);
                break;
            }
            cin >> OP;
            int colIndex = sillyQL[tableName].getColIndex(colName);
            int rowsDeleted = 0;
            EntryType valType = sillyQL[tableName].getDType(colIndex);
            TableEntry val = database.declareEntry(valType);
            sillyQL[tableName].deleteRow(rowsDeleted, OP, val, colIndex);
            cout << "Deleted " << rowsDeleted << " rows from " << tableName << "\n";
            break;
        }
        case 'J':
        {
            string tableName1;
            string tableName2;
            string colName1;
            string colName2;
            cin >> junk; // Join
            cin >> tableName1;
            if (sillyQL.count(tableName1) == 0)
            {
                cout << "Error during JOIN: " << tableName1 << " does not name a table in the database" << "\n";
                getline(cin, junk);
                break;
            }
            cin >> junk; // And
            cin >> tableName2;
            if (sillyQL.count(tableName2) == 0)
            {
                cout << "Error during JOIN: " << tableName2 << " does not name a table in the database" << "\n";
                getline(cin, junk);
                break;
            }
            cin >> junk; // WHere
            cin >> colName1;
            if (!sillyQL[tableName1].checkCols(colName1))
            {
                cout << "Error during JOIN: " << colName1 << " does not name a column in " << tableName1 << "\n";
                getline(cin, junk);
                break;
            }
            cin >> junk; //=
            cin >> colName2;
            if (!sillyQL[tableName2].checkCols(colName2))
            {
                cout << "Error during JOIN: " << colName2 << " does not name a column in " << tableName2 << "\n";
                getline(cin, junk);
                break;
            }
            cin >> junk; // ANd
            cin >> junk; // PRINT
            cin >> printCols;
            vector<std::pair<string, int>> selectedCols;
            vector<string> selectedTables;
            selectedTables.push_back(tableName1);
            selectedTables.push_back(tableName2);
            int tableNum = 0;
            for (int i = 0; i < printCols; i++)
            {
                cin >> selectedCol;
                cin >> tableNum;
                tableNum -= 1;
                if (!sillyQL[selectedTables[(size_t)tableNum]].checkCols(selectedCol))
                {
                    cout << "Error during JOIN: " << selectedCol << " does not name a column in " << selectedTables[(size_t)tableNum] << "\n";
                    getline(cin, junk);
                    break;
                }
                selectedCols.push_back(make_pair(selectedCol, tableNum));
            }
            if (!quietMode)
            {
                for (auto x : selectedCols)
                {
                    cout << x.first << " ";
                }
                cout << "\n";
            }
            int joinedRows = 0;
            database.joinTables(sillyQL[tableName1], sillyQL[tableName2], colName1, colName2, selectedCols, joinedRows, quietMode);
            cout << "Printed " << joinedRows << " rows from joining " << tableName1 << " to " << tableName2 << "\n";
            break;
        }
        case 'G':
        {
            string tableName;
            string indexType;
            string colName;
            cin >> junk; // GENERATE
            cin >> junk; // FOR
            cin >> tableName;
            if (sillyQL.count(tableName) == 0)
            {
                cout << "Error during GENERATE: " << tableName << " does not name a table in the database" << "\n";
                getline(cin, junk);
                break;
            }
            cin >> indexType;
            cin >> junk; // INDEX
            cin >> junk; // ON
            cin >> colName;
            if (!sillyQL[tableName].checkCols(colName))
            {
                cout << "Error during GENERATE: " << colName << " does not name a column in " << tableName << "\n";
                getline(cin, junk);
                break;
            }

            if (indexType == "hash")
            {
                sillyQL[tableName].generateHash(colName);
                // sillyQL[tableName].printHash();
            }
            else if (indexType == "bst")
            {
                sillyQL[tableName].generateBST(colName);
                // sillyQL[tableName].printBST();
            }
            cout << "Created " << indexType << " index for table " << tableName << " on column " << colName << "\n";
            break;
        }
        default:
            cout << "Error: unrecognized command" << "\n";
            getline(cin, junk);
            break;
        }
    } while (cmd != 'Q');
    cout << "Thanks for being silly!" << "\n";
    return 0;
}

// TODO: FIX INSERT WITH NO DATA
// TODO: ASK HOW TO PRINT USING THE MAP
// TODO: Submit to autograder
// TODO: IMPLEMENT GENERATE INDEX IN JOIN TO MAKE IT RUN FASTER
