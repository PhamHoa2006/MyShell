/*
 * ============================================================================
 * Exercise 01: Anonymous Pipes (Ống dẫn ẩn danh)
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrate one-way inter-process communication using anonymous pipes.
 *   Minh họa giao tiếp một chiều giữa các tiến trình bằng ống dẫn ẩn danh.
 *
 *   The parent process creates a pipe, spawns a child process, writes data
 *   to the pipe, and the child reads it.
 *   Tiến trình cha tạo pipe, sinh tiến trình con, ghi dữ liệu vào pipe,
 *   và tiến trình con đọc dữ liệu từ pipe.
 *
 * Key APIs (Các API chính):
 *   - CreatePipe()           : Create an anonymous pipe (Tạo ống dẫn ẩn danh)
 *   - WriteFile()            : Write data to pipe (Ghi dữ liệu vào pipe)
 *   - ReadFile()             : Read data from pipe (Đọc dữ liệu từ pipe)
 *   - CreateProcess()        : Create child process (Tạo tiến trình con)
 *   - SetHandleInformation() : Control handle inheritance (Kiểm soát kế thừa handle)
 *
 * Compile (Biên dịch):
 *   g++ 01_anonymous_pipe.cpp -o 01_anonymous_pipe.exe
 *
 * Usage (Cách dùng):
 *   01_anonymous_pipe.exe          (run as parent / chạy với vai trò cha)
 *   01_anonymous_pipe.exe --child  (run as child / chạy với vai trò con - auto)
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Error helper - In thông báo lỗi kèm mã lỗi Windows
// ============================================================================
void PrintError(const char* context) {
    DWORD err = GetLastError();
    char buf[512];
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, err, 0, buf, sizeof(buf), NULL
    );
    fprintf(stderr, "[ERROR] %s: (%lu) %s", context, err, buf);
}

// ============================================================================
// CHILD PROCESS MODE (Chế độ tiến trình con)
// The child reads from stdin (which is the read-end of the pipe).
// Tiến trình con đọc từ stdin (chính là đầu đọc của pipe).
// ============================================================================
void RunAsChild() {
    printf("[CHILD] Child process started. PID = %lu\n", GetCurrentProcessId());
    printf("[CHILD] Tien trinh con da khoi dong. PID = %lu\n\n", GetCurrentProcessId());

    // stdin has been redirected to the pipe's read end by the parent.
    // stdin đã được chuyển hướng đến đầu đọc của pipe bởi tiến trình cha.
    HANDLE hReadPipe = GetStdHandle(STD_INPUT_HANDLE);
    if (hReadPipe == INVALID_HANDLE_VALUE) {
        PrintError("GetStdHandle(STD_INPUT_HANDLE)");
        return;
    }

    printf("[CHILD] Reading messages from parent via anonymous pipe...\n");
    printf("[CHILD] Dang doc tin nhan tu tien trinh cha qua pipe an danh...\n\n");

    // Read loop: read data until pipe is closed (parent closes write end).
    // Vòng lặp đọc: đọc dữ liệu cho đến khi pipe bị đóng.
    char buffer[256];
    DWORD bytesRead;
    int msgCount = 0;

    while (true) {
        // ReadFile() blocks until data is available or the write end is closed.
        // ReadFile() sẽ chặn (block) cho đến khi có dữ liệu hoặc đầu ghi bị đóng.
        BOOL success = ReadFile(
            hReadPipe,   // Handle to read from (Handle để đọc)
            buffer,      // Buffer to store data (Bộ đệm lưu dữ liệu)
            sizeof(buffer) - 1, // Max bytes to read (Số byte tối đa)
            &bytesRead,  // Actual bytes read (Số byte thực sự đọc được)
            NULL         // Not using overlapped I/O (Không dùng I/O chồng lấp)
        );

        if (!success || bytesRead == 0) {
            // Pipe closed or error
            // Pipe đã đóng hoặc có lỗi
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                printf("\n[CHILD] Pipe closed by parent. (Pipe da bi dong boi tien trinh cha.)\n");
            } else if (bytesRead == 0) {
                printf("\n[CHILD] End of pipe data. (Het du lieu trong pipe.)\n");
            } else {
                PrintError("ReadFile from pipe");
            }
            break;
        }

        // Null-terminate the data and display it
        // Thêm ký tự null vào cuối và hiển thị
        buffer[bytesRead] = '\0';
        msgCount++;
        printf("  [CHILD] Message #%d (%lu bytes): \"%s\"\n",
               msgCount, bytesRead, buffer);
    }

    printf("\n[CHILD] Total messages received: %d\n", msgCount);
    printf("[CHILD] Tong so tin nhan da nhan: %d\n", msgCount);
    printf("[CHILD] Child process exiting. (Tien trinh con ket thuc.)\n");
}

// ============================================================================
// PARENT PROCESS MODE (Chế độ tiến trình cha)
// The parent creates the pipe, spawns a child, and writes data.
// Tiến trình cha tạo pipe, sinh tiến trình con, và ghi dữ liệu.
// ============================================================================
void RunAsParent() {
    printf("=== Anonymous Pipe Demo (Minh hoa Pipe An Danh) ===\n");
    printf("Parent PID = %lu\n\n", GetCurrentProcessId());

    // -----------------------------------------------------------------------
    // Step 1: Create anonymous pipe
    // Bước 1: Tạo ống dẫn ẩn danh
    // -----------------------------------------------------------------------
    // CreatePipe creates a pair of handles: one for reading, one for writing.
    // CreatePipe tạo một cặp handle: một để đọc, một để ghi.
    HANDLE hReadPipe  = NULL;
    HANDLE hWritePipe = NULL;

    // SECURITY_ATTRIBUTES: bInheritHandle = TRUE allows child to inherit handles.
    // SECURITY_ATTRIBUTES: bInheritHandle = TRUE cho phép tiến trình con kế thừa handle.
    SECURITY_ATTRIBUTES sa;
    sa.nLength              = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle       = TRUE;   // Child can inherit (Con có thể kế thừa)
    sa.lpSecurityDescriptor = NULL;

    printf("[PARENT] Step 1: Creating anonymous pipe...\n");
    printf("[PARENT] Buoc 1: Tao pipe an danh...\n");

    if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
        PrintError("CreatePipe");
        return;
    }
    printf("[PARENT] Pipe created successfully!\n");
    printf("[PARENT]   Read handle  = %p\n", hReadPipe);
    printf("[PARENT]   Write handle = %p\n\n", hWritePipe);

    // -----------------------------------------------------------------------
    // Step 2: Ensure the write handle is NOT inherited by child
    // Bước 2: Đảm bảo handle ghi KHÔNG được kế thừa bởi tiến trình con
    // -----------------------------------------------------------------------
    // The child only needs the read handle. If the child also inherits the
    // write handle, it won't detect EOF properly.
    // Tiến trình con chỉ cần handle đọc. Nếu con cũng kế thừa handle ghi,
    // nó sẽ không phát hiện EOF đúng cách.
    printf("[PARENT] Step 2: Preventing write handle from being inherited...\n");
    printf("[PARENT] Buoc 2: Ngan handle ghi bi ke thua...\n");

    if (!SetHandleInformation(hWritePipe, HANDLE_FLAG_INHERIT, 0)) {
        PrintError("SetHandleInformation");
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return;
    }
    printf("[PARENT] Write handle marked as non-inheritable.\n\n");

    // -----------------------------------------------------------------------
    // Step 3: Create child process with redirected stdin
    // Bước 3: Tạo tiến trình con với stdin được chuyển hướng
    // -----------------------------------------------------------------------
    printf("[PARENT] Step 3: Creating child process...\n");
    printf("[PARENT] Buoc 3: Tao tien trinh con...\n");

    // Get our own executable name to re-launch as child
    // Lấy tên file thực thi của chính mình để chạy lại dưới vai trò con
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    char cmdLine[MAX_PATH + 32];
    snprintf(cmdLine, sizeof(cmdLine), "\"%s\" --child", exePath);

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    // Redirect child's stdin to the pipe's read end
    // Chuyển hướng stdin của con đến đầu đọc của pipe
    si.hStdInput  = hReadPipe;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE); // Share parent's stdout
    si.hStdError  = GetStdHandle(STD_ERROR_HANDLE);  // Share parent's stderr
    si.dwFlags   |= STARTF_USESTDHANDLES;

    BOOL success = CreateProcessA(
        NULL,           // Application name (NULL = use cmdLine)
        cmdLine,        // Command line: ourselves with --child flag
        NULL,           // Process security attributes
        NULL,           // Thread security attributes
        TRUE,           // bInheritHandles: child inherits our handles
        0,              // Creation flags
        NULL,           // Use parent's environment
        NULL,           // Use parent's working directory
        &si,            // Startup info with redirected handles
        &pi             // Process info output
    );

    if (!success) {
        PrintError("CreateProcess");
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return;
    }
    printf("[PARENT] Child process created! PID = %lu\n\n", pi.dwProcessId);

    // Close read end in parent - parent only writes, child only reads.
    // Đóng đầu đọc ở cha - cha chỉ ghi, con chỉ đọc.
    CloseHandle(hReadPipe);

    // -----------------------------------------------------------------------
    // Step 4: Write messages to the pipe
    // Bước 4: Ghi các tin nhắn vào pipe
    // -----------------------------------------------------------------------
    printf("[PARENT] Step 4: Writing messages to pipe...\n");
    printf("[PARENT] Buoc 4: Ghi tin nhan vao pipe...\n\n");

    const char* messages[] = {
        "Hello from parent!",
        "This is message number 2.",
        "Pipes are great for IPC!",
        "Xin chao tu tien trinh cha!",
        "Goodbye! (Tam biet!)"
    };
    int numMessages = sizeof(messages) / sizeof(messages[0]);

    for (int i = 0; i < numMessages; i++) {
        DWORD bytesWritten;
        DWORD msgLen = (DWORD)strlen(messages[i]);

        // WriteFile writes data to the pipe's write end.
        // WriteFile ghi dữ liệu vào đầu ghi của pipe.
        BOOL writeOk = WriteFile(
            hWritePipe,     // Handle to write to (Handle để ghi)
            messages[i],    // Data to write (Dữ liệu cần ghi)
            msgLen,         // Number of bytes (Số byte)
            &bytesWritten,  // Bytes actually written (Số byte thực sự ghi)
            NULL            // Not overlapped (Không chồng lấp)
        );

        if (!writeOk) {
            PrintError("WriteFile to pipe");
            break;
        }

        printf("  [PARENT] Sent message #%d (%lu bytes): \"%s\"\n",
               i + 1, bytesWritten, messages[i]);

        // Small delay so output is readable
        // Delay nhỏ để output dễ đọc
        Sleep(200);
    }

    // -----------------------------------------------------------------------
    // Step 5: Close write handle (signals EOF to child)
    // Bước 5: Đóng handle ghi (báo hiệu EOF cho tiến trình con)
    // -----------------------------------------------------------------------
    printf("\n[PARENT] Step 5: Closing write handle (signal EOF to child)...\n");
    printf("[PARENT] Buoc 5: Dong handle ghi (bao hieu EOF cho con)...\n");
    CloseHandle(hWritePipe);

    // -----------------------------------------------------------------------
    // Step 6: Wait for child process to finish
    // Bước 6: Đợi tiến trình con kết thúc
    // -----------------------------------------------------------------------
    printf("[PARENT] Step 6: Waiting for child process to finish...\n");
    printf("[PARENT] Buoc 6: Doi tien trinh con ket thuc...\n\n");

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    printf("[PARENT] Child process exited with code: %lu\n", exitCode);

    // Clean up process handles
    // Dọn dẹp các handle tiến trình
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printf("\n=== Anonymous Pipe Demo Complete (Hoan tat minh hoa Pipe An Danh) ===\n");
}

// ============================================================================
// Main entry point - Điểm vào chính
// ============================================================================
int main(int argc, char* argv[]) {
    // Check if we're running as child process
    // Kiểm tra xem chúng ta có đang chạy dưới vai trò tiến trình con không
    if (argc > 1 && strcmp(argv[1], "--child") == 0) {
        RunAsChild();
    } else {
        RunAsParent();
    }

    return 0;
}
