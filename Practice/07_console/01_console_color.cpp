/*
 * ============================================================================
 * Exercise 07-01: Console Colors / Màu sắc Console
 * ============================================================================
 * Description (EN): Demonstrates SetConsoleTextAttribute to change console
 *                   text colors. Shows all 16 foreground and background colors,
 *                   builds a color name mapping, and creates colorful output.
 *
 * Mô tả (VI):      Minh họa SetConsoleTextAttribute để thay đổi màu văn bản
 *                   console. Hiển thị tất cả 16 màu nền trước và nền sau,
 *                   xây dựng bảng ánh xạ tên màu, và tạo đầu ra đầy màu sắc.
 *
 * Key APIs:  GetStdHandle, SetConsoleTextAttribute, GetConsoleScreenBufferInfo
 *
 * Compile:   g++ 01_console_color.cpp -o 01_console_color.exe
 * ============================================================================
 */

#include <windows.h>
#include <stdio.h>

// ============================================================================
// Bảng ánh xạ mã màu Windows Console
// Windows Console color code mapping table
// ============================================================================
// Console color = 4-bit value (0x0 - 0xF)
// Lower 4 bits = foreground (chữ) / Upper 4 bits = background (nền)
// Attribute = (background << 4) | foreground

struct ColorEntry {
    WORD  code;         // Mã màu / Color code (0-15)
    const char* nameEN; // Tên tiếng Anh / English name
    const char* nameVI; // Tên tiếng Việt / Vietnamese name
};

// 16 màu chuẩn của Windows Console / 16 standard Windows Console colors
ColorEntry COLORS[] = {
    { 0x0, "BLACK",        "Đen" },
    { 0x1, "DARK BLUE",    "Xanh dương đậm" },
    { 0x2, "DARK GREEN",   "Xanh lá đậm" },
    { 0x3, "DARK CYAN",    "Xanh lục lam đậm" },
    { 0x4, "DARK RED",     "Đỏ đậm" },
    { 0x5, "DARK MAGENTA", "Tím đậm" },
    { 0x6, "DARK YELLOW",  "Vàng đậm (nâu)" },
    { 0x7, "LIGHT GRAY",   "Xám nhạt" },
    { 0x8, "DARK GRAY",    "Xám đậm" },
    { 0x9, "BLUE",         "Xanh dương" },
    { 0xA, "GREEN",        "Xanh lá" },
    { 0xB, "CYAN",         "Xanh lục lam" },
    { 0xC, "RED",          "Đỏ" },
    { 0xD, "MAGENTA",      "Tím" },
    { 0xE, "YELLOW",       "Vàng" },
    { 0xF, "WHITE",        "Trắng" }
};

// ============================================================================
// Hàm tiện ích: Set màu chữ / Utility: Set text color
// ============================================================================
void SetColor(HANDLE hConsole, WORD foreground, WORD background = 0)
{
    // SetConsoleTextAttribute: Đặt thuộc tính màu cho văn bản
    // SetConsoleTextAttribute: Sets color attributes for console text
    // Attribute = (background << 4) | foreground
    // Ví dụ: Chữ đỏ trên nền xanh = (0x1 << 4) | 0xC = 0x1C
    // Example: Red text on blue bg = (0x1 << 4) | 0xC = 0x1C
    SetConsoleTextAttribute(hConsole, (background << 4) | foreground);
}

// ============================================================================
// Hàm tiện ích: Khôi phục màu mặc định / Utility: Reset to default colors
// ============================================================================
void ResetColor(HANDLE hConsole, WORD originalAttr)
{
    SetConsoleTextAttribute(hConsole, originalAttr);
}

// ============================================================================
// Demo 1: Hiển thị tất cả 16 màu foreground
// Demo 1: Display all 16 foreground colors
// ============================================================================
void DemoForegroundColors(HANDLE hConsole, WORD origAttr)
{
    printf("=== 1. All 16 Foreground Colors / Tất cả 16 màu chữ ===\n\n");

    for (int i = 0; i < 16; i++) {
        printf("  ");
        // Set foreground color, black background / Đặt màu chữ, nền đen
        SetColor(hConsole, (WORD)i, 0x0);
        printf("  Color 0x%X: %-14s / %-20s  ",
               i, COLORS[i].nameEN, COLORS[i].nameVI);
        // In mẫu ký tự / Print sample characters
        printf("ABCDEFG abcdefg 1234567890");
        ResetColor(hConsole, origAttr);
        printf("\n");
    }

    // Chú ý: Màu 0 (đen) trên nền đen sẽ không nhìn thấy!
    // Note: Color 0 (black) on black background is invisible!
    printf("\n  Note / Ghi chú: Color 0 (BLACK) is invisible on black background!\n");
    printf("  Màu 0 (ĐEN) không nhìn thấy trên nền đen!\n\n");
}

// ============================================================================
// Demo 2: Hiển thị tất cả 16 màu background
// Demo 2: Display all 16 background colors
// ============================================================================
void DemoBackgroundColors(HANDLE hConsole, WORD origAttr)
{
    printf("=== 2. All 16 Background Colors / Tất cả 16 màu nền ===\n\n");

    for (int i = 0; i < 16; i++) {
        printf("  ");
        // Chọn foreground tương phản / Choose contrasting foreground
        WORD fgColor = (i < 8) ? 0xF : 0x0;  // Trắng cho nền tối, đen cho nền sáng
        SetColor(hConsole, fgColor, (WORD)i);
        printf("  BG 0x%X: %-14s / %-20s  Sample text  ",
               i, COLORS[i].nameEN, COLORS[i].nameVI);
        ResetColor(hConsole, origAttr);
        printf("\n");
    }
    printf("\n");
}

// ============================================================================
// Demo 3: Bảng kết hợp foreground × background (16×16 = 256 tổ hợp)
// Demo 3: Foreground × Background combination table (16×16 = 256 combos)
// ============================================================================
void DemoColorMatrix(HANDLE hConsole, WORD origAttr)
{
    printf("=== 3. Color Matrix (FG x BG) / Ma trận màu ===\n\n");
    printf("  FG\\BG ");
    for (int bg = 0; bg < 16; bg++) {
        printf(" %X ", bg);
    }
    printf("\n  ------");
    for (int bg = 0; bg < 16; bg++) printf("---");
    printf("\n");

    for (int fg = 0; fg < 16; fg++) {
        printf("   0x%X  ", fg);
        for (int bg = 0; bg < 16; bg++) {
            SetConsoleTextAttribute(hConsole, (WORD)((bg << 4) | fg));
            printf("aA ");
        }
        ResetColor(hConsole, origAttr);
        printf("  %s\n", COLORS[fg].nameEN);
    }
    printf("\n");
}

// ============================================================================
// Demo 4: Tạo văn bản nhiều màu (rainbow text)
// Demo 4: Create rainbow-colored text
// ============================================================================
void DemoRainbowText(HANDLE hConsole, WORD origAttr)
{
    printf("=== 4. Rainbow Text / Văn bản cầu vồng ===\n\n");

    const char* message = "  Hello from Windows Console API! - Xin chao tu Console Windows!  ";
    int len = (int)strlen(message);

    // Màu cầu vồng / Rainbow colors
    WORD rainbow[] = { 0xC, 0xE, 0xA, 0xB, 0x9, 0xD };
    int numColors = sizeof(rainbow) / sizeof(rainbow[0]);

    printf("  ");
    for (int i = 0; i < len; i++) {
        SetColor(hConsole, rainbow[i % numColors]);
        printf("%c", message[i]);
    }
    ResetColor(hConsole, origAttr);
    printf("\n\n");

    // Rainbow background / Nền cầu vồng
    printf("  ");
    for (int i = 0; i < len; i++) {
        SetColor(hConsole, 0xF, rainbow[i % numColors]);
        printf("%c", message[i]);
    }
    ResetColor(hConsole, origAttr);
    printf("\n\n");
}

// ============================================================================
// Demo 5: Hàm set_color giống Tiny Shell
// Demo 5: set_color function similar to Tiny Shell
// ============================================================================
// Ánh xạ tên màu thành mã / Map color names to codes
WORD ColorNameToCode(const char* name)
{
    // Tìm kiếm theo tên (không phân biệt hoa thường)
    // Search by name (case-insensitive)
    const struct { const char* name; WORD code; } colorMap[] = {
        {"black",   0x0}, {"dark_blue",    0x1}, {"dark_green",  0x2},
        {"dark_cyan",0x3}, {"dark_red",     0x4}, {"dark_magenta",0x5},
        {"dark_yellow",0x6}, {"gray",       0x7}, {"dark_gray",  0x8},
        {"blue",    0x9}, {"green",         0xA}, {"cyan",        0xB},
        {"red",     0xC}, {"magenta",       0xD}, {"yellow",      0xE},
        {"white",   0xF}
    };

    for (int i = 0; i < 16; i++) {
        if (_stricmp(name, colorMap[i].name) == 0) {
            return colorMap[i].code;
        }
    }
    return 0x7; // Mặc định: xám nhạt / Default: light gray
}

void DemoSetColorFunction(HANDLE hConsole, WORD origAttr)
{
    printf("=== 5. set_color Function / Hàm set_color (giống Tiny Shell) ===\n\n");

    const char* testColors[] = {
        "red", "green", "blue", "yellow", "cyan", "magenta", "white"
    };

    for (int i = 0; i < 7; i++) {
        WORD code = ColorNameToCode(testColors[i]);
        SetColor(hConsole, code);
        printf("  set_color(\"%s\") -> code=0x%X: This is %s text!\n",
               testColors[i], code, testColors[i]);
    }
    ResetColor(hConsole, origAttr);
    printf("\n");
}

// ============================================================================
// Demo 6: Lấy thông tin thuộc tính console hiện tại
// Demo 6: Get current console attribute info
// ============================================================================
void DemoConsoleInfo(HANDLE hConsole)
{
    printf("=== 6. Console Buffer Info / Thông tin buffer Console ===\n\n");

    // GetConsoleScreenBufferInfo: Lấy thông tin buffer màn hình
    // GetConsoleScreenBufferInfo: Gets screen buffer information
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        printf("  Buffer Size / Kích thước buffer: %d x %d\n",
               csbi.dwSize.X, csbi.dwSize.Y);
        printf("  Window Size / Kích thước cửa sổ: %d x %d\n",
               csbi.srWindow.Right - csbi.srWindow.Left + 1,
               csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
        printf("  Cursor Position / Vị trí con trỏ: (%d, %d)\n",
               csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y);

        WORD attr = csbi.wAttributes;
        printf("  Current Attribute / Thuộc tính hiện tại: 0x%04X\n", attr);
        printf("    Foreground / Màu chữ: 0x%X (%s)\n",
               attr & 0x0F, COLORS[attr & 0x0F].nameEN);
        printf("    Background / Màu nền: 0x%X (%s)\n",
               (attr >> 4) & 0x0F, COLORS[(attr >> 4) & 0x0F].nameEN);
    } else {
        printf("  [ERROR] GetConsoleScreenBufferInfo failed / Lỗi: %lu\n",
               GetLastError());
    }
    printf("\n");
}

// ============================================================================
// Main
// ============================================================================
int main()
{
    // GetStdHandle: Lấy handle của thiết bị chuẩn (stdin, stdout, stderr)
    // GetStdHandle: Gets handle for standard device
    // STD_OUTPUT_HANDLE = -11: Handle cho stdout (màn hình console)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole == INVALID_HANDLE_VALUE) {
        printf("[ERROR] Cannot get console handle / Không lấy được handle console!\n");
        return 1;
    }

    // Lưu thuộc tính gốc để khôi phục sau / Save original attributes for restoration
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttr = csbi.wAttributes;

    printf("************************************************************\n");
    printf("*  Exercise 07-01: Console Colors / Bài tập: Màu Console  *\n");
    printf("************************************************************\n\n");

    DemoConsoleInfo(hConsole);
    DemoForegroundColors(hConsole, originalAttr);
    DemoBackgroundColors(hConsole, originalAttr);
    DemoColorMatrix(hConsole, originalAttr);
    DemoRainbowText(hConsole, originalAttr);
    DemoSetColorFunction(hConsole, originalAttr);

    // Đảm bảo khôi phục màu gốc / Ensure original colors are restored
    ResetColor(hConsole, originalAttr);

    printf("============================================================\n");
    printf("  Done! / Hoàn thành!\n");
    printf("============================================================\n");
    return 0;
}
