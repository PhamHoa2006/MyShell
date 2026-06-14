/*
 * ============================================================================
 * Exercise 07-02: Console Cursor / Con trỏ Console
 * ============================================================================
 * Description (EN): Demonstrates console cursor manipulation including
 *                   positioning, visibility control, and creating simple
 *                   text-based animations using cursor movement.
 *
 * Mô tả (VI):      Minh họa thao tác con trỏ console bao gồm định vị,
 *                   điều khiển hiển thị, và tạo hoạt ảnh đơn giản dựa trên
 *                   văn bản bằng cách di chuyển con trỏ.
 *
 * Key APIs:  SetConsoleCursorPosition, GetConsoleScreenBufferInfo,
 *            SetConsoleCursorInfo, FillConsoleOutputCharacter,
 *            FillConsoleOutputAttribute
 *
 * Compile:   g++ 02_console_cursor.cpp -o 02_console_cursor.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// ============================================================================
// Hàm tiện ích: Di chuyển con trỏ đến vị trí (x, y)
// Utility: Move cursor to position (x, y)
// ============================================================================
void GoToXY(HANDLE hConsole, short x, short y)
{
    // COORD: Cấu trúc chứa tọa độ 2D (X, Y) trên console
    // COORD: Structure containing 2D coordinates (X, Y) on console
    // X = cột (0 = trái nhất), Y = hàng (0 = trên cùng)
    // X = column (0 = leftmost), Y = row (0 = topmost)
    COORD pos = { x, y };

    // SetConsoleCursorPosition: Di chuyển con trỏ đến vị trí chỉ định
    // SetConsoleCursorPosition: Moves cursor to specified position
    // Tọa độ tính từ (0,0) ở góc trên bên trái
    // Coordinates start from (0,0) at top-left corner
    SetConsoleCursorPosition(hConsole, pos);
}

// ============================================================================
// Hàm tiện ích: Lấy vị trí con trỏ hiện tại
// Utility: Get current cursor position
// ============================================================================
COORD GetCursorPos(HANDLE hConsole)
{
    // GetConsoleScreenBufferInfo: Lấy thông tin buffer bao gồm vị trí con trỏ
    // GetConsoleScreenBufferInfo: Gets buffer info including cursor position
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.dwCursorPosition;
}

// ============================================================================
// Hàm tiện ích: Ẩn/hiện con trỏ
// Utility: Show/hide cursor
// ============================================================================
void SetCursorVisible(HANDLE hConsole, BOOL visible)
{
    // CONSOLE_CURSOR_INFO: Thông tin về con trỏ console
    // CONSOLE_CURSOR_INFO: Information about the console cursor
    // dwSize: Phần trăm ô ký tự mà con trỏ chiếm (1-100)
    // dwSize: Percentage of character cell filled by cursor (1-100)
    // bVisible: TRUE = hiển thị, FALSE = ẩn
    // bVisible: TRUE = visible, FALSE = hidden
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = visible;

    // SetConsoleCursorInfo: Đặt kích thước và hiển thị con trỏ
    // SetConsoleCursorInfo: Sets cursor size and visibility
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

// ============================================================================
// Hàm tiện ích: Xóa màn hình console
// Utility: Clear the console screen
// ============================================================================
void ClearScreen(HANDLE hConsole)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD charsWritten;
    COORD topLeft = { 0, 0 };

    // FillConsoleOutputCharacter: Điền ký tự vào buffer console
    // FillConsoleOutputCharacter: Fills console buffer with a character
    // Dùng khoảng trắng để "xóa" màn hình
    // Use spaces to "clear" the screen
    FillConsoleOutputCharacterA(hConsole, ' ', consoleSize, topLeft, &charsWritten);

    // FillConsoleOutputAttribute: Điền thuộc tính màu vào buffer
    // FillConsoleOutputAttribute: Fills console buffer with attributes
    // Khôi phục thuộc tính mặc định cho toàn bộ buffer
    // Restore default attributes for entire buffer
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, topLeft, &charsWritten);

    // Đưa con trỏ về góc trên bên trái / Move cursor to top-left
    SetConsoleCursorPosition(hConsole, topLeft);
}

// ============================================================================
// Demo 1: Vị trí con trỏ cơ bản
// Demo 1: Basic cursor positioning
// ============================================================================
void DemoCursorPositioning(HANDLE hConsole)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    short startRow = csbi.dwCursorPosition.Y;

    printf("=== 1. Cursor Positioning / Định vị con trỏ ===\n\n");

    // Lấy hàng bắt đầu cho demo / Get starting row for demo
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    startRow = csbi.dwCursorPosition.Y;

    // Viết text tại các vị trí khác nhau / Write text at different positions
    GoToXY(hConsole, 5, startRow);
    printf("Text at (5, %d) / Văn bản tại (5, %d)", startRow, startRow);

    GoToXY(hConsole, 10, startRow + 1);
    printf("Text at (10, %d) / Văn bản tại (10, %d)", startRow + 1, startRow + 1);

    GoToXY(hConsole, 20, startRow + 2);
    printf("Text at (20, %d) / Văn bản tại (20, %d)", startRow + 2, startRow + 2);

    GoToXY(hConsole, 30, startRow + 3);
    printf("Text at (30, %d) / Văn bản tại (30, %d)", startRow + 3, startRow + 3);

    // Nhảy về đầu dòng tiếp theo / Jump to next line
    GoToXY(hConsole, 0, startRow + 5);
    printf("\n");
}

// ============================================================================
// Demo 2: Vẽ khung hộp bằng con trỏ
// Demo 2: Draw a box using cursor positioning
// ============================================================================
void DemoDrawBox(HANDLE hConsole)
{
    printf("=== 2. Draw Box / Vẽ khung hộp ===\n\n");

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    short startRow = csbi.dwCursorPosition.Y;

    short boxLeft = 5, boxTop = startRow;
    short boxWidth = 40, boxHeight = 7;

    // Vẽ viền trên / Draw top border
    GoToXY(hConsole, boxLeft, boxTop);
    printf("+");
    for (int i = 0; i < boxWidth - 2; i++) printf("-");
    printf("+");

    // Vẽ viền trái và phải / Draw left and right borders
    for (int row = 1; row < boxHeight - 1; row++) {
        GoToXY(hConsole, boxLeft, boxTop + (short)row);
        printf("|");
        for (int i = 0; i < boxWidth - 2; i++) printf(" ");
        printf("|");
    }

    // Vẽ viền dưới / Draw bottom border
    GoToXY(hConsole, boxLeft, boxTop + boxHeight - 1);
    printf("+");
    for (int i = 0; i < boxWidth - 2; i++) printf("-");
    printf("+");

    // Viết text bên trong hộp / Write text inside the box
    GoToXY(hConsole, boxLeft + 3, boxTop + 2);
    printf("Box drawn with cursor! / Hộp vẽ bằng");
    GoToXY(hConsole, boxLeft + 3, boxTop + 3);
    printf("SetConsoleCursorPosition!");
    GoToXY(hConsole, boxLeft + 3, boxTop + 4);
    printf("Width=%d, Height=%d", boxWidth, boxHeight);

    GoToXY(hConsole, 0, boxTop + boxHeight + 1);
    printf("\n");
}

// ============================================================================
// Demo 3: Thanh tiến trình có hoạt ảnh
// Demo 3: Animated progress bar
// ============================================================================
void DemoProgressBar(HANDLE hConsole)
{
    printf("=== 3. Animated Progress Bar / Thanh tiến trình hoạt ảnh ===\n\n");

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    short barRow = csbi.dwCursorPosition.Y;

    int barWidth = 40;

    // Ẩn con trỏ cho hoạt ảnh mượt / Hide cursor for smooth animation
    SetCursorVisible(hConsole, FALSE);

    for (int progress = 0; progress <= 100; progress += 2) {
        // Di chuyển về đầu thanh tiến trình / Move to start of progress bar
        GoToXY(hConsole, 2, barRow);

        int filled = (int)((double)progress / 100.0 * barWidth);

        printf("  Progress / Tiến trình: [");
        for (int i = 0; i < barWidth; i++) {
            if (i < filled) {
                // Đổi màu cho phần đã hoàn thành / Change color for completed portion
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                printf("#");
            } else {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
                printf("-");
            }
        }
        // Khôi phục màu / Restore color
        SetConsoleTextAttribute(hConsole, csbi.wAttributes);
        printf("] %3d%%", progress);

        Sleep(50);  // Tạm dừng 50ms giữa mỗi frame / 50ms delay between frames
    }

    // Hiện lại con trỏ / Show cursor again
    SetCursorVisible(hConsole, TRUE);

    printf("\n\n  Done! Progress bar complete. / Hoàn thành!\n\n");
}

// ============================================================================
// Demo 4: Hoạt ảnh spinner (xoay tròn)
// Demo 4: Spinner animation
// ============================================================================
void DemoSpinner(HANDLE hConsole)
{
    printf("=== 4. Spinner Animation / Hoạt ảnh xoay ===\n\n");

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    short spinnerRow = csbi.dwCursorPosition.Y;

    // Các ký tự spinner / Spinner characters
    const char spinChars[] = "|/-\\";
    int numFrames = 40;  // Số frame hoạt ảnh / Number of animation frames

    SetCursorVisible(hConsole, FALSE);

    for (int i = 0; i < numFrames; i++) {
        GoToXY(hConsole, 2, spinnerRow);
        printf("  Loading / Đang tải... %c  [Frame %2d/%d]",
               spinChars[i % 4], i + 1, numFrames);
        Sleep(100);
    }

    GoToXY(hConsole, 2, spinnerRow);
    printf("  Loading / Đang tải... Done! / Xong!          ");

    SetCursorVisible(hConsole, TRUE);
    printf("\n\n");
}

// ============================================================================
// Demo 5: Hiệu ứng đánh máy (typewriter effect)
// Demo 5: Typewriter effect
// ============================================================================
void DemoTypewriter(HANDLE hConsole)
{
    printf("=== 5. Typewriter Effect / Hiệu ứng đánh máy ===\n\n  ");

    const char* text = "Welcome to Windows Console Programming! / Chao mung den lap trinh Console Windows!";

    SetCursorVisible(hConsole, FALSE);

    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        Sleep(40);  // Tạm dừng giữa mỗi ký tự / Pause between each character
    }

    SetCursorVisible(hConsole, TRUE);
    printf("\n\n");
}

// ============================================================================
// Demo 6: Di chuyển đối tượng trên màn hình
// Demo 6: Move an object across the screen
// ============================================================================
void DemoBounce(HANDLE hConsole)
{
    printf("=== 6. Bouncing Ball / Bóng nảy ===\n\n");
    printf("  (Watch the ball bounce / Xem bóng nảy)\n");

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    short startRow = csbi.dwCursorPosition.Y + 1;

    int width = 50;   // Chiều rộng sân / Field width
    int x = 2;        // Vị trí bóng / Ball position
    int dx = 1;       // Hướng di chuyển / Direction

    SetCursorVisible(hConsole, FALSE);

    // Vẽ viền / Draw borders
    GoToXY(hConsole, 1, startRow);
    printf("+");
    for (int i = 0; i < width; i++) printf("-");
    printf("+");

    GoToXY(hConsole, 1, startRow + 2);
    printf("+");
    for (int i = 0; i < width; i++) printf("-");
    printf("+");

    for (int frame = 0; frame < 100; frame++) {
        // Xóa vị trí cũ / Clear old position
        GoToXY(hConsole, (short)(x + 1), startRow + 1);
        printf(" ");

        // Cập nhật vị trí / Update position
        x += dx;
        if (x <= 1 || x >= width) {
            dx = -dx;  // Đổi hướng khi chạm biên / Reverse at border
        }

        // Vẽ bóng tại vị trí mới / Draw ball at new position
        GoToXY(hConsole, (short)(x + 1), startRow + 1);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        printf("O");
        SetConsoleTextAttribute(hConsole, csbi.wAttributes);

        Sleep(30);
    }

    SetCursorVisible(hConsole, TRUE);
    GoToXY(hConsole, 0, startRow + 4);
    printf("\n");
}

// ============================================================================
// Main
// ============================================================================
int main()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        printf("[ERROR] Cannot get console handle / Không lấy được handle!\n");
        return 1;
    }

    printf("************************************************************\n");
    printf("*  Exercise 07-02: Console Cursor / Bài tập: Con trỏ      *\n");
    printf("************************************************************\n\n");

    DemoCursorPositioning(hConsole);
    DemoDrawBox(hConsole);
    DemoProgressBar(hConsole);
    DemoSpinner(hConsole);
    DemoTypewriter(hConsole);
    DemoBounce(hConsole);

    printf("============================================================\n");
    printf("  Done! / Hoàn thành!\n");
    printf("============================================================\n");
    return 0;
}
