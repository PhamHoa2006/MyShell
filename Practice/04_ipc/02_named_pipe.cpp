/*
 * ============================================================================
 * Exercise 02: Named Pipes (Ống dẫn có tên)
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrate bidirectional IPC using named pipes.
 *   Minh họa giao tiếp hai chiều giữa các tiến trình bằng ống dẫn có tên.
 *
 *   This single program uses threads to simulate both a pipe server and
 *   a pipe client. The server creates a named pipe and waits for a client.
 *   The client connects and they exchange messages.
 *   Chương trình này dùng luồng (thread) để mô phỏng cả server lẫn client.
 *   Server tạo named pipe và chờ client. Client kết nối và trao đổi tin nhắn.
 *
 * Key APIs (Các API chính):
 *   - CreateNamedPipe()  : Create a named pipe instance (Tạo named pipe)
 *   - ConnectNamedPipe() : Wait for client connection (Chờ client kết nối)
 *   - CreateFile()       : Client connects to named pipe (Client kết nối pipe)
 *   - WriteFile()        : Write data to pipe (Ghi dữ liệu)
 *   - ReadFile()         : Read data from pipe (Đọc dữ liệu)
 *   - DisconnectNamedPipe() : Disconnect client (Ngắt kết nối client)
 *
 * Compile (Biên dịch):
 *   g++ 02_named_pipe.cpp -o 02_named_pipe.exe
 *
 * Usage (Cách dùng):
 *   02_named_pipe.exe           (run both server & client in one process)
 *   02_named_pipe.exe --server  (run as server only)
 *   02_named_pipe.exe --client  (run as client only)
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// Named pipe path - must start with \\.\pipe\
// Đường dẫn named pipe - phải bắt đầu bằng \\.\pipe\
#define PIPE_NAME "\\\\.\\pipe\\MyTestPipe_IPC_Demo"

// ============================================================================
// Error helper - In thông báo lỗi
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
// PIPE SERVER (Máy chủ Pipe)
// Creates a named pipe, waits for a client, and exchanges messages.
// Tạo named pipe, chờ client, và trao đổi tin nhắn.
// ============================================================================
#define PIPE_NAME "\\\\.\\pipe\\my_test_pipe"

DWORD WINAPI PipeServerThread(LPVOID lpParam) {
    printf("[SERVER] Starting pipe server...\n");
    printf("[SERVER] Khoi dong may chu pipe...\n");
    printf("[SERVER] Pipe name: %s\n\n", PIPE_NAME);

    // -----------------------------------------------------------------------
    // Step 1: Create the named pipe
    // Bước 1: Tạo named pipe
    // -----------------------------------------------------------------------
    // CreateNamedPipe creates a new named pipe or an instance of an existing one.
    // CreateNamedPipe tạo named pipe mới hoặc một thể hiện (instance) của pipe đã tồn tại.
    HANDLE hPipe = CreateNamedPipeA(
        PIPE_NAME,                    // Pipe name (Tên pipe)
        PIPE_ACCESS_DUPLEX,           // Bidirectional (Hai chiều: đọc + ghi)
        PIPE_TYPE_MESSAGE |           // Data written as messages (Ghi dạng tin nhắn)
        PIPE_READMODE_MESSAGE |       // Data read as messages (Đọc dạng tin nhắn)
        PIPE_WAIT,                    // Blocking mode (Chế độ chặn)
        1,                            // Max instances (Số instance tối đa)
        512,                          // Output buffer size (Kích thước buffer xuất)
        512,                          // Input buffer size (Kích thước buffer nhập)
        0,                            // Default timeout (Thời gian chờ mặc định)
        NULL                          // Default security (Bảo mật mặc định)
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        PrintError("CreateNamedPipe");
        return 1;
    }
    printf("[SERVER] Named pipe created successfully!\n");

    // -----------------------------------------------------------------------
    // Step 2: Wait for a client to connect
    // Bước 2: Chờ client kết nối
    // -----------------------------------------------------------------------
    // ConnectNamedPipe blocks until a client connects with CreateFile.
    // ConnectNamedPipe chặn (block) cho đến khi client kết nối bằng CreateFile.
    printf("[SERVER] Waiting for client to connect...\n");
    printf("[SERVER] Dang cho client ket noi...\n\n");

    BOOL connected = ConnectNamedPipe(hPipe, NULL);
    if (!connected && GetLastError() != ERROR_PIPE_CONNECTED) {
        PrintError("ConnectNamedPipe");
        CloseHandle(hPipe);
        return 1;
    }
    printf("[SERVER] Client connected! (Client da ket noi!)\n\n");

    // -----------------------------------------------------------------------
    // Step 3: Read-write loop - exchange messages
    // Bước 3: Vòng lặp đọc-ghi - trao đổi tin nhắn
    // -----------------------------------------------------------------------
    char buffer[512];
    DWORD bytesRead, bytesWritten;

    // Read messages from client and send replies
    // Đọc tin nhắn từ client và gửi phản hồi
    for (int round = 0; round < 3; round++) {
        // --- Read from client (Đọc từ client) ---
        ZeroMemory(buffer, sizeof(buffer));
        BOOL readOk = ReadFile(
            hPipe,              // Pipe handle
            buffer,             // Buffer to receive data (Bộ đệm nhận dữ liệu)
            sizeof(buffer) - 1, // Max bytes (Số byte tối đa)
            &bytesRead,         // Bytes read (Số byte đọc được)
            NULL                // Not overlapped
        );

        if (!readOk || bytesRead == 0) {
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                printf("[SERVER] Client disconnected.\n");
            } else {
                PrintError("Server ReadFile");
            }
            break;
        }

        buffer[bytesRead] = '\0';
        printf("[SERVER] Received from client: \"%s\" (%lu bytes)\n",
               buffer, bytesRead);

        // --- Prepare and send reply (Chuẩn bị và gửi phản hồi) ---
        char reply[512];
        snprintf(reply, sizeof(reply),
                 "Server reply #%d: I received your message '%s'", round + 1, buffer);

        BOOL writeOk = WriteFile(
            hPipe,                      // Pipe handle
            reply,                      // Data to send (Dữ liệu gửi)
            (DWORD)strlen(reply),       // Bytes to write
            &bytesWritten,              // Bytes written
            NULL                        // Not overlapped
        );

        if (!writeOk) {
            PrintError("Server WriteFile");
            break;
        }
        printf("[SERVER] Sent reply: \"%s\" (%lu bytes)\n\n", reply, bytesWritten);
    }

    // -----------------------------------------------------------------------
    // Step 4: Clean up
    // Bước 4: Dọn dẹp
    // -----------------------------------------------------------------------
    // FlushFileBuffers ensures all data is written before disconnecting.
    // FlushFileBuffers đảm bảo tất cả dữ liệu được ghi trước khi ngắt.
    FlushFileBuffers(hPipe);

    // DisconnectNamedPipe disconnects the server end of the pipe.
    // DisconnectNamedPipe ngắt kết nối phía server của pipe.
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    printf("[SERVER] Pipe server stopped. (May chu pipe da dung.)\n");
    return 0;
}

// ============================================================================
// PIPE CLIENT (Client Pipe)
// Connects to the named pipe and exchanges messages with the server.
// Kết nối đến named pipe và trao đổi tin nhắn với server.
// ============================================================================
DWORD WINAPI PipeClientThread(LPVOID lpParam) {
    // Small delay to let server start first
    // Delay nhỏ để server khởi động trước
    Sleep(500);

    printf("[CLIENT] Starting pipe client...\n");
    printf("[CLIENT] Khoi dong pipe client...\n\n");

    // -----------------------------------------------------------------------
    // Step 1: Connect to the named pipe using CreateFile
    // Bước 1: Kết nối đến named pipe bằng CreateFile
    // -----------------------------------------------------------------------
    // For named pipes, CreateFile acts as "connect to server".
    // Với named pipe, CreateFile hoạt động như "kết nối đến server".
    printf("[CLIENT] Connecting to pipe: %s\n", PIPE_NAME);

    HANDLE hPipe = INVALID_HANDLE_VALUE;
    int retries = 0;
    const int MAX_RETRIES = 10;

    while (retries < MAX_RETRIES) {
        hPipe = CreateFileA(
            PIPE_NAME,            // Pipe name (Tên pipe)
            GENERIC_READ |        // Read access (Quyền đọc)
            GENERIC_WRITE,        // Write access (Quyền ghi)
            0,                    // No sharing (Không chia sẻ)
            NULL,                 // Default security
            OPEN_EXISTING,        // Must already exist (Phải đã tồn tại)
            0,                    // Default attributes
            NULL                  // No template
        );

        if (hPipe != INVALID_HANDLE_VALUE) break;

        // If pipe is busy, wait for it to become available
        // Nếu pipe bận, đợi nó sẵn sàng
        if (GetLastError() == ERROR_PIPE_BUSY) {
            printf("[CLIENT] Pipe is busy, waiting...\n");
            if (!WaitNamedPipeA(PIPE_NAME, 2000)) {
                PrintError("WaitNamedPipe");
            }
        } else {
            // Server may not have started yet, retry
            // Server có thể chưa khởi động, thử lại
            Sleep(300);
        }
        retries++;
    }

    if (hPipe == INVALID_HANDLE_VALUE) {
        PrintError("Client CreateFile (connect to pipe)");
        return 1;
    }
    printf("[CLIENT] Connected to pipe! (Da ket noi den pipe!)\n\n");

    // -----------------------------------------------------------------------
    // Step 2: Set pipe to message-read mode
    // Bước 2: Đặt pipe ở chế độ đọc tin nhắn
    // -----------------------------------------------------------------------
    DWORD dwMode = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL)) {
        PrintError("SetNamedPipeHandleState");
        CloseHandle(hPipe);
        return 1;
    }

    // -----------------------------------------------------------------------
    // Step 3: Send messages and receive replies
    // Bước 3: Gửi tin nhắn và nhận phản hồi
    // -----------------------------------------------------------------------
    const char* messages[] = {
        "Hello Server!",
        "How are you?",
        "Goodbye! (Tam biet!)"
    };
    int numMessages = sizeof(messages) / sizeof(messages[0]);

    char buffer[512];
    DWORD bytesRead, bytesWritten;

    for (int i = 0; i < numMessages; i++) {
        // --- Send message to server (Gửi tin nhắn đến server) ---
        BOOL writeOk = WriteFile(
            hPipe,
            messages[i],
            (DWORD)strlen(messages[i]),
            &bytesWritten,
            NULL
        );

        if (!writeOk) {
            PrintError("Client WriteFile");
            break;
        }
        printf("[CLIENT] Sent: \"%s\" (%lu bytes)\n", messages[i], bytesWritten);

        // --- Read reply from server (Đọc phản hồi từ server) ---
        ZeroMemory(buffer, sizeof(buffer));
        BOOL readOk = ReadFile(
            hPipe,
            buffer,
            sizeof(buffer) - 1,
            &bytesRead,
            NULL
        );

        if (!readOk || bytesRead == 0) {
            PrintError("Client ReadFile");
            break;
        }
        buffer[bytesRead] = '\0';
        printf("[CLIENT] Received reply: \"%s\" (%lu bytes)\n\n",
               buffer, bytesRead);

        Sleep(300); // Small delay for readability (Delay nhỏ để dễ đọc)
    }

    // -----------------------------------------------------------------------
    // Step 4: Clean up (Dọn dẹp)
    // -----------------------------------------------------------------------
    CloseHandle(hPipe);
    printf("[CLIENT] Pipe client stopped. (Pipe client da dung.)\n");
    return 0;
}

// ============================================================================
// Server-only mode (Chế độ chỉ chạy server)
// ============================================================================
void RunServerOnly() {
    printf("=== Named Pipe Server Mode (Che do Server) ===\n\n");
    PipeServerThread(NULL);
    printf("\n=== Server Finished ===\n");
}

// ============================================================================
// Client-only mode (Chế độ chỉ chạy client)
// ============================================================================
void RunClientOnly() {
    printf("=== Named Pipe Client Mode (Che do Client) ===\n\n");
    PipeClientThread(NULL);
    printf("\n=== Client Finished ===\n");
}

// ============================================================================
// Combined mode - run both in one process using threads
// Chế độ kết hợp - chạy cả hai trong một tiến trình bằng threads
// ============================================================================
void RunCombined() {
    printf("=== Named Pipe Demo (Minh hoa Named Pipe) ===\n");
    printf("Running server and client in the same process using threads.\n");
    printf("Chay server va client trong cung tien trinh bang cac luong.\n\n");
    printf("============================================================\n\n");

    // Create server thread
    // Tạo luồng server
    HANDLE hServerThread = CreateThread(
        NULL, 0, PipeServerThread, NULL, 0, NULL
    );

    // Create client thread (will auto-delay to let server start)
    // Tạo luồng client (sẽ tự delay để server khởi động)
    HANDLE hClientThread = CreateThread(
        NULL, 0, PipeClientThread, NULL, 0, NULL
    );

    if (!hServerThread || !hClientThread) {
        PrintError("CreateThread");
        return;
    }

    // Wait for both threads to finish
    // Chờ cả hai luồng kết thúc
    HANDLE threads[] = { hServerThread, hClientThread };
    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    CloseHandle(hServerThread);
    CloseHandle(hClientThread);

    printf("\n=== Named Pipe Demo Complete (Hoan tat minh hoa Named Pipe) ===\n");
}

// ============================================================================
// Main (Hàm chính)
// ============================================================================
int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "--server") == 0) {
            RunServerOnly();
        } else if (strcmp(argv[1], "--client") == 0) {
            RunClientOnly();
        } else {
            printf("Usage: %s [--server | --client]\n", argv[0]);
            printf("  No arguments: run both server & client (default)\n");
            printf("  --server: run pipe server only\n");
            printf("  --client: run pipe client only\n");
        }
    } else {
        RunCombined();
    }
    return 0;
}
