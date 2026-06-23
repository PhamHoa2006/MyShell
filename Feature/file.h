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
#include <algorithm> // Thêm thư viện này để dùng hàm min

using namespace std;
namespace fs = std::filesystem;

class FileManager {
public:
    FileManager(){};
    static const unordered_set<string> supportedCommands;
    static const unordered_set<string>& getSupportedCommands(){
        return supportedCommands;
    };

    void checkFileExistence(const vector<string>& fileNames){
        for (const auto& fileName : fileNames) {
            if (fs::exists(fileName)) {
                cout << "File exists: " << fileName << endl;
            } else {
                cout << "File does not exist: " << fileName << endl;
            }
        }
    }

    void fileSize(const vector<string>& fileNames){
        for (const auto& fileName : fileNames) {
            if (fs::is_directory(fileName)){
                cout << "Not file";
                return;
            }
            if (fs::exists(fileName)) {
                cout << "Size of file " << fileName << ": " << fs::file_size(fileName) << " bytes" << endl;
            } else {
                cout << "File does not exist: " << fileName << endl;
            }
        }
    }

    void createFile(const vector<string>& fileNames){
    for (const auto& fileName : fileNames) {
        // Kiểm tra xem file đã tồn tại chưa
        if (fs::exists(fileName)) {
            cout << "File already exists : " << fileName << endl;
            continue; // Nhảy sang file tiếp theo, không ghi đè
        }

        ofstream file(fileName);
        if (file) {
            cout << "File created successfully: " << fileName << endl;
        } else {
            cerr << "Failed to create file: " << fileName << endl;
        }
    }
}

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
    void openFile(const vector<string>& fileNames) {
        for (const auto& fileName : fileNames) {
            
            // Gọi ShellExecuteA trực tiếp. Nếu thành công, nó trả về một giá trị > 32
            HINSTANCE result = ShellExecuteA(NULL, "open", fileName.c_str(), NULL, NULL, SW_SHOWNORMAL);
            
            // Ép kiểu về INT_PTR để kiểm tra lỗi
            if ((INT_PTR)result > 32) {
                cout << "Successfully opened/executed: " << fileName << endl;
            } else {
                // Nếu thất bại (có thể do file không tồn tại, hoặc không có ứng dụng nào hỗ trợ mở)
                cerr << "Failed to open or execute: " << fileName << " (Error code: " << (INT_PTR)result << ")" << endl;
            }
        }
    }

    void renameFile(const vector<string>& args){
        if (args.size() != 2){
            cerr << "Usage: rename <old_file_name> <new_file_name>" << endl;
            return;
        }
        fs::path oldName = args[0];
        fs::path newName = args[1];
        if (!fs::exists(oldName)){
            cerr << "File does not exist: " << oldName << endl;
            return;
        }
        fs::rename(oldName, newName);
        cout << "File renamed from " << oldName << " to " << newName << endl;
    }

    // ĐÃ SỬA: Thêm ios::app để ghi vào cuối file khi có 2 tham số
    void writeFile(const std::vector<std::string>& args){
        if (args.size() < 2){
            cerr << "Cu phap sai. Usage: write_file <content> <filename> [HEAD | FOOT | LINE N]" << endl;
            return;
        }
        string content = args[0]; 
        string filename = args[1];
        if (args.size() == 2){
            ofstream file(filename, ios::app); // Sửa tại đây để không bị xóa file cũ
            if (!file){
                cerr << "File not found: " << filename << endl;
                return;
            }
            file << content << endl;
            file.close();
        }
        else if (args.size() == 3 || (args.size() == 4 && args[2] == "LINE")){
            string position = args[2];
            ifstream file_in(filename);
            if (!file_in){
                cerr << "File not found: " << filename << endl;
                return;
            }
            vector<string> lines;
            string line;
            while (getline(file_in, line)){
                lines.push_back(line);
            }
            file_in.close();
            if (position == "HEAD"){
                lines.insert(lines.begin(), content);
            }
            else if (position == "FOOT"){
                lines.push_back(content);
            }
            else if (position == "LINE"){
                if (args.size() != 4){
                    cerr << "Cu phap sai. Usage: write_file <content> <filename> LINE N" << endl;
                    return;
                }
                int lineNumber = stoi(args[3]);
                if (lineNumber < 1 || lineNumber > (int)lines.size() + 1){
                    cerr << "Line number out of range. Must be between 1 and " << lines.size() + 1 << endl;
                    return;
                }
                lines.insert(lines.begin() + lineNumber - 1, content);
            }
            ofstream file_out(filename);
            if (!file_out){
                cerr << "Failed to open file for writing: " << filename << endl;
                return;
            }
            for (const auto& l : lines){
                file_out << l << endl;
            }
            file_out.close();
        }
    }

    // ĐÃ SỬA: Sửa lại toàn bộ logic đọc file nâng cao tránh crash stoi()
    void readFile(const vector<string>& args){
        if (args.size() < 1){ // Sửa từ < 2 thành < 1
            cerr << "Usage: read_file <filename> [HEAD N | FOOT N | RANGE M N | LINE N]" << endl;
            return;
        }
        string filename = args[0];
        ifstream file(filename);
        if (!file){
            cerr << "File not found: " << filename << endl;
            return;
        }
        vector<string> lines;
        string line;
        while (getline(file, line)){
            lines.push_back(line);
        }
        file.close();

        if (args.size() == 1){
            for (const auto& l : lines){
                cout << l << endl;
            }
        }
        else if (args.size() >= 3) {
            string option = args[1];
            if (option == "HEAD") {
                int numLines = stoi(args[2]);
                for (int i = 0; i < min(numLines, (int)lines.size()); ++i){
                    cout << lines[i] << endl;
                }
            }
            else if (option == "FOOT") {
                int numLines = stoi(args[2]);
                int start = max(0, (int)lines.size() - numLines);
                for (size_t i = start; i < lines.size(); ++i){
                    cout << lines[i] << endl;
                }
            }
            else if (option == "LINE") {
                int lineNum = stoi(args[2]);
                if (lineNum >= 1 && lineNum <= (int)lines.size()) {
                    cout << lines[lineNum - 1] << endl;
                } else {
                    cerr << "Line number out of range." << endl;
                }
            }
            else if (option == "RANGE" && args.size() == 4) {
                int m = stoi(args[2]);
                int n = stoi(args[3]);
                int start = max(1, m);
                int end = min(n, (int)lines.size());
                for (int i = start - 1; i < end; ++i) {
                    cout << lines[i] << endl;
                }
            }
        }
    }
    void moveFile(const vector<string>& args){
    if (args.size() != 2){
        cerr << "Usage: move_file <source_path> <destination_path>" << endl;
        return;
    }

    fs::path sourcePath = args[0];
    fs::path destinationPath = args[1];

    if (!fs::exists(sourcePath)){
        cerr << "Source file does not exist: " << sourcePath << endl;
        return;
    }

    // SỬA : Nếu đích là thư mục, tự động nối thêm tên file vào sau thư mục đó
    if (fs::is_directory(destinationPath)) {
        destinationPath = destinationPath / sourcePath.filename();
    }

    try {
        // Thử cách nhanh nhất (cùng ổ đĩa)
        fs::rename(sourcePath, destinationPath);
        cout << "File moved from " << sourcePath << " to " << destinationPath << endl;
    } 
    catch (const fs::filesystem_error& e) {
        // SỬA : Xử lý khi di chuyển khác ổ đĩa (ví dụ từ C: sang D:)
        try {
            // Copy file sang nơi mới (ghi đè nếu đã tồn tại)
            fs::copy_file(sourcePath, destinationPath, fs::copy_options::overwrite_existing);
            // Xóa file ở nơi cũ
            fs::remove(sourcePath);
            
            cout << "File moved (cross-drive) from " << sourcePath << " to " << destinationPath << endl;
        } 
        catch (const exception& ex) {
            cerr << "Failed to move file cross-drive: " << ex.what() << endl;
        }
    }
}
private:
    
};
const unordered_set<string> FileManager::supportedCommands= {
        "write_file",
        "read_file",
        "size_file",
        "move_file",
        "open",
        "rename"
    };
#endif // FILE_H