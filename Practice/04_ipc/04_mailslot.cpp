/*
 * ============================================================================
 * Exercise 04: Mailslot (Hộp thư)
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrate one-to-many messaging using Windows Mailslots.
 *   Minh họa giao tiếp một-đến-nhiều bằng Mailslot của Windows.
 *
 *   A mailslot is a one-directional IPC mechanism. The server creates
 *   a mailslot and reads from it. One or more clients can write to it.
 *   Mailslot là cơ chế IPC một chiều. Server tạo mailslot và đọc từ nó.
 *   Một hoặc nhiều client có thể ghi vào nó.
 *
 *   Key characteristics of mailslots:
 *   Đặc điểm chính của mailslot:
 *   - One-way: clients write, server reads (Một chiều: client ghi, server đọc)
 *   - One-to-many: multiple clients can write (Một-đến-nhiều: nhiều client ghi)
 *   - Messages up to ~424 bytes for broadcast (Tin nhắn tối đa ~424 byte cho broadcast)
 *   - No guaranteed delivery (Không đảm bảo truyền tin)
 *
 * Key APIs (Các API chính):
 *   - CreateMailslot()     : Create mailslot server (Tạo máy chủ mailslot)
 *   - CreateFile()         : Client connects to mailslot (Client kết nối)
 *   - WriteFile()          : Client sends message (Client gửi tin nhắn)
 *   - ReadFile()           : Server reads message (Server đọc tin nhắn)
 *   - GetMailslotInfo()    : Query mailslot status (Truy vấn trạng thái)
 *
 * Compile (Biên dịch):
 *   g++ 04_mailslot.cpp -o 04_mailslot.exe
 *
 * Usage (Cách dùng):
 *   04_mailslot.exe            (demo with threads in one process)
 *   04_mailslot.exe --server   (run as mailslot server)
 *   04_mailslot.exe --client   (run as mailslot client)
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// Mailslot name - must start with \\.\mailslot\
// Tên mailslot - phải bắt đầu bằng \\.\mailslot\
#define MAILSLOT_NAME "\\\\.\\mailslot\\MyMailslot_IPC_Demo"

// ============================================================================
// Error helper
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
// MAILSLOT SERVER (Máy chủ Mailslot)
// Creates a mailslot and reads incoming messages.
// Tạo mailslot và đọc các tin nhắn đến.
// ============================================================================
#define MAILSLOT_NAME "\\\\.\\mailslot\\my_test_mailslot"

DWORD WINAPI MailslotServerThread(LPVOID lpParam) {
    printf("[SERVER] Starting mailslot server...\n");
    printf("[SERVER] Khoi dong may chu mailslot...\n\n");

    // -----------------------------------------------------------------------
    // Step 1: Create the mailslot
    // Bước 1: Tạo mailslot
    // -----------------------------------------------------------------------
    // CreateMailslot creates a mailslot with the given name.
    // CreateMailslot tạo một mailslot với tên được chỉ định.
    printf("[SERVER] Step 1: Creating mailslot...\n");
    printf("[SERVER] Mailslot name: %s\n", MAILSLOT_NAME);

    HANDLE hMailslot = CreateMailslotA(
        MAILSLOT_NAME,  // Mailslot name (Tên mailslot)
        0,              // Max message size, 0 = any size (Kích thước tối đa, 0 = bất kỳ)
        5000,           // Read timeout in ms (Thời gian chờ đọc, ms)
                        // MAILSLOT_WAIT_FOREVER for infinite (MAILSLOT_WAIT_FOREVER cho vô hạn)
        NULL            // Default security (Bảo mật mặc định)
    );

    if (hMailslot == INVALID_HANDLE_VALUE) {
        PrintError("CreateMailslot");
        return 1;
    }
    printf("[SERVER] Mailslot created! Handle = %p\n\n", hMailslot);

    // -----------------------------------------------------------------------
    // Step 2: Query mailslot info
    // Bước 2: Truy vấn thông tin mailslot
    // -----------------------------------------------------------------------
    printf("[SERVER] Step 2: Querying mailslot info...\n");
    printf("[SERVER] Buoc 2: Truy van thong tin mailslot...\n");

    DWORD maxMsgSize, nextMsgSize, msgCount, readTimeout;
    if (GetMailslotInfo(hMailslot, &maxMsgSize, &nextMsgSize,
                        &msgCount, &readTimeout)) {
        printf("[SERVER]   Max message size : %lu bytes\n", maxMsgSize);
        printf("[SERVER]   Next message size: %s\n",
               nextMsgSize == MAILSLOT_NO_MESSAGE ? "No messages" : "Has message");
        printf("[SERVER]   Message count    : %lu\n", msgCount);
        printf("[SERVER]   Read timeout     : %lu ms\n\n", readTimeout);
    }

    // -----------------------------------------------------------------------
    // Step 3: Read messages in a loop
    // Bước 3: Đọc tin nhắn trong vòng lặp
    // -----------------------------------------------------------------------
    printf("[SERVER] Step 3: Waiting for messages...\n");
    printf("[SERVER] Buoc 3: Dang cho tin nhan...\n\n");

    char buffer[512];
    DWORD bytesRead;
    int totalMessages = 0;
    int emptyReads = 0;
    const int MAX_EMPTY_READS = 3; // Exit after 3 timeouts (Thoát sau 3 lần timeout)

    while (emptyReads < MAX_EMPTY_READS) {
        // Check if there's a message waiting
        // Kiểm tra xem có tin nhắn đang chờ không
        if (GetMailslotInfo(hMailslot, NULL, &nextMsgSize, &msgCount, NULL)) {
            if (nextMsgSize == MAILSLOT_NO_MESSAGE) {
                // No message yet, wait a bit
                // Chưa có tin nhắn, chờ một chút
                Sleep(500);
                emptyReads++;
                continue;
            }
        }

        // Read the message
        // Đọc tin nhắn
        ZeroMemory(buffer, sizeof(buffer));
        BOOL readOk = ReadFile(
            hMailslot,          // Mailslot handle
            buffer,             // Buffer for data (Bộ đệm cho dữ liệu)
            sizeof(buffer) - 1, // Max bytes
            &bytesRead,         // Bytes actually read (Số byte thực sự đọc)
            NULL                // Not overlapped
        );

        if (readOk && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            totalMessages++;
            emptyReads = 0; // Reset empty counter (Reset bộ đếm rỗng)

            printf("  [SERVER] Message #%d (%lu bytes): \"%s\"\n",
                   totalMessages, bytesRead, buffer);

            // Check for termination message
            // Kiểm tra tin nhắn kết thúc
            if (strcmp(buffer, "EXIT") == 0) {
                printf("  [SERVER] Received EXIT command.\n");
                break;
            }
        } else {
            emptyReads++;
            if (GetLastError() != ERROR_SEM_TIMEOUT) {
                PrintError("Server ReadFile");
            }
        }
    }

    // -----------------------------------------------------------------------
    // Step 4: Clean up
    // Bước 4: Dọn dẹp
    // -----------------------------------------------------------------------
    printf("\n[SERVER] Total messages received: %d\n", totalMessages);
    printf("[SERVER] Tong so tin nhan da nhan: %d\n", totalMessages);
    CloseHandle(hMailslot);
    printf("[SERVER] Mailslot server stopped. (May chu da dung.)\n");

    return 0;
}

// ============================================================================
// MAILSLOT CLIENT (Client Mailslot)
// Writes messages to the mailslot.
// Ghi tin nhắn vào mailslot.
// ============================================================================
DWORD WINAPI MailslotClientThread(LPVOID lpParam) {
    // Extract client ID from parameter (Lấy ID client từ tham số)
    int clientId = (int)(intptr_t)lpParam;

    // Delay to let server start (Delay để server khởi động)
    Sleep(800 + clientId * 200);

    printf("[CLIENT-%d] Starting mailslot client...\n", clientId);
    printf("[CLIENT-%d] Khoi dong mailslot client...\n\n", clientId);

    // -----------------------------------------------------------------------
    // Step 1: Open the mailslot using CreateFile
    // Bước 1: Mở mailslot bằng CreateFile
    // -----------------------------------------------------------------------
    // To write to a mailslot, the client uses CreateFile.
    // Để ghi vào mailslot, client dùng CreateFile.
    printf("[CLIENT-%d] Connecting to mailslot: %s\n", clientId, MAILSLOT_NAME);

    HANDLE hMailslot = INVALID_HANDLE_VALUE;
    int retries = 0;

    while (retries < 10) {
        hMailslot = CreateFileA(
            MAILSLOT_NAME,      // Mailslot name (Tên mailslot)
            GENERIC_WRITE,      // Write-only (Chỉ ghi - mailslot is one-way)
            FILE_SHARE_READ,    // Share for reading (Chia sẻ đọc)
            NULL,               // Default security
            OPEN_EXISTING,      // Must exist (Phải đã tồn tại)
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (hMailslot != INVALID_HANDLE_VALUE) break;
        Sleep(300);
        retries++;
    }

    if (hMailslot == INVALID_HANDLE_VALUE) {
        PrintError("Client CreateFile (mailslot)");
        return 1;
    }
    printf("[CLIENT-%d] Connected to mailslot!\n\n", clientId);

    // -----------------------------------------------------------------------
    // Step 2: Send messages
    // Bước 2: Gửi tin nhắn
    // -----------------------------------------------------------------------
    char message[256];
    DWORD bytesWritten;

    // Send 3 numbered messages from this client
    // Gửi 3 tin nhắn đánh số từ client này
    for (int i = 1; i <= 3; i++) {
        snprintf(message, sizeof(message),
                 "Hello from Client %d, message %d (PID=%lu)",
                 clientId, i, GetCurrentProcessId());

        BOOL writeOk = WriteFile(
            hMailslot,
            message,
            (DWORD)strlen(message),
            &bytesWritten,
            NULL
        );

        if (!writeOk) {
            PrintError("Client WriteFile");
            break;
        }

        printf("  [CLIENT-%d] Sent (%lu bytes): \"%s\"\n",
               clientId, bytesWritten, message);
        Sleep(400);
    }

    // Send EXIT command (only from client 1, last client to finish)
    // Gửi lệnh EXIT (chỉ từ client 1, client cuối cùng kết thúc)
    if (clientId == 1) {
        Sleep(1500); // Wait for other clients to finish
        const char* exitMsg = "EXIT";
        WriteFile(hMailslot, exitMsg, (DWORD)strlen(exitMsg), &bytesWritten, NULL);
        printf("  [CLIENT-%d] Sent EXIT command.\n", clientId);
    }

    // -----------------------------------------------------------------------
    // Step 3: Clean up (Dọn dẹp)
    // -----------------------------------------------------------------------
    CloseHandle(hMailslot);
    printf("[CLIENT-%d] Client done. (Client hoan tat.)\n\n", clientId);

    return 0;
}

// ============================================================================
// Main - Hàm chính
// ============================================================================
int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--server") == 0) {
        printf("=== Mailslot Server Mode (Che do Server) ===\n\n");
        MailslotServerThread(NULL);

    } else if (argc > 1 && strcmp(argv[1], "--client") == 0) {
        printf("=== Mailslot Client Mode (Che do Client) ===\n\n");
        MailslotClientThread((LPVOID)1);

    } else {
        // Combined mode: 1 server + 2 clients using threads
        // Chế độ kết hợp: 1 server + 2 client bằng threads
        printf("=== Mailslot Demo (Minh hoa Mailslot) ===\n");
        printf("One-to-many messaging: 1 server, 2 clients.\n");
        printf("Giao tiep mot-den-nhieu: 1 server, 2 client.\n");
        printf("=============================================\n\n");

        // Create server thread (Tạo luồng server)
        HANDLE hServer = CreateThread(
            NULL, 0, MailslotServerThread, NULL, 0, NULL
        );

        // Create two client threads to show one-to-many capability
        // Tạo hai luồng client để minh họa khả năng một-đến-nhiều
        HANDLE hClient1 = CreateThread(
            NULL, 0, MailslotClientThread, (LPVOID)1, 0, NULL
        );
        HANDLE hClient2 = CreateThread(
            NULL, 0, MailslotClientThread, (LPVOID)2, 0, NULL
        );

        if (!hServer || !hClient1 || !hClient2) {
            PrintError("CreateThread");
            return 1;
        }

        // Wait for all threads (Chờ tất cả luồng)
        HANDLE threads[] = { hServer, hClient1, hClient2 };
        WaitForMultipleObjects(3, threads, TRUE, 30000); // 30s max

        CloseHandle(hServer);
        CloseHandle(hClient1);
        CloseHandle(hClient2);

        printf("\n=== Mailslot Demo Complete (Hoan tat minh hoa Mailslot) ===\n");
    }

    return 0;
}
