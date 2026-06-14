/*
 * ============================================================================
 * Exercise 07-03: Console Input / Nhập liệu Console
 * ============================================================================
 * Description (EN): Uses ReadConsoleInput to read raw keyboard and mouse
 *                   events from the console. Builds an interactive menu
 *                   that responds to arrow keys, Enter, and Escape.
 *
 * Mô tả (VI):      Sử dụng ReadConsoleInput để đọc sự kiện bàn phím và
 *                   chuột thô từ console. Xây dựng menu tương tác phản hồi
 *                   phím mũi tên, Enter, và Escape.
 *
 * Key APIs:  GetStdHandle, ReadConsoleInput, SetConsoleMode,
 *            SetConsoleCursorPosition, SetConsoleTextAttribute,
 *            FlushConsoleInputBuffer
 *
 * Compile:   g++ 03_console_input.cpp -o 03_console_input.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Hàm tiện ích / Utility functions
// ============================================================================

void GoToXY(HANDLE hOut, short x, short y)
{
    COORD pos = { x, y };
    SetConsoleCursorPosition(hOut, pos);
}

void SetColor(HANDLE hOut, WORD attr)
{
    SetConsoleTextAttribute(hOut, attr);
}

void ClearScreen(HANDLE hOut)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD written;
    COORD origin = { 0, 0 };
    FillConsoleOutputCharacterA(hOut, ' ', size, origin, &written);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, size, origin, &written);
    SetConsoleCursorPosition(hOut, origin);
}

void HideCursor(HANDLE hOut)
{
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(hOut, &ci);
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &ci);
}

void ShowCursor(HANDLE hOut)
{
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(hOut, &ci);
    ci.bVisible = TRUE;
    SetConsoleCursorInfo(hOut, &ci);
}

// ============================================================================
// Demo 1: Hiển thị sự kiện bàn phím thô
// Demo 1: Display raw keyboard events
// ============================================================================
void DemoRawKeyboardEvents(HANDLE hIn, HANDLE hOut)
{
    printf("=== 1. Raw Keyboard Events / Sự kiện bàn phím thô ===\n\n");
    printf("  Press keys to see events. Press ESC to continue.\n");
    printf("  Nhấn phím để xem sự kiện. Nhấn ESC để tiếp tục.\n\n");
    printf("  %-8s %-8s %-12s %-12s %-10s %s\n",
           "Type", "KeyDown", "VirtualKey", "ScanCode", "Char", "CtrlKeys");
    printf("  %-8s %-8s %-12s %-12s %-10s %s\n",
           "--------", "--------", "------------", "------------",
           "----------", "----------");

    // SetConsoleMode: Cấu hình chế độ console
    // SetConsoleMode: Configure console mode
    // Tắt ENABLE_LINE_INPUT và ENABLE_ECHO_INPUT để đọc từng sự kiện
    // Disable ENABLE_LINE_INPUT and ENABLE_ECHO_INPUT for raw event reading
    DWORD oldMode;
    GetConsoleMode(hIn, &oldMode);
    SetConsoleMode(hIn, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

    int eventCount = 0;
    BOOL running = TRUE;

    while (running && eventCount < 30) {
        INPUT_RECORD ir;
        DWORD eventsRead;

        // ReadConsoleInput: Đọc sự kiện đầu vào từ console input buffer
        // ReadConsoleInput: Reads input events from console input buffer
        // Hàm này CHẶN (blocking) cho đến khi có sự kiện
        // This function BLOCKS until an event is available
        // Trả về INPUT_RECORD chứa thông tin sự kiện
        // Returns INPUT_RECORD containing event information
        if (!ReadConsoleInput(hIn, &ir, 1, &eventsRead)) {
            printf("  [ERROR] ReadConsoleInput failed / Lỗi: %lu\n", GetLastError());
            break;
        }

        if (eventsRead == 0) continue;

        // ir.EventType: Loại sự kiện / Event type
        // KEY_EVENT (0x0001):    Sự kiện bàn phím / Keyboard event
        // MOUSE_EVENT (0x0002):  Sự kiện chuột / Mouse event
        // WINDOW_BUFFER_SIZE_EVENT: Thay đổi kích thước / Resize event
        // FOCUS_EVENT:           Sự kiện focus / Focus event
        // MENU_EVENT:            Sự kiện menu / Menu event

        if (ir.EventType == KEY_EVENT) {
            // KEY_EVENT_RECORD: Chi tiết sự kiện bàn phím
            // KEY_EVENT_RECORD: Keyboard event details
            KEY_EVENT_RECORD key = ir.Event.KeyEvent;

            // bKeyDown: TRUE = nhấn xuống, FALSE = thả ra
            // bKeyDown: TRUE = key pressed, FALSE = key released
            // wVirtualKeyCode: Mã phím ảo (VK_xxx)
            // wVirtualKeyCode: Virtual key code (VK_xxx)
            // wVirtualScanCode: Mã quét phần cứng
            // wVirtualScanCode: Hardware scan code
            // uChar.AsciiChar: Ký tự ASCII (nếu có)
            // uChar.AsciiChar: ASCII character (if applicable)
            // dwControlKeyState: Trạng thái phím điều khiển
            // dwControlKeyState: Control key state flags

            char charBuf[16];
            if (key.uChar.AsciiChar >= 32 && key.uChar.AsciiChar < 127) {
                snprintf(charBuf, sizeof(charBuf), "'%c' (0x%02X)",
                         key.uChar.AsciiChar, (unsigned char)key.uChar.AsciiChar);
            } else {
                snprintf(charBuf, sizeof(charBuf), "0x%02X",
                         (unsigned char)key.uChar.AsciiChar);
            }

            // Xây dựng chuỗi phím điều khiển / Build control key string
            char ctrlStr[64] = "";
            if (key.dwControlKeyState & LEFT_CTRL_PRESSED)  strcat(ctrlStr, "LCtrl ");
            if (key.dwControlKeyState & RIGHT_CTRL_PRESSED) strcat(ctrlStr, "RCtrl ");
            if (key.dwControlKeyState & LEFT_ALT_PRESSED)   strcat(ctrlStr, "LAlt ");
            if (key.dwControlKeyState & RIGHT_ALT_PRESSED)  strcat(ctrlStr, "RAlt ");
            if (key.dwControlKeyState & SHIFT_PRESSED)       strcat(ctrlStr, "Shift ");

            printf("  %-8s %-8s VK=0x%04X    SC=0x%04X    %-10s %s\n",
                   "KEY",
                   key.bKeyDown ? "DOWN" : "UP",
                   key.wVirtualKeyCode,
                   key.wVirtualScanCode,
                   charBuf,
                   ctrlStr[0] ? ctrlStr : "-");

            // Kiểm tra ESC để thoát / Check ESC to exit
            if (key.bKeyDown && key.wVirtualKeyCode == VK_ESCAPE) {
                running = FALSE;
            }
            eventCount++;
        }
    }

    // Khôi phục chế độ console / Restore console mode
    SetConsoleMode(hIn, oldMode);
    printf("\n");
}

// ============================================================================
// Demo 2: Nhận dạng phím đặc biệt
// Demo 2: Recognize special keys
// ============================================================================
void DemoSpecialKeys(HANDLE hIn, HANDLE hOut)
{
    printf("=== 2. Special Keys / Phím đặc biệt ===\n\n");
    printf("  Press arrow keys, F1-F12, Home, End, etc. ESC to continue.\n");
    printf("  Nhấn phím mũi tên, F1-F12, Home, End... ESC để tiếp tục.\n\n");

    DWORD oldMode;
    GetConsoleMode(hIn, &oldMode);
    SetConsoleMode(hIn, ENABLE_WINDOW_INPUT);

    BOOL running = TRUE;
    while (running) {
        INPUT_RECORD ir;
        DWORD eventsRead;

        ReadConsoleInput(hIn, &ir, 1, &eventsRead);
        if (eventsRead == 0) continue;

        if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
            WORD vk = ir.Event.KeyEvent.wVirtualKeyCode;
            const char* keyName = "Unknown / Không biết";

            // Ánh xạ Virtual Key Code sang tên phím
            // Map Virtual Key Code to key name
            switch (vk) {
                case VK_UP:      keyName = "UP ARROW / Mũi tên LÊN"; break;
                case VK_DOWN:    keyName = "DOWN ARROW / Mũi tên XUỐNG"; break;
                case VK_LEFT:    keyName = "LEFT ARROW / Mũi tên TRÁI"; break;
                case VK_RIGHT:   keyName = "RIGHT ARROW / Mũi tên PHẢI"; break;
                case VK_HOME:    keyName = "HOME / Đầu"; break;
                case VK_END:     keyName = "END / Cuối"; break;
                case VK_INSERT:  keyName = "INSERT / Chèn"; break;
                case VK_DELETE:  keyName = "DELETE / Xóa"; break;
                case VK_PRIOR:   keyName = "PAGE UP / Trang trên"; break;
                case VK_NEXT:    keyName = "PAGE DOWN / Trang dưới"; break;
                case VK_F1:      keyName = "F1"; break;
                case VK_F2:      keyName = "F2"; break;
                case VK_F3:      keyName = "F3"; break;
                case VK_F4:      keyName = "F4"; break;
                case VK_F5:      keyName = "F5"; break;
                case VK_F6:      keyName = "F6"; break;
                case VK_F7:      keyName = "F7"; break;
                case VK_F8:      keyName = "F8"; break;
                case VK_F9:      keyName = "F9"; break;
                case VK_F10:     keyName = "F10"; break;
                case VK_F11:     keyName = "F11"; break;
                case VK_F12:     keyName = "F12"; break;
                case VK_RETURN:  keyName = "ENTER / Nhập"; break;
                case VK_TAB:     keyName = "TAB"; break;
                case VK_BACK:    keyName = "BACKSPACE / Xóa lui"; break;
                case VK_SPACE:   keyName = "SPACE / Dấu cách"; break;
                case VK_ESCAPE:  keyName = "ESCAPE / Thoát"; running = FALSE; break;
                default:
                    if (vk >= 'A' && vk <= 'Z') {
                        static char letterBuf[32];
                        snprintf(letterBuf, sizeof(letterBuf), "Letter / Chữ '%c'", (char)vk);
                        keyName = letterBuf;
                    } else if (vk >= '0' && vk <= '9') {
                        static char numBuf[32];
                        snprintf(numBuf, sizeof(numBuf), "Number / Số '%c'", (char)vk);
                        keyName = numBuf;
                    }
                    break;
            }

            printf("  Key: VK=0x%04X -> %s\n", vk, keyName);
        }
    }

    SetConsoleMode(hIn, oldMode);
    printf("\n");
}

// ============================================================================
// Demo 3: Menu tương tác với phím mũi tên
// Demo 3: Interactive menu with arrow keys
// ============================================================================
void DemoInteractiveMenu(HANDLE hIn, HANDLE hOut)
{
    printf("=== 3. Interactive Menu / Menu tương tác ===\n\n");
    printf("  Use UP/DOWN arrows to navigate, ENTER to select, ESC to exit.\n");
    printf("  Dùng mũi tên LÊN/XUỐNG để di chuyển, ENTER để chọn, ESC để thoát.\n\n");

    // Định nghĩa các mục menu / Define menu items
    const char* menuItems[] = {
        "1. List Files       / Liệt kê tập tin",
        "2. Create Directory / Tạo thư mục",
        "3. Copy File        / Sao chép tập tin",
        "4. Delete File      / Xóa tập tin",
        "5. System Info      / Thông tin hệ thống",
        "6. Exit             / Thoát"
    };
    int numItems = sizeof(menuItems) / sizeof(menuItems[0]);
    int selected = 0;  // Mục đang chọn / Currently selected item

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    short menuStartRow = csbi.dwCursorPosition.Y;
    WORD normalAttr = csbi.wAttributes;
    WORD highlightAttr = BACKGROUND_BLUE | BACKGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

    DWORD oldMode;
    GetConsoleMode(hIn, &oldMode);
    SetConsoleMode(hIn, ENABLE_WINDOW_INPUT);

    HideCursor(hOut);

    BOOL running = TRUE;
    BOOL needRedraw = TRUE;

    while (running) {
        // Vẽ menu / Draw menu
        if (needRedraw) {
            for (int i = 0; i < numItems; i++) {
                GoToXY(hOut, 4, menuStartRow + (short)i);

                if (i == selected) {
                    // Mục đang chọn: nổi bật / Selected item: highlighted
                    SetColor(hOut, highlightAttr);
                    printf(" >> %-45s ", menuItems[i]);
                } else {
                    // Mục bình thường / Normal item
                    SetColor(hOut, normalAttr);
                    printf("    %-45s ", menuItems[i]);
                }
            }
            SetColor(hOut, normalAttr);
            needRedraw = FALSE;
        }

        // Đọc sự kiện / Read event
        INPUT_RECORD ir;
        DWORD eventsRead;
        ReadConsoleInput(hIn, &ir, 1, &eventsRead);

        if (eventsRead > 0 && ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
            switch (ir.Event.KeyEvent.wVirtualKeyCode) {
                case VK_UP:
                    // Di chuyển lên / Move up
                    selected = (selected - 1 + numItems) % numItems;
                    needRedraw = TRUE;
                    break;

                case VK_DOWN:
                    // Di chuyển xuống / Move down
                    selected = (selected + 1) % numItems;
                    needRedraw = TRUE;
                    break;

                case VK_HOME:
                    selected = 0;
                    needRedraw = TRUE;
                    break;

                case VK_END:
                    selected = numItems - 1;
                    needRedraw = TRUE;
                    break;

                case VK_RETURN:
                    // Chọn mục / Select item
                    GoToXY(hOut, 4, menuStartRow + (short)numItems + 1);
                    SetColor(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    printf("  Selected / Đã chọn: %s\n", menuItems[selected]);
                    SetColor(hOut, normalAttr);

                    if (selected == numItems - 1) {
                        // "Exit" selected / Đã chọn "Thoát"
                        running = FALSE;
                    } else {
                        // Tạm dừng để hiển thị lựa chọn / Pause to show selection
                        Sleep(1000);
                        GoToXY(hOut, 4, menuStartRow + (short)numItems + 1);
                        printf("%-60s", "");  // Xóa dòng / Clear line
                        needRedraw = TRUE;
                    }
                    break;

                case VK_ESCAPE:
                    running = FALSE;
                    break;
            }
        }
    }

    ShowCursor(hOut);
    SetConsoleMode(hIn, oldMode);
    SetColor(hOut, normalAttr);

    GoToXY(hOut, 0, menuStartRow + (short)numItems + 3);
    printf("  Menu closed. / Đã đóng menu.\n\n");
}

// ============================================================================
// Main
// ============================================================================
int main()
{
    // Lấy handle cho cả input và output
    // Get handles for both input and output
    HANDLE hIn  = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hIn == INVALID_HANDLE_VALUE || hOut == INVALID_HANDLE_VALUE) {
        printf("[ERROR] Cannot get console handles / Không lấy được handle!\n");
        return 1;
    }

    printf("************************************************************\n");
    printf("*  Exercise 07-03: Console Input / Bài tập: Nhập liệu     *\n");
    printf("************************************************************\n\n");

    DemoRawKeyboardEvents(hIn, hOut);
    DemoSpecialKeys(hIn, hOut);
    DemoInteractiveMenu(hIn, hOut);

    // FlushConsoleInputBuffer: Xóa sạch các sự kiện chưa xử lý
    // FlushConsoleInputBuffer: Clears all unprocessed events
    FlushConsoleInputBuffer(hIn);

    printf("============================================================\n");
    printf("  Done! / Hoàn thành!\n");
    printf("============================================================\n");
    return 0;
}
