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
        // TODO: Implement
        // 1. Push command to history vector
        // 2. Append command to history file
    }

    // Show all commands in history
    void show() const
    {
        // TODO: Implement
        // Loop through history vector, print with index numbers
    }

    // Load history from file (call once at shell startup)
    void load()
    {
        // TODO: Implement
        // Read history.txt line by line into history vector
    }

    // Clear all history
    void clear()
    {
        // TODO: Implement
        // 1. Clear the vector
        // 2. Truncate the file (open with ofstream::trunc)
    }

private:
    vector<string> history;
    const string historyFile = "history.txt";
};

#endif // HISTORY_H
