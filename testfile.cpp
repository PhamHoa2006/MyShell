#include <iostream>
#include <string>
#include <vector>
#include "Feature\file.h"

using namespace std;

int main() {
    FileManager fileManager;

    // Test checkFileExistence
    cout << "Testing checkFileExistence:" << endl;
    fileManager.checkFileExistence({"test1.txt", "test2.txt"});
    
    // Test createFile
    cout << "\nTesting createFile:" << endl;
    fileManager.createFile({"test1.txt", "test2.txt"});

    // Test deleteFile
    cout << "\nTesting deleteFile:" << endl;
    fileManager.deleteFile({"test1.txt", "test2.txt"});
     
    // Test fileSize on non-existent files
    cout << "\nTesting fileSize on non-existent files:" << endl;
    fileManager.fileSize({"test1.txt", "test2.txt"});

    // test checkFileExistence again to confirm files were deleted
    cout << "\nTesting checkFileExistence again:" << endl;
    fileManager.checkFileExistence({"test1.txt", "test2.txt"});

    // Test createFile again to test fileSize
    cout << "\nTesting createFile again:" << endl;
    fileManager.createFile({"test1.txt", "test2.txt"});

    // Test writeFile
    cout << "\nTesting writeFile:" << endl;
    fileManager.writeFile({"Hello, World!", "test1.txt"});
    fileManager.writeFile({"This is a test file.", "test1.txt", "HEAD"});
    fileManager.writeFile({"This is the end of the file.", "test1.txt", "FOOT"});
    fileManager.writeFile({"This line is inserted at line 2.", "test1.txt", "LINE", "2"}); 

    //test readFile
    cout << "\nTesting readFile:" << endl;
    fileManager.readFile({"test1.txt"});
    fileManager.readFile({"test1.txt", "HEAD", "2"});
    fileManager.readFile({"test1.txt", "FOOT", "2"});
    fileManager.readFile({"test1.txt", "LINE", "3"});
    fileManager.readFile({"test1.txt", "RANGE", "2", "4"});
    return 0;
}
