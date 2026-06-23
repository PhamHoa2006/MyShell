// ============================================================================
// history.h — Command History
// ============================================================================
// Commands to implement:
//   history        — Show all past commands
//   clear_history  — Clear command history
//
// The history is saved to a file (history.txt) so it persists between sessions.
//
// Libraries needed:
//   #include <vector>    — Store commands in memory
//   #include <string>
//   #include <fstream>   — Read/write history file
//   #include <iostream>
//
// Reference: ../Operating-System-Projects/Feature/history.h
// ============================================================================

#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

class CommandHistory
{
public:
    // Add a command to history (call this every time user enters a command)
    void add(const string& command)
    {
        // 1. Push command to history vector
        history.push_back(command);
        // 2. Append command to history file
        ofstream outFile(historyFile, ios::app);
        if (outFile.is_open()) {
            outFile << command << "\n";
            outFile.close();
        }
    }

    // Show all commands in history
    void show() const
    {
        // Loop through history vector, print with index numbers
        for (size_t i = 0; i < history.size(); ++i) {
            cout << (i + 1) << "  " << history[i] << endl;
        }
    }

    // Load history from file (call once at shell startup)
    void load()
    {
        history.clear();
        ifstream inFile(historyFile);
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                if (!line.empty()) {
                    history.push_back(line);
                }
            }
            inFile.close();
        }
    }

    // Clear all history
    void clear()
    {
        // 1. Clear the vector
        history.clear();
        // 2. Truncate the file (open with ofstream::trunc)
        ofstream outFile(historyFile, ios::trunc);
        if (outFile.is_open()) {
            outFile.close();
        }
    }

private:
    vector<string> history;
    const string historyFile = "history.txt";
};

#endif // HISTORY_H
