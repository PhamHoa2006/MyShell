/*
 * ============================================================================
 * Exercise 03: Shared Memory (Bộ nhớ chia sẻ)
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrate inter-process communication using shared memory.
 *   Minh họa giao tiếp giữa các tiến trình bằng bộ nhớ chia sẻ.
 *
 *   Uses CreateFileMapping with INVALID_HANDLE_VALUE (no backing file) to
 *   create a shared memory region. This program can run in two modes:
 *   - Writer: creates shared memory and writes data
 *   - Reader: opens existing shared memory and reads data
 *   Sử dụng CreateFileMapping với INVALID_HANDLE_VALUE (không cần file) để
 *   tạo vùng nhớ chia sẻ. Chương trình có hai chế độ:
 *   - Writer: tạo vùng nhớ chia sẻ và ghi dữ liệu
 *   - Reader: mở vùng nhớ chia sẻ đã tồn tại và đọc dữ liệu
 *
 * Key APIs (Các API chính):
 *   - CreateFileMapping()  : Create a file mapping object (Tạo đối tượng ánh xạ file)
 *   - OpenFileMapping()    : Open existing mapping (Mở mapping đã tồn tại)
 *   - MapViewOfFile()      : Map a view into address space (Ánh xạ vào không gian địa chỉ)
 *   - UnmapViewOfFile()    : Unmap the view (Hủy ánh xạ)
 *   - CreateEvent()        : Create synchronization event (Tạo sự kiện đồng bộ)
 *
 * Compile (Biên dịch):
 *   g++ 03_shared_memory.cpp -o 03_shared_memory.exe
 *
 * Usage (Cách dùng):
 *   03_shared_memory.exe            (demo with threads in one process)
 *   03_shared_memory.exe --writer   (run as writer process)
 *   03_shared_memory.exe --reader   (run as reader process, start after writer)
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// Name for the shared memory object - must be globally unique
// Tên cho đối tượng bộ nhớ chia sẻ - phải là duy nhất toàn cục
#define SHM_NAME    "Local\\MySharedMemory_IPC_Demo"
#define SHM_SIZE    4096   // 4 KB shared region (Vùng chia sẻ 4KB)

// Event names for synchronization between writer and reader
// Tên sự kiện để đồng bộ giữa writer và reader
#define EVT_DATA_READY   "Local\\SHM_DataReady"
#define EVT_DATA_READ    "Local\\SHM_DataRead"

// ============================================================================
// Shared data structure - placed in shared memory
// Cấu trúc dữ liệu chia sẻ - đặt trong bộ nhớ chia sẻ
// ============================================================================
struct SharedData {
    int    messageId;           // Message sequence number (Số thứ tự tin nhắn)
    int    processIdWriter;     // Writer's PID (PID của writer)
    char   message[256];        // Message text (Nội dung tin nhắn)
    bool   done;                // Flag: no more messages (Cờ: hết tin nhắn)
};

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
// WRITER: Creates shared memory and writes data
// WRITER: Tạo bộ nhớ chia sẻ và ghi dữ liệu
// ============================================================================
DWORD WINAPI WriterThread(LPVOID lpParam) {
    printf("[WRITER] Starting writer (PID = %lu)...\n", GetCurrentProcessId());
    printf("[WRITER] Khoi dong writer...\n\n");

    // -----------------------------------------------------------------------
    // Step 1: Create a file mapping object backed by the paging file
    // Bước 1: Tạo đối tượng ánh xạ file được hỗ trợ bởi paging file
    // -----------------------------------------------------------------------
    // INVALID_HANDLE_VALUE means "use system paging file" (no physical file).
    // INVALID_HANDLE_VALUE nghĩa là "dùng paging file của hệ thống" (không cần file vật lý).
    printf("[WRITER] Step 1: Creating file mapping object...\n");
    printf("[WRITER] Buoc 1: Tao doi tuong anh xa file...\n");

    HANDLE hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,   // Use paging file (Dùng paging file)
        NULL,                   // Default security (Bảo mật mặc định)
        PAGE_READWRITE,         // Read/Write access (Quyền đọc/ghi)
        0,                      // Max size high DWORD
        SHM_SIZE,               // Max size low DWORD (Kích thước tối đa)
        SHM_NAME                // Name of mapping object (Tên đối tượng)
    );

    if (hMapFile == NULL) {
        PrintError("CreateFileMapping");
        return 1;
    }
    printf("[WRITER] File mapping created: handle = %p\n", hMapFile);
    printf("[WRITER] Shared memory name: %s\n", SHM_NAME);
    printf("[WRITER] Size: %d bytes\n\n", SHM_SIZE);

    // -----------------------------------------------------------------------
    // Step 2: Map the view into our address space
    // Bước 2: Ánh xạ vào không gian địa chỉ của tiến trình
    // -----------------------------------------------------------------------
    // MapViewOfFile maps a view of the file mapping into the address space.
    // MapViewOfFile ánh xạ một phần của file mapping vào không gian địa chỉ.
    printf("[WRITER] Step 2: Mapping view of file...\n");
    printf("[WRITER] Buoc 2: Anh xa vao bo nho...\n");

    SharedData* pData = (SharedData*)MapViewOfFile(
        hMapFile,               // Handle to mapping object
        FILE_MAP_ALL_ACCESS,    // Read/Write access (Quyền đọc/ghi)
        0,                      // File offset high
        0,                      // File offset low
        SHM_SIZE                // Number of bytes to map (Số byte ánh xạ)
    );

    if (pData == NULL) {
        PrintError("MapViewOfFile");
        CloseHandle(hMapFile);
        return 1;
    }
    printf("[WRITER] Mapped at address: %p\n\n", pData);

    // -----------------------------------------------------------------------
    // Step 3: Create synchronization events
    // Bước 3: Tạo các sự kiện đồng bộ
    // -----------------------------------------------------------------------
    // Events coordinate writer and reader so they don't race.
    // Các sự kiện đồng bộ hóa writer và reader để tránh race condition.
    printf("[WRITER] Step 3: Creating synchronization events...\n");
    printf("[WRITER] Buoc 3: Tao cac su kien dong bo...\n");

    HANDLE hEvtDataReady = CreateEventA(NULL, FALSE, FALSE, EVT_DATA_READY);
    HANDLE hEvtDataRead  = CreateEventA(NULL, FALSE, FALSE, EVT_DATA_READ);

    if (!hEvtDataReady || !hEvtDataRead) {
        PrintError("CreateEvent");
        UnmapViewOfFile(pData);
        CloseHandle(hMapFile);
        return 1;
    }
    printf("[WRITER] Events created.\n\n");

    // -----------------------------------------------------------------------
    // Step 4: Write messages to shared memory
    // Bước 4: Ghi tin nhắn vào bộ nhớ chia sẻ
    // -----------------------------------------------------------------------
    printf("[WRITER] Step 4: Writing messages to shared memory...\n");
    printf("[WRITER] Buoc 4: Ghi tin nhan vao bo nho chia se...\n\n");

    const char* messages[] = {
        "Hello from shared memory!",
        "This is message number 2.",
        "Shared memory is very fast for IPC!",
        "Bo nho chia se rat nhanh cho IPC!",
        "Final message - goodbye!"
    };
    int numMessages = sizeof(messages) / sizeof(messages[0]);

    for (int i = 0; i < numMessages; i++) {
        // Write data to the shared structure
        // Ghi dữ liệu vào cấu trúc chia sẻ
        pData->messageId      = i + 1;
        pData->processIdWriter = (int)GetCurrentProcessId();
        pData->done            = (i == numMessages - 1);
        strncpy(pData->message, messages[i], sizeof(pData->message) - 1);
        pData->message[sizeof(pData->message) - 1] = '\0';

        printf("  [WRITER] Wrote message #%d: \"%s\"%s\n",
               pData->messageId, pData->message,
               pData->done ? " [FINAL]" : "");

        // Signal reader that data is ready
        // Báo hiệu cho reader rằng dữ liệu đã sẵn sàng
        SetEvent(hEvtDataReady);

        // Wait for reader to acknowledge (so we don't overwrite too fast)
        // Chờ reader xác nhận (để không ghi đè quá nhanh)
        WaitForSingleObject(hEvtDataRead, 5000);

        Sleep(200); // Simulated work delay (Delay mô phỏng công việc)
    }

    // -----------------------------------------------------------------------
    // Step 5: Clean up
    // Bước 5: Dọn dẹp
    // -----------------------------------------------------------------------
    printf("\n[WRITER] All messages written. Cleaning up...\n");
    printf("[WRITER] Tat ca tin nhan da ghi. Dang don dep...\n");

    CloseHandle(hEvtDataReady);
    CloseHandle(hEvtDataRead);

    // UnmapViewOfFile unmaps the view from our address space.
    // UnmapViewOfFile hủy ánh xạ khỏi không gian địa chỉ.
    UnmapViewOfFile(pData);
    CloseHandle(hMapFile);

    printf("[WRITER] Writer done. (Writer hoan tat.)\n");
    return 0;
}

// ============================================================================
// READER: Opens existing shared memory and reads data
// READER: Mở bộ nhớ chia sẻ đã tồn tại và đọc dữ liệu
// ============================================================================
DWORD WINAPI ReaderThread(LPVOID lpParam) {
    // Small delay to let writer create shared memory first
    // Delay nhỏ để writer tạo bộ nhớ chia sẻ trước
    Sleep(300);

    printf("[READER] Starting reader (PID = %lu)...\n", GetCurrentProcessId());
    printf("[READER] Khoi dong reader...\n\n");

    // -----------------------------------------------------------------------
    // Step 1: Open the existing file mapping
    // Bước 1: Mở file mapping đã tồn tại
    // -----------------------------------------------------------------------
    // OpenFileMapping opens an already-created named file mapping.
    // OpenFileMapping mở một file mapping có tên đã được tạo sẵn.
    printf("[READER] Step 1: Opening existing file mapping...\n");

    HANDLE hMapFile = NULL;
    int retries = 0;
    while (retries < 20) {
        hMapFile = OpenFileMappingA(
            FILE_MAP_ALL_ACCESS,    // Read/Write access
            FALSE,                  // Don't inherit handle
            SHM_NAME               // Name of mapping object
        );
        if (hMapFile != NULL) break;
        Sleep(200);
        retries++;
    }

    if (hMapFile == NULL) {
        PrintError("OpenFileMapping");
        return 1;
    }
    printf("[READER] File mapping opened: handle = %p\n\n", hMapFile);

    // -----------------------------------------------------------------------
    // Step 2: Map the view
    // Bước 2: Ánh xạ vào bộ nhớ
    // -----------------------------------------------------------------------
    SharedData* pData = (SharedData*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0, 0,
        SHM_SIZE
    );

    if (pData == NULL) {
        PrintError("Reader MapViewOfFile");
        CloseHandle(hMapFile);
        return 1;
    }
    printf("[READER] Mapped at address: %p\n\n", pData);

    // -----------------------------------------------------------------------
    // Step 3: Open synchronization events
    // Bước 3: Mở các sự kiện đồng bộ
    // -----------------------------------------------------------------------
    HANDLE hEvtDataReady = OpenEventA(EVENT_ALL_ACCESS, FALSE, EVT_DATA_READY);
    HANDLE hEvtDataRead  = OpenEventA(EVENT_ALL_ACCESS, FALSE, EVT_DATA_READ);

    if (!hEvtDataReady || !hEvtDataRead) {
        PrintError("OpenEvent");
        UnmapViewOfFile(pData);
        CloseHandle(hMapFile);
        return 1;
    }

    // -----------------------------------------------------------------------
    // Step 4: Read messages from shared memory
    // Bước 4: Đọc tin nhắn từ bộ nhớ chia sẻ
    // -----------------------------------------------------------------------
    printf("[READER] Waiting for messages from writer...\n");
    printf("[READER] Dang cho tin nhan tu writer...\n\n");

    int totalMessages = 0;
    bool keepReading = true;

    while (keepReading) {
        // Wait for writer to signal data is ready (timeout 10s)
        // Chờ writer báo hiệu dữ liệu sẵn sàng (timeout 10 giây)
        DWORD waitResult = WaitForSingleObject(hEvtDataReady, 10000);

        if (waitResult == WAIT_TIMEOUT) {
            printf("[READER] Timeout waiting for data.\n");
            break;
        }

        // Read the shared data
        // Đọc dữ liệu chia sẻ
        totalMessages++;
        printf("  [READER] Message #%d from PID %d: \"%s\"%s\n",
               pData->messageId,
               pData->processIdWriter,
               pData->message,
               pData->done ? " [FINAL]" : "");

        if (pData->done) {
            keepReading = false;
        }

        // Signal writer that we've read the data
        // Báo hiệu cho writer rằng chúng ta đã đọc dữ liệu
        SetEvent(hEvtDataRead);
    }

    // -----------------------------------------------------------------------
    // Step 5: Clean up (Dọn dẹp)
    // -----------------------------------------------------------------------
    printf("\n[READER] Total messages received: %d\n", totalMessages);
    printf("[READER] Tong so tin nhan da nhan: %d\n", totalMessages);

    CloseHandle(hEvtDataReady);
    CloseHandle(hEvtDataRead);
    UnmapViewOfFile(pData);
    CloseHandle(hMapFile);

    printf("[READER] Reader done. (Reader hoan tat.)\n");
    return 0;
}

// ============================================================================
// Main - Hàm chính
// ============================================================================
int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--writer") == 0) {
        // Writer-only mode (Chế độ chỉ chạy writer)
        printf("=== Shared Memory Writer Mode ===\n\n");
        WriterThread(NULL);

        // Keep alive so reader can finish
        printf("\nPress Enter to exit... (Nhan Enter de thoat...)\n");
        getchar();

    } else if (argc > 1 && strcmp(argv[1], "--reader") == 0) {
        // Reader-only mode (Chế độ chỉ chạy reader)
        printf("=== Shared Memory Reader Mode ===\n\n");
        ReaderThread(NULL);

    } else {
        // Combined mode using threads (Chế độ kết hợp dùng threads)
        printf("=== Shared Memory Demo (Minh hoa Bo Nho Chia Se) ===\n");
        printf("Running writer and reader in the same process using threads.\n");
        printf("Chay writer va reader trong cung tien trinh bang cac luong.\n");
        printf("==========================================================\n\n");

        // Create writer and reader threads
        // Tạo luồng writer và reader
        HANDLE hWriter = CreateThread(NULL, 0, WriterThread, NULL, 0, NULL);
        HANDLE hReader = CreateThread(NULL, 0, ReaderThread, NULL, 0, NULL);

        if (!hWriter || !hReader) {
            PrintError("CreateThread");
            return 1;
        }

        // Wait for both to finish (Chờ cả hai kết thúc)
        HANDLE threads[] = { hWriter, hReader };
        WaitForMultipleObjects(2, threads, TRUE, INFINITE);

        CloseHandle(hWriter);
        CloseHandle(hReader);

        printf("\n=== Shared Memory Demo Complete ===\n");
        printf("=== Hoan tat minh hoa Bo Nho Chia Se ===\n");
    }

    return 0;
}
