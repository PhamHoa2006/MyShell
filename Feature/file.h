// quan ly file

#ifndef FILE_H
#define FILE_H

#include <string>
#include <filesystem>
#include <iostream>
#include <unordered_set>    
#include <vector>
#include <windows.h>
#include <conio.h>
#include <fstream>



using namespace std;
namespace fs = std::filesystem;

// Đã có các hàm :     
// - checkFileExistence: Kiểm tra sự tồn tại của một hay nhiều file
// - createFile: Tạo một hoặc nhiều file
// - deleteFile: Xóa một hoặc nhiều file
// - openFile: Mở một hoặc nhiều file
// - renameFile: Đổi tên một hoặc nhiều file
// - writeFile: Ghi nội dung vào một hoặc nhiều file (chưa viết xong)


class FileManager {
public:
    FileManager();
    static const unordered_set<string> supportedCommands;
    static const unordered_set<string>& getSupportedCommands(){
        return supportedCommands;
    };
    // Hàm để kiểm tra sự tồn tại của một hay nhiều file
    void checkFileExistence(const vector<string>& fileNames){
        for (const auto& fileName : fileNames) {
            if (fs::exists(fileName)) {
                cout << "File exists: " << fileName << endl;
            } else {
                cout << "File does not exist: " << fileName << endl;
            }
        }
    }
    // Hàm để tạo một hoặc nhiều file
    void createFile(const vector<string>& fileNames){
        for (const auto& fileName : fileNames) {
            ofstream file(fileName);
            if (file) {
                cout << "File created successfully: " << fileName << endl;
            } else {
                cerr << "Failed to create file: " << fileName << endl;
            }
        }
    }
    // Hàm để xóa một hoặc nhiều file
    void deleteFile(const vector<string>& fileNames){
        for (const auto& fileName : fileNames) {
            if (fs::remove(fileName)){
                cout << "File deleted " << fileName << endl;
            }
            else {
                cerr << "Failed to delete file: " << fileName << endl;
            }
        }
    }

    // Hàm để open file
    void openFile(const vector<string>& fileNames){
        for(const auto& fileName : fileNames){
            if (fs::exists(fileName)){
                ShellExecuteA(NULL, "open", fileName.c_str(), NULL, NULL, SW_SHOWNORMAL);
                cout << "File opened: " << fileName << endl;
            }
            else {
                cerr << "File does not exist: " << fileName << endl;
            }
        }
    }

    // Hàm để đổi tên file
    void renameFile(const vector<string>& args){
        if (args.size() != 2){
            cerr << "Usage: rename <old_file_path> <new_file_path>" << endl;
            return;
        }
        fs::path oldPath = args[0];
        fs::path newPath = args[1];
        if (!fs::exists(oldPath)){
            cerr << "File does not exist: " << oldPath << endl;
            return;
        }
        fs::rename(oldPath, newPath);
        cout << "File renamed from " << oldPath << " to " << newPath << endl;
    }

    // Hàm để ghi nội dung vào file
    // Nếu chỉ có 2 tham số, ghi vào cuối file
    // Nếu có 3 tham số, xác định vị trí ghi: ~HEAD, ~FOOT, ~LINE N
    void writeFile(const std::vector<std::string>& args){
        if (args.size() < 2){
            cerr << "Cu phap sai. Usage: write_file <content> <filename> [~HEAD | ~FOOT | ~LINE N]" << endl;
            return;
        }
        string content = args[0]; 
        string filename = args[1];
        if (args.size() == 2){
            ofstream file(filename);
            if (!file){
                cerr << "File not found: " << filename << endl;
                return;
            }
            file << content << endl;
            file.close();
        }

    }


private:
    static bool interrupted;
    static void signalHandler(int signal);
    std::unordered_set<std::string> files;


};

#endif // FILE_H