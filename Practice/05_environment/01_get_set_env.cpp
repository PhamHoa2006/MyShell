/*
 * ============================================================================
 * Exercise 01: Get & Set Environment Variables
 *              (Đọc & Ghi Biến Môi Trường)
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrate reading and writing environment variables using Windows API.
 *   Minh họa đọc và ghi biến môi trường bằng API Windows.
 *
 *   Environment variables are key-value pairs inherited by child processes.
 *   They store configuration like PATH, USERNAME, TEMP, etc.
 *   Biến môi trường là cặp khóa-giá trị được kế thừa bởi tiến trình con.
 *   Chúng lưu cấu hình như PATH, USERNAME, TEMP, v.v.
 *
 * Key APIs (Các API chính):
 *   - GetEnvironmentVariableA() : Read an env var (Đọc biến môi trường)
 *   - SetEnvironmentVariableA() : Set/create/delete env var (Đặt/tạo/xóa biến)
 *
 * Compile (Biên dịch):
 *   g++ 01_get_set_env.cpp -o 01_get_set_env.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Helper: Read and display an environment variable
// Trợ giúp: Đọc và hiển thị một biến môi trường
// ============================================================================
void ReadEnvVar(const char* varName) {
    // First call: get required buffer size
    // Lần gọi đầu: lấy kích thước buffer cần thiết
    // If the variable doesn't exist, GetEnvironmentVariable returns 0.
    // Nếu biến không tồn tại, GetEnvironmentVariable trả về 0.
    DWORD needed = GetEnvironmentVariableA(varName, NULL, 0);

    if (needed == 0) {
        DWORD err = GetLastError();
        if (err == ERROR_ENVVAR_NOT_FOUND) {
            printf("  %s = (not set / khong ton tai)\n", varName);
        } else {
            printf("  %s = (error %lu)\n", varName, err);
        }
        return;
    }

    // Allocate buffer and read the value
    // Cấp phát buffer và đọc giá trị
    char* value = new char[needed];
    DWORD result = GetEnvironmentVariableA(
        varName,    // Variable name (Tên biến)
        value,      // Buffer to receive value (Bộ đệm nhận giá trị)
        needed      // Buffer size (Kích thước buffer)
    );

    if (result > 0) {
        // Truncate long values for display (Cắt ngắn giá trị dài để hiển thị)
        if (strlen(value) > 100) {
            char truncated[110];
            strncpy(truncated, value, 100);
            truncated[100] = '\0';
            printf("  %s = %s... (%lu chars total)\n", varName, truncated, result);
        } else {
            printf("  %s = %s\n", varName, value);
        }
    }

    delete[] value;
}

// ============================================================================
// Demo 1: Read common environment variables
// Minh họa 1: Đọc các biến môi trường phổ biến
// ============================================================================
void DemoReadEnvVars() {
    printf("=== Demo 1: Reading Environment Variables ===\n");
    printf("=== Minh hoa 1: Doc Bien Moi Truong ===\n\n");

    // List of common Windows environment variables
    // Danh sách các biến môi trường Windows phổ biến
    const char* commonVars[] = {
        "USERNAME",         // Current user (Người dùng hiện tại)
        "COMPUTERNAME",     // Computer name (Tên máy tính)
        "OS",               // Operating system (Hệ điều hành)
        "PROCESSOR_ARCHITECTURE", // CPU architecture (Kiến trúc CPU)
        "NUMBER_OF_PROCESSORS",   // CPU count (Số CPU)
        "SYSTEMROOT",       // Windows directory (Thư mục Windows)
        "USERPROFILE",      // User home directory (Thư mục người dùng)
        "TEMP",             // Temp directory (Thư mục tạm)
        "HOMEPATH",         // Home path (Đường dẫn home)
        "APPDATA",          // Application data (Dữ liệu ứng dụng)
        "PATH",             // System path (Đường dẫn hệ thống)
    };
    int numVars = sizeof(commonVars) / sizeof(commonVars[0]);

    printf("Common Windows environment variables:\n");
    printf("Cac bien moi truong Windows pho bien:\n\n");

    for (int i = 0; i < numVars; i++) {
        ReadEnvVar(commonVars[i]);
    }
    printf("\n");
}

// ============================================================================
// Demo 2: Read with dynamic buffer sizing
// Minh họa 2: Đọc với kích thước buffer động
// ============================================================================
void DemoBufferSizing() {
    printf("=== Demo 2: Dynamic Buffer Sizing ===\n");
    printf("=== Minh hoa 2: Kich Thuoc Buffer Dong ===\n\n");

    const char* varName = "PATH";

    // Method 1: Try with a small buffer first, then resize
    // Phương pháp 1: Thử với buffer nhỏ trước, sau đó thay đổi kích thước
    printf("Method 1: Try small buffer, then resize if needed.\n");
    printf("Phuong phap 1: Thu buffer nho, roi thay doi neu can.\n\n");

    char smallBuf[64];
    DWORD result = GetEnvironmentVariableA(varName, smallBuf, sizeof(smallBuf));

    if (result == 0) {
        printf("  Variable not found.\n");
    } else if (result >= sizeof(smallBuf)) {
        // Buffer too small! 'result' tells us the size needed (including null).
        // Buffer quá nhỏ! 'result' cho biết kích thước cần thiết (bao gồm null).
        printf("  Buffer too small! Need %lu chars, have %zu.\n",
               result, sizeof(smallBuf));
        printf("  Buffer qua nho! Can %lu ky tu, co %zu.\n\n",
               result, sizeof(smallBuf));

        // Allocate exact size and retry
        // Cấp phát đúng kích thước và thử lại
        char* bigBuf = new char[result];
        DWORD result2 = GetEnvironmentVariableA(varName, bigBuf, result);
        if (result2 > 0) {
            printf("  Retry succeeded! %s has %lu characters.\n",
                   varName, result2);
        }
        delete[] bigBuf;
    } else {
        printf("  Got value in first try: %s\n", smallBuf);
    }

    // Method 2: Query size first (pass NULL buffer, size 0)
    // Phương pháp 2: Truy vấn kích thước trước (truyền buffer NULL, kích thước 0)
    printf("\nMethod 2: Query size first, then allocate.\n");
    printf("Phuong phap 2: Truy van kich thuoc truoc, roi cap phat.\n\n");

    DWORD needed = GetEnvironmentVariableA(varName, NULL, 0);
    printf("  %s requires %lu characters (including null terminator).\n",
           varName, needed);
    printf("  %s can %lu ky tu (bao gom ky tu null cuoi).\n\n",
           varName, needed);
}

// ============================================================================
// Demo 3: Create, modify, and delete environment variables
// Minh họa 3: Tạo, thay đổi, và xóa biến môi trường
// ============================================================================
void DemoSetEnvVars() {
    printf("=== Demo 3: Create, Modify, Delete Variables ===\n");
    printf("=== Minh hoa 3: Tao, Thay Doi, Xoa Bien ===\n\n");

    // -----------------------------------------------------------------------
    // Step 1: Create a new variable
    // Bước 1: Tạo biến mới
    // -----------------------------------------------------------------------
    printf("Step 1: Creating MY_CUSTOM_VAR...\n");
    printf("Buoc 1: Tao MY_CUSTOM_VAR...\n");

    // SetEnvironmentVariable creates or modifies an environment variable.
    // SetEnvironmentVariable tạo mới hoặc thay đổi biến môi trường.
    // This only affects the current process and its children.
    // Điều này chỉ ảnh hưởng đến tiến trình hiện tại và các tiến trình con.
    BOOL ok = SetEnvironmentVariableA("MY_CUSTOM_VAR", "Hello World 123");
    if (ok) {
        printf("  Created successfully! (Tao thanh cong!)\n");
        ReadEnvVar("MY_CUSTOM_VAR");
    } else {
        printf("  Failed! Error: %lu\n", GetLastError());
    }

    // -----------------------------------------------------------------------
    // Step 2: Modify the variable
    // Bước 2: Thay đổi biến
    // -----------------------------------------------------------------------
    printf("\nStep 2: Modifying MY_CUSTOM_VAR...\n");
    printf("Buoc 2: Thay doi MY_CUSTOM_VAR...\n");

    SetEnvironmentVariableA("MY_CUSTOM_VAR", "Updated Value - Gia Tri Moi");
    ReadEnvVar("MY_CUSTOM_VAR");

    // -----------------------------------------------------------------------
    // Step 3: Create multiple variables
    // Bước 3: Tạo nhiều biến
    // -----------------------------------------------------------------------
    printf("\nStep 3: Creating multiple variables...\n");
    printf("Buoc 3: Tao nhieu bien...\n");

    SetEnvironmentVariableA("MY_APP_NAME",    "TinyShell");
    SetEnvironmentVariableA("MY_APP_VERSION", "1.0.0");
    SetEnvironmentVariableA("MY_APP_AUTHOR",  "Student");

    ReadEnvVar("MY_APP_NAME");
    ReadEnvVar("MY_APP_VERSION");
    ReadEnvVar("MY_APP_AUTHOR");

    // -----------------------------------------------------------------------
    // Step 4: Delete a variable
    // Bước 4: Xóa biến
    // -----------------------------------------------------------------------
    // Pass NULL as value to delete the variable.
    // Truyền NULL làm giá trị để xóa biến.
    printf("\nStep 4: Deleting MY_CUSTOM_VAR...\n");
    printf("Buoc 4: Xoa MY_CUSTOM_VAR...\n");

    SetEnvironmentVariableA("MY_CUSTOM_VAR", NULL);
    ReadEnvVar("MY_CUSTOM_VAR");

    // -----------------------------------------------------------------------
    // Step 5: Verify remaining variables
    // Bước 5: Xác nhận các biến còn lại
    // -----------------------------------------------------------------------
    printf("\nStep 5: Verify remaining custom variables...\n");
    printf("Buoc 5: Xac nhan cac bien tuy chinh con lai...\n");
    ReadEnvVar("MY_APP_NAME");
    ReadEnvVar("MY_APP_VERSION");
    ReadEnvVar("MY_APP_AUTHOR");

    // Clean up (Dọn dẹp)
    SetEnvironmentVariableA("MY_APP_NAME", NULL);
    SetEnvironmentVariableA("MY_APP_VERSION", NULL);
    SetEnvironmentVariableA("MY_APP_AUTHOR", NULL);
    printf("\n  (All custom variables cleaned up / Tat ca bien tuy chinh da xoa)\n\n");
}

// ============================================================================
// Demo 4: Environment variable scope demonstration
// Minh họa 4: Phạm vi (scope) của biến môi trường
// ============================================================================
void DemoScope() {
    printf("=== Demo 4: Variable Scope (Process-Level) ===\n");
    printf("=== Minh hoa 4: Pham Vi Bien (Cap Tien Trinh) ===\n\n");

    printf("IMPORTANT: SetEnvironmentVariable only changes variables\n");
    printf("for the CURRENT PROCESS and its future child processes.\n");
    printf("It does NOT change system or user environment variables.\n\n");

    printf("QUAN TRONG: SetEnvironmentVariable chi thay doi bien\n");
    printf("cho TIEN TRINH HIEN TAI va cac tien trinh con tuong lai.\n");
    printf("No KHONG thay doi bien moi truong he thong hoac nguoi dung.\n\n");

    // Demonstrate: reading a non-existent variable
    // Minh họa: đọc biến không tồn tại
    printf("Reading non-existent variable DOES_NOT_EXIST:\n");
    ReadEnvVar("DOES_NOT_EXIST");

    // Show that changes are process-local
    // Hiển thị rằng thay đổi chỉ ở cấp tiến trình
    printf("\nCreating SCOPE_TEST in this process:\n");
    SetEnvironmentVariableA("SCOPE_TEST", "I exist only in this process!");
    ReadEnvVar("SCOPE_TEST");

    printf("\nAfter this program exits, SCOPE_TEST will NOT exist\n");
    printf("in the parent process (e.g., cmd.exe or PowerShell).\n");
    printf("Sau khi chuong trinh nay thoat, SCOPE_TEST se KHONG ton tai\n");
    printf("trong tien trinh cha (vi du: cmd.exe hoac PowerShell).\n\n");

    // Clean up (Dọn dẹp)
    SetEnvironmentVariableA("SCOPE_TEST", NULL);
}

// ============================================================================
// Main - Hàm chính
// ============================================================================
int main() {
    printf("============================================================\n");
    printf("  Environment Variables: Get & Set\n");
    printf("  Bien Moi Truong: Doc & Ghi\n");
    printf("  PID = %lu\n", GetCurrentProcessId());
    printf("============================================================\n\n");

    DemoReadEnvVars();
    DemoBufferSizing();
    DemoSetEnvVars();
    DemoScope();

    printf("============================================================\n");
    printf("  Exercise Complete! (Bai tap hoan tat!)\n");
    printf("============================================================\n");

    return 0;
}
