// test directory

#include <iostream>
#include <string>
#include <vector>
#include "Feature\directory.h"

using namespace std;

int main() {
    DirectoryManager directoryManager;

    // test create Directory
    directoryManager.createDirectory({"testPath"});
    directoryManager.moveDirectory({"testPath", "Test"});

    return 0;
}