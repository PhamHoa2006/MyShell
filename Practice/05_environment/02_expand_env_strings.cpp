/*
 * ============================================================================
 * Exercise 02: Expand Environment Strings
 *              (Mở rộng Chuỗi Biến Môi Trường)
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrate ExpandEnvironmentStrings to replace %VARIABLE% references
 *   in strings with their actual values.
 *   Minh họa ExpandEnvironmentStrings để thay thế các tham chiếu %VARIABLE%
 *   trong chuỗi bằng giá trị thực của chúng.
 *
 *   This is exactly how cmd.exe processes commands like:
 *     echo %USERNAME% is using %OS%
 *   Đây chính xác là cách cmd.exe xử lý các lệnh như:
 *     echo %USERNAME% is using %OS%
 *
 * Key APIs (Các API chính):
 *   - ExpandEnvironmentStringsA() : Expand %VAR% references in a string
 *     (Mở rộng tham chiếu %VAR% trong chuỗi)
 *   - SetEnvironmentVariableA()   : Set custom variables for expansion
 *     (Đặt biến tùy chỉnh để mở rộng)
 *
 * Compile (Biên dịch):
 *   g++ 02_expand_env_strings.cpp -o 02_expand_env_strings.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Helper: Expand and display a string containing %VARIABLE% references
// Trợ giúp: Mở rộng và hiển thị chuỗi chứa tham chiếu %VARIABLE%
// ============================================================================
void ExpandAndShow(const char* input) {
    printf("  Input  (Dau vao) : \"%s\"\n", input);

    // -----------------------------------------------------------------------
    // Method: Query required size, then expand
    // Phương pháp: Truy vấn kích thước cần thiết, sau đó mở rộng
    // -----------------------------------------------------------------------
    // First call: get the required buffer size
    // Lần gọi đầu: lấy kích thước buffer cần thiết
    // ExpandEnvironmentStrings replaces all %VAR% with their values.
    // ExpandEnvironmentStrings thay thế tất cả %VAR% bằng giá trị của chúng.
    DWORD needed = ExpandEnvironmentStringsA(
        input,  // Source string with %VAR% references (Chuỗi nguồn với %VAR%)
        NULL,   // NULL = just query size (NULL = chỉ truy vấn kích thước)
        0       // Buffer size = 0 (Kích thước buffer = 0)
    );

    if (needed == 0) {
        printf("  Output (Ket qua) : (error expanding / loi khi mo rong)\n\n");
        return;
    }

    // Allocate buffer and expand (Cấp phát buffer và mở rộng)
    char* expanded = new char[needed];
    DWORD result = ExpandEnvironmentStringsA(
        input,      // Source string (Chuỗi nguồn)
        expanded,   // Destination buffer (Buffer đích)
        needed      // Buffer size (Kích thước buffer)
    );

    if (result > 0) {
        printf("  Output (Ket qua) : \"%s\"\n", expanded);
        printf("  Size: %lu chars input -> %lu chars output\n\n",
               (DWORD)strlen(input), result - 1);
    } else {
        printf("  Output (Ket qua) : (expansion failed / mo rong that bai)\n\n");
    }

    delete[] expanded;
}

// ============================================================================
// Demo 1: Expand common system variables
// Minh họa 1: Mở rộng các biến hệ thống phổ biến
// ============================================================================
void DemoSystemVars() {
    printf("=== Demo 1: Expanding System Variables ===\n");
    printf("=== Minh hoa 1: Mo Rong Bien He Thong ===\n\n");

    // Each %VAR% in the string will be replaced with its value.
    // Mỗi %VAR% trong chuỗi sẽ được thay thế bằng giá trị của nó.

    printf("--- Single variable expansion (Mo rong mot bien) ---\n\n");
    ExpandAndShow("%USERNAME%");
    ExpandAndShow("%COMPUTERNAME%");
    ExpandAndShow("%OS%");
    ExpandAndShow("%USERPROFILE%");
    ExpandAndShow("%SYSTEMROOT%");
    ExpandAndShow("%TEMP%");
}

// ============================================================================
// Demo 2: Expand variables embedded in strings
// Minh họa 2: Mở rộng biến nhúng trong chuỗi
// ============================================================================
void DemoEmbeddedVars() {
    printf("=== Demo 2: Variables Embedded in Strings ===\n");
    printf("=== Minh hoa 2: Bien Nhung Trong Chuoi ===\n\n");

    // Variables can appear anywhere in a string
    // Các biến có thể xuất hiện bất kỳ đâu trong chuỗi
    ExpandAndShow("Hello, %USERNAME%! Welcome to %COMPUTERNAME%.");
    ExpandAndShow("Your home is: %USERPROFILE%");
    ExpandAndShow("Windows is at: %SYSTEMROOT%\\System32");
    ExpandAndShow("Temp folder: %TEMP%\\my_app_cache");
    ExpandAndShow("%USERNAME% is running %OS% on %PROCESSOR_ARCHITECTURE%");
}

// ============================================================================
// Demo 3: Build file paths using environment variables
// Minh họa 3: Xây dựng đường dẫn file bằng biến môi trường
// ============================================================================
void DemoPathBuilding() {
    printf("=== Demo 3: Building Paths with Variables ===\n");
    printf("=== Minh hoa 3: Xay Dung Duong Dan Voi Bien ===\n\n");

    printf("Common path patterns (Cac mau duong dan pho bien):\n\n");

    // These are common patterns used in Windows applications
    // Đây là các mẫu thường dùng trong ứng dụng Windows
    ExpandAndShow("%APPDATA%\\MyApplication\\config.ini");
    ExpandAndShow("%LOCALAPPDATA%\\MyApplication\\cache");
    ExpandAndShow("%TEMP%\\myapp_session_%USERNAME%");
    ExpandAndShow("%USERPROFILE%\\Documents\\MyProject");
    ExpandAndShow("%SYSTEMROOT%\\System32\\drivers\\etc\\hosts");
    ExpandAndShow("%ProgramFiles%\\Common Files");
}

// ============================================================================
// Demo 4: Custom variables and expansion
// Minh họa 4: Biến tùy chỉnh và mở rộng
// ============================================================================
void DemoCustomVars() {
    printf("=== Demo 4: Custom Variable Expansion ===\n");
    printf("=== Minh hoa 4: Mo Rong Bien Tuy Chinh ===\n\n");

    // Create custom variables (Tạo biến tùy chỉnh)
    printf("Creating custom variables...\n");
    printf("Tao cac bien tuy chinh...\n\n");

    SetEnvironmentVariableA("MY_APP",     "TinyShell");
    SetEnvironmentVariableA("MY_VERSION", "2.0");
    SetEnvironmentVariableA("MY_DATA_DIR", "D:\\Projects\\data");

    // Now expand strings containing our custom variables
    // Bây giờ mở rộng chuỗi chứa các biến tùy chỉnh
    ExpandAndShow("Application: %MY_APP% v%MY_VERSION%");
    ExpandAndShow("Data directory: %MY_DATA_DIR%\\output");
    ExpandAndShow("%MY_APP% by %USERNAME% on %COMPUTERNAME%");

    // Clean up (Dọn dẹp)
    SetEnvironmentVariableA("MY_APP", NULL);
    SetEnvironmentVariableA("MY_VERSION", NULL);
    SetEnvironmentVariableA("MY_DATA_DIR", NULL);
}

// ============================================================================
// Demo 5: Edge cases and behavior
// Minh họa 5: Trường hợp đặc biệt và hành vi
// ============================================================================
void DemoEdgeCases() {
    printf("=== Demo 5: Edge Cases ===\n");
    printf("=== Minh hoa 5: Truong Hop Dac Biet ===\n\n");

    // Case 1: Variable doesn't exist - %VAR% is left as-is
    // Trường hợp 1: Biến không tồn tại - %VAR% giữ nguyên
    printf("--- Case 1: Non-existent variable (Bien khong ton tai) ---\n\n");
    ExpandAndShow("Value is: %NONEXISTENT_VARIABLE_12345%");
    printf("Note: Undefined variables are kept as-is in the output.\n");
    printf("Ghi chu: Bien chua dinh nghia se giu nguyen trong ket qua.\n\n");

    // Case 2: No variables to expand
    // Trường hợp 2: Không có biến để mở rộng
    printf("--- Case 2: No variables (Khong co bien) ---\n\n");
    ExpandAndShow("This is a plain string with no variables.");

    // Case 3: Adjacent variables
    // Trường hợp 3: Các biến liền kề
    printf("--- Case 3: Adjacent variables (Bien lien ke) ---\n\n");
    ExpandAndShow("%USERNAME%%COMPUTERNAME%");

    // Case 4: Single percent sign (not a variable)
    // Trường hợp 4: Dấu phần trăm đơn (không phải biến)
    printf("--- Case 4: Single percent sign (Dau phan tram don) ---\n\n");
    ExpandAndShow("50% complete");

    // Case 5: Empty variable name
    // Trường hợp 5: Tên biến rỗng
    printf("--- Case 5: Empty variable name (Ten bien rong) ---\n\n");
    ExpandAndShow("Value: %%");
}

// ============================================================================
// Demo 6: Building a simple template engine
// Minh họa 6: Xây dựng hệ thống mẫu đơn giản
// ============================================================================
void DemoTemplateEngine() {
    printf("=== Demo 6: Simple Template Engine ===\n");
    printf("=== Minh hoa 6: He Thong Mau Don Gian ===\n\n");

    // Set up template variables (Thiết lập biến mẫu)
    SetEnvironmentVariableA("PROJECT_NAME", "OS-Practice");
    SetEnvironmentVariableA("STUDENT_NAME", "Nguyen Van A");
    SetEnvironmentVariableA("COURSE",       "Operating Systems");
    SetEnvironmentVariableA("SEMESTER",     "2024-2025");

    printf("Template variables set. Expanding template...\n");
    printf("Cac bien mau da dat. Dang mo rong mau...\n\n");

    // A multi-line template using environment variables
    // Một mẫu nhiều dòng sử dụng biến môi trường
    const char* templateLines[] = {
        "=== Project Report ===",
        "Student: %STUDENT_NAME%",
        "Course : %COURSE%",
        "Semester: %SEMESTER%",
        "Project: %PROJECT_NAME%",
        "System : %OS% on %COMPUTERNAME%",
        "User   : %USERNAME%",
        "Date   : (use GetLocalTime for actual date)",
        "======================"
    };
    int numLines = sizeof(templateLines) / sizeof(templateLines[0]);

    printf("Expanded template output:\n");
    printf("Ket qua mau da mo rong:\n\n");

    for (int i = 0; i < numLines; i++) {
        char expanded[1024];
        ExpandEnvironmentStringsA(templateLines[i], expanded, sizeof(expanded));
        printf("  %s\n", expanded);
    }
    printf("\n");

    // Clean up (Dọn dẹp)
    SetEnvironmentVariableA("PROJECT_NAME", NULL);
    SetEnvironmentVariableA("STUDENT_NAME", NULL);
    SetEnvironmentVariableA("COURSE", NULL);
    SetEnvironmentVariableA("SEMESTER", NULL);
}

// ============================================================================
// Main - Hàm chính
// ============================================================================
int main() {
    printf("============================================================\n");
    printf("  ExpandEnvironmentStrings Demo\n");
    printf("  Minh hoa Mo Rong Chuoi Bien Moi Truong\n");
    printf("============================================================\n\n");

    DemoSystemVars();
    DemoEmbeddedVars();
    DemoPathBuilding();
    DemoCustomVars();
    DemoEdgeCases();
    DemoTemplateEngine();

    printf("============================================================\n");
    printf("  Exercise Complete! (Bai tap hoan tat!)\n");
    printf("============================================================\n");

    return 0;
}
