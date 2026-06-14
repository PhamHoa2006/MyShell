/*
 * ============================================================================
 * Exercise 05: Signal Handling (Xử lý tín hiệu)
 * ============================================================================
 * Description (Mô tả):
 *   Demonstrate signal handling using both C standard signal() and
 *   Windows-specific SetConsoleCtrlHandler().
 *   Minh họa xử lý tín hiệu bằng cả signal() tiêu chuẩn C và
 *   SetConsoleCtrlHandler() của Windows.
 *
 *   Signals allow processes to respond to external events like:
 *   Tín hiệu cho phép tiến trình phản hồi các sự kiện bên ngoài như:
 *   - Ctrl+C (SIGINT / CTRL_C_EVENT): User interrupts program
 *   - Ctrl+Break (CTRL_BREAK_EVENT): User sends break signal
 *   - Console close (CTRL_CLOSE_EVENT): Console window is being closed
 *   - SIGTERM: Termination request
 *   - SIGABRT: Abnormal termination
 *   - SIGFPE: Floating-point exception
 *   - SIGSEGV: Segmentation fault
 *
 * Key APIs (Các API chính):
 *   - signal()                : C standard signal handler (Xử lý tín hiệu chuẩn C)
 *   - raise()                 : Send signal to self (Gửi tín hiệu cho chính mình)
 *   - SetConsoleCtrlHandler() : Windows console event handler (Xử lý sự kiện console)
 *
 * Compile (Biên dịch):
 *   g++ 05_signal_handling.cpp -o 05_signal_handling.exe
 *
 * Usage (Cách dùng):
 *   05_signal_handling.exe
 *   Then press Ctrl+C to test signal handling!
 *   Sau đó nhấn Ctrl+C để kiểm tra xử lý tín hiệu!
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

// ============================================================================
// Global flags for signal tracking
// Các cờ toàn cục để theo dõi tín hiệu
// ============================================================================
volatile bool g_running        = true;  // Main loop flag (Cờ vòng lặp chính)
volatile int  g_ctrlCCount     = 0;     // Count of Ctrl+C presses (Số lần nhấn Ctrl+C)
volatile int  g_signalReceived = 0;     // Last signal received (Tín hiệu cuối nhận được)

// ============================================================================
// Part 1: C Standard signal() handler
// Phần 1: Xử lý tín hiệu bằng signal() chuẩn C
// ============================================================================

/*
 * Signal handler function for C standard signals.
 * Hàm xử lý tín hiệu cho các tín hiệu chuẩn C.
 *
 * NOTE: Signal handlers should be as simple as possible.
 * Only set flags - don't call printf or complex functions in production!
 * (Here we use printf for educational purposes.)
 * CHÚ Ý: Hàm xử lý tín hiệu nên đơn giản nhất có thể.
 * Chỉ nên đặt cờ - không nên gọi printf hay hàm phức tạp trong thực tế!
 */
void CSignalHandler(int signum) {
    g_signalReceived = signum;

    switch (signum) {
    case SIGINT:
        // SIGINT: Interrupt signal, usually from Ctrl+C
        // SIGINT: Tín hiệu ngắt, thường từ Ctrl+C
        printf("\n  [C-SIGNAL] Received SIGINT (signal %d) - Interrupt\n", signum);
        printf("  [C-SIGNAL] Nhan duoc SIGINT - Ngat chuong trinh\n");
        g_ctrlCCount++;
        if (g_ctrlCCount >= 3) {
            printf("  [C-SIGNAL] 3 interrupts received, will exit.\n");
            g_running = false;
        } else {
            printf("  [C-SIGNAL] Press Ctrl+C %d more time(s) to exit.\n",
                   3 - g_ctrlCCount);
            // Re-register handler (on Windows, signal() is reset after each call)
            // Đăng ký lại handler (trên Windows, signal() bị reset sau mỗi lần gọi)
            signal(SIGINT, CSignalHandler);
        }
        break;

    case SIGTERM:
        // SIGTERM: Termination request
        // SIGTERM: Yêu cầu kết thúc
        printf("\n  [C-SIGNAL] Received SIGTERM (signal %d) - Termination request\n", signum);
        printf("  [C-SIGNAL] Nhan duoc SIGTERM - Yeu cau ket thuc\n");
        g_running = false;
        break;

    case SIGABRT:
        // SIGABRT: Abnormal termination (e.g., from abort())
        // SIGABRT: Kết thúc bất thường (ví dụ từ abort())
        printf("\n  [C-SIGNAL] Received SIGABRT (signal %d) - Abnormal termination\n", signum);
        printf("  [C-SIGNAL] Nhan duoc SIGABRT - Ket thuc bat thuong\n");
        break;

    case SIGFPE:
        // SIGFPE: Floating-point exception
        // SIGFPE: Ngoại lệ dấu phẩy động
        printf("\n  [C-SIGNAL] Received SIGFPE (signal %d) - Floating-point error\n", signum);
        printf("  [C-SIGNAL] Nhan duoc SIGFPE - Loi dau phay dong\n");
        break;

    case SIGSEGV:
        // SIGSEGV: Segmentation violation
        // SIGSEGV: Vi phạm phân đoạn bộ nhớ
        printf("\n  [C-SIGNAL] Received SIGSEGV (signal %d) - Segmentation fault\n", signum);
        printf("  [C-SIGNAL] Nhan duoc SIGSEGV - Loi truy cap bo nho\n");
        break;

    default:
        printf("\n  [C-SIGNAL] Received unknown signal: %d\n", signum);
        break;
    }
}

// ============================================================================
// Part 2: Windows Console Ctrl Handler
// Phần 2: Xử lý sự kiện Console của Windows
// ============================================================================

/*
 * Console control handler for Windows-specific events.
 * Hàm xử lý sự kiện điều khiển console cho Windows.
 *
 * SetConsoleCtrlHandler registers this function to handle console events.
 * SetConsoleCtrlHandler đăng ký hàm này để xử lý các sự kiện console.
 *
 * Return TRUE to indicate the event was handled (don't pass to next handler).
 * Return FALSE to pass to the next handler in the chain.
 * Trả về TRUE để chỉ ra sự kiện đã được xử lý (không chuyển tiếp).
 * Trả về FALSE để chuyển cho handler tiếp theo trong chuỗi.
 */
BOOL WINAPI ConsoleCtrlHandler(DWORD ctrlType) {
    switch (ctrlType) {
    case CTRL_C_EVENT:
        // Ctrl+C pressed (Nhấn Ctrl+C)
        printf("\n  [WIN-CTRL] CTRL_C_EVENT received!\n");
        printf("  [WIN-CTRL] Nhan duoc su kien CTRL+C!\n");
        g_ctrlCCount++;
        if (g_ctrlCCount >= 3) {
            printf("  [WIN-CTRL] 3 Ctrl+C received. Shutting down gracefully...\n");
            printf("  [WIN-CTRL] Da nhan 3 lan Ctrl+C. Dang tat an toan...\n");
            g_running = false;
        } else {
            printf("  [WIN-CTRL] Press Ctrl+C %d more time(s) to exit.\n",
                   3 - g_ctrlCCount);
        }
        return TRUE;  // Handled (Đã xử lý)

    case CTRL_BREAK_EVENT:
        // Ctrl+Break pressed (Nhấn Ctrl+Break)
        printf("\n  [WIN-CTRL] CTRL_BREAK_EVENT received!\n");
        printf("  [WIN-CTRL] Nhan duoc su kien CTRL+BREAK!\n");
        printf("  [WIN-CTRL] Break signal - forcing exit.\n");
        g_running = false;
        return TRUE;

    case CTRL_CLOSE_EVENT:
        // Console window is closing (Cửa sổ console đang đóng)
        printf("\n  [WIN-CTRL] CTRL_CLOSE_EVENT - Console closing!\n");
        printf("  [WIN-CTRL] Cua so console dang dong!\n");
        printf("  [WIN-CTRL] Performing cleanup...\n");
        // You have ~5 seconds to clean up before force termination
        // Bạn có ~5 giây để dọn dẹp trước khi bị buộc tắt
        Sleep(1000);
        return TRUE;

    case CTRL_LOGOFF_EVENT:
        // User is logging off (Người dùng đang đăng xuất)
        printf("\n  [WIN-CTRL] CTRL_LOGOFF_EVENT - User logging off!\n");
        printf("  [WIN-CTRL] Nguoi dung dang dang xuat!\n");
        return FALSE; // Let system handle it (Để hệ thống xử lý)

    case CTRL_SHUTDOWN_EVENT:
        // System is shutting down (Hệ thống đang tắt)
        printf("\n  [WIN-CTRL] CTRL_SHUTDOWN_EVENT - System shutting down!\n");
        printf("  [WIN-CTRL] He thong dang tat!\n");
        return FALSE;

    default:
        printf("\n  [WIN-CTRL] Unknown control type: %lu\n", ctrlType);
        return FALSE;
    }
}

// ============================================================================
// Demo 1: C Standard Signals
// Minh họa 1: Tín hiệu chuẩn C
// ============================================================================
void DemoCSignals() {
    printf("=== Part 1: C Standard signal() Demo ===\n");
    printf("=== Phan 1: Minh hoa signal() chuan C ===\n\n");

    // Register signal handlers
    // Đăng ký các hàm xử lý tín hiệu
    printf("Registering signal handlers...\n");
    printf("Dang ky cac ham xu ly tin hieu...\n\n");

    signal(SIGINT,  CSignalHandler);  // Ctrl+C
    signal(SIGTERM, CSignalHandler);  // Termination
    signal(SIGABRT, CSignalHandler);  // Abort
    signal(SIGFPE,  CSignalHandler);  // Floating-point
    signal(SIGSEGV, CSignalHandler);  // Segfault

    printf("Registered handlers for: SIGINT, SIGTERM, SIGABRT, SIGFPE, SIGSEGV\n\n");

    // Demonstrate raise() - send signals to ourselves
    // Minh họa raise() - gửi tín hiệu cho chính mình
    printf("--- Sending signals to self with raise() ---\n");
    printf("--- Gui tin hieu cho chinh minh bang raise() ---\n\n");

    printf("Calling raise(SIGTERM)...\n");
    raise(SIGTERM);
    printf("Returned from SIGTERM handler.\n\n");

    // Reset running flag for demo purposes (Đặt lại cờ cho mục đích minh họa)
    g_running = true;

    // Re-register because Windows resets handlers
    // Đăng ký lại vì Windows reset handler sau mỗi lần gọi
    signal(SIGTERM, CSignalHandler);

    printf("--- C signal() demo complete ---\n\n");
}

// ============================================================================
// Demo 2: Windows Console Ctrl Handler
// Minh họa 2: Xử lý Console Windows
// ============================================================================
void DemoWindowsCtrlHandler() {
    printf("=== Part 2: Windows SetConsoleCtrlHandler() Demo ===\n");
    printf("=== Phan 2: Minh hoa SetConsoleCtrlHandler() ===\n\n");

    // -----------------------------------------------------------------------
    // Register the console control handler
    // Đăng ký hàm xử lý sự kiện điều khiển console
    // -----------------------------------------------------------------------
    // SetConsoleCtrlHandler adds our handler to the list of handlers.
    // SetConsoleCtrlHandler thêm handler của chúng ta vào danh sách.
    // The second parameter TRUE = add handler, FALSE = remove handler.
    // Tham số thứ hai TRUE = thêm handler, FALSE = xóa handler.
    printf("Registering Windows console control handler...\n");
    printf("Dang ky ham xu ly su kien console Windows...\n\n");

    if (!SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE)) {
        DWORD err = GetLastError();
        fprintf(stderr, "[ERROR] SetConsoleCtrlHandler failed: %lu\n", err);
        return;
    }
    printf("Console control handler registered successfully!\n\n");

    // -----------------------------------------------------------------------
    // Print info about all event types
    // In thông tin về các loại sự kiện
    // -----------------------------------------------------------------------
    printf("Supported console events (Cac su kien console ho tro):\n");
    printf("  CTRL_C_EVENT      (0) - Ctrl+C pressed\n");
    printf("  CTRL_BREAK_EVENT  (1) - Ctrl+Break pressed\n");
    printf("  CTRL_CLOSE_EVENT  (2) - Console window closing\n");
    printf("  CTRL_LOGOFF_EVENT (5) - User logging off\n");
    printf("  CTRL_SHUTDOWN_EVENT(6)- System shutting down\n\n");

    // -----------------------------------------------------------------------
    // Main loop - wait for signals
    // Vòng lặp chính - chờ tín hiệu
    // -----------------------------------------------------------------------
    printf("============================================\n");
    printf("Program is running. Try these:\n");
    printf("Chuong trinh dang chay. Hay thu:\n");
    printf("  - Press Ctrl+C     (3 times to exit)\n");
    printf("  - Nhan Ctrl+C      (3 lan de thoat)\n");
    printf("  - Press Ctrl+Break (immediate exit)\n");
    printf("  - Nhan Ctrl+Break  (thoat ngay)\n");
    printf("============================================\n\n");

    g_ctrlCCount = 0; // Reset counter (Đặt lại bộ đếm)
    g_running = true;

    int tick = 0;
    while (g_running) {
        Sleep(1000);
        tick++;
        printf("[MAIN] Running... tick %d (Ctrl+C count: %d)\n",
               tick, g_ctrlCCount);

        // Auto-exit after 30 seconds if no user input
        // Tự thoát sau 30 giây nếu không có thao tác
        if (tick >= 30) {
            printf("[MAIN] Auto-exit after 30 seconds.\n");
            break;
        }
    }

    // -----------------------------------------------------------------------
    // Clean up: remove the handler
    // Dọn dẹp: gỡ handler
    // -----------------------------------------------------------------------
    printf("\n[MAIN] Removing console control handler...\n");
    printf("[MAIN] Dang go ham xu ly console...\n");
    SetConsoleCtrlHandler(ConsoleCtrlHandler, FALSE); // FALSE = remove

    printf("[MAIN] Handler removed.\n\n");
}

// ============================================================================
// Demo 3: Show signal numbers and their meanings
// Minh họa 3: Hiển thị số tín hiệu và ý nghĩa
// ============================================================================
void ShowSignalInfo() {
    printf("=== Part 3: Signal Reference Table ===\n");
    printf("=== Phan 3: Bang tham khao tin hieu ===\n\n");

    printf("+--------+----------+--------------------------------------------+\n");
    printf("| Signal | Value    | Description (Mo ta)                        |\n");
    printf("+--------+----------+--------------------------------------------+\n");
    printf("| SIGINT | %-8d | Interrupt (Ctrl+C) - Ngat (Ctrl+C)        |\n", SIGINT);
    printf("| SIGTERM| %-8d | Termination request - Yeu cau ket thuc     |\n", SIGTERM);
    printf("| SIGABRT| %-8d | Abnormal termination - Ket thuc bat thuong |\n", SIGABRT);
    printf("| SIGFPE | %-8d | Floating-point error - Loi dau phay dong   |\n", SIGFPE);
    printf("| SIGSEGV| %-8d | Segmentation fault - Loi bo nho            |\n", SIGSEGV);
    printf("+--------+----------+--------------------------------------------+\n\n");

    printf("Windows Console Events (Su kien Console Windows):\n");
    printf("+---------------------+-------+----------------------------------+\n");
    printf("| Event               | Value | Description (Mo ta)              |\n");
    printf("+---------------------+-------+----------------------------------+\n");
    printf("| CTRL_C_EVENT        |   0   | Ctrl+C                           |\n");
    printf("| CTRL_BREAK_EVENT    |   1   | Ctrl+Break                       |\n");
    printf("| CTRL_CLOSE_EVENT    |   2   | Console closing                  |\n");
    printf("| CTRL_LOGOFF_EVENT   |   5   | User logoff (Dang xuat)          |\n");
    printf("| CTRL_SHUTDOWN_EVENT |   6   | System shutdown (Tat may)        |\n");
    printf("+---------------------+-------+----------------------------------+\n\n");

    printf("Key differences (Khac biet chinh):\n");
    printf("  signal(): C standard, limited on Windows, handler resets after use.\n");
    printf("  signal(): Chuan C, han che tren Windows, handler bi reset sau khi dung.\n\n");
    printf("  SetConsoleCtrlHandler(): Windows-specific, more events, persists.\n");
    printf("  SetConsoleCtrlHandler(): Rieng Windows, nhieu su kien hon, khong bi reset.\n\n");
}

// ============================================================================
// Main - Hàm chính
// ============================================================================
int main() {
    printf("=== Signal Handling Demo (Minh hoa Xu Ly Tin Hieu) ===\n");
    printf("PID = %lu\n", GetCurrentProcessId());
    printf("=======================================================\n\n");

    // Part 1: Show signal reference (Hiển thị bảng tham khảo tín hiệu)
    ShowSignalInfo();

    // Part 2: C standard signals demo (Minh họa tín hiệu chuẩn C)
    DemoCSignals();

    // Part 3: Windows console ctrl handler (interactive)
    // Phần 3: Xử lý sự kiện console Windows (tương tác)
    DemoWindowsCtrlHandler();

    printf("=== Signal Handling Demo Complete ===\n");
    printf("=== Hoan tat minh hoa Xu Ly Tin Hieu ===\n");

    return 0;
}
