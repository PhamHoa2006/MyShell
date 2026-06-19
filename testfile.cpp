#include <iostream>
#include <string>
#include <vector>
#include "Feature\file.h"

using namespace std;

int main() {
    FileManager fileManager;
    // test createFile
    fileManager.createFile({"test1.txt", "test2.txt"});
    // test openfile
    vector<string> filesToOpen = {"test1.txt"};
    fileManager.openFile(filesToOpen);
    return 0;
}
