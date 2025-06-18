#include "installer.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <random>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <Windows.h>
#define chdir _chdir
#define mkdir _mkdir
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace std;

void create_compilation_scripts() {
    ofstream win_script("compile_windows.bat");
    win_script << "@echo off\n";
    win_script << "echo Компиляция программы для Windows...\n";
    win_script << "g++ -std=c++17 -o cipher_program.exe main.cpp cipher.cpp atbash.cpp vigenere.cpp table_transposition.cpp file_operations.cpp ui_utilities.cpp installer.cpp converter_utils.cpp -static -lstdc++ -lwinmm -luser32\n";
    win_script << "if exist cipher_program.exe (\n";
    win_script << "    echo Успешно скомпилировано! Запустите cipher_program.exe\n";
    win_script << ") else (\n";
    win_script << "    echo Ошибка компиляции! Убедитесь что установлен MinGW\n";
    win_script << ")\n";
    win_script << "pause\n";
    win_script.close();

    ofstream win_dll_script("compile_dll_windows.bat");
    win_dll_script << "@echo off\n";
    win_dll_script << "echo Компиляция DLL для Windows...\n";
    win_dll_script << "g++ -shared -std=c++17 -o cipher.dll cipher.cpp atbash.cpp vigenere.cpp table_transposition.cpp converter_utils.cpp -DCIPHERLIB_EXPORTS\n";
    win_dll_script << "if exist cipher.dll (\n";
    win_dll_script << "    echo Успешно скомпилировано! DLL: cipher.dll\n";
    win_dll_script << ") else (\n";
    win_dll_script << "    echo Ошибка компиляции!\n";
    win_dll_script << ")\n";
    win_dll_script << "pause\n";
    win_dll_script.close();

    ofstream linux_script("compile_linux.sh");
    linux_script << "#!/bin/bash\n";
    linux_script << "echo \"Компиляция программы для Linux...\"\n";
    linux_script << "g++ -std=c++17 -o cipher_program main.cpp cipher.cpp atbash.cpp vigenere.cpp table_transposition.cpp file_operations.cpp ui_utilities.cpp installer.cpp converter_utils.cpp\n";
    linux_script << "if [ -f \"cipher_program\" ]; then\n";
    linux_script << "    echo \"Успешно скомпилировано! Запустите: ./cipher_program\"\n";
    linux_script << "else\n";
    linux_script << "    echo \"Ошибка компиляции! Установите g++: sudo apt install g++\"\n";
    linux_script << "fi\n";
    linux_script << "read -p \"Нажмите Enter для выхода\"\n";
    linux_script.close();

    ofstream mac_script("compile_mac.sh");
    mac_script << "#!/bin/bash\n";
    mac_script << "echo \"Компиляция программы для macOS...\"\n";
    mac_script << "g++ -std=c++17 -o cipher_program main.cpp cipher.cpp atbash.cpp vigenere.cpp table_transposition.cpp file_operations.cpp ui_utilities.cpp installer.cpp converter_utils.cpp\n";
    mac_script << "if [ -f \"cipher_program\" ]; then\n";
    mac_script << "    echo \"Успешно скомпилировано! Запустите: ./cipher_program\"\n";
    mac_script << "else\n";
    mac_script << "    echo \"Ошибка компиляции! Установите Xcode Command Line Tools: xcode-select --install\"\n";
    mac_script << "fi\n";
    mac_script << "read -p \"Нажмите Enter для выхода\"\n";
    mac_script.close();

#ifndef _WIN32
    chmod("compile_linux.sh", S_IRUSR | S_IWUSR | S_IXUSR);
    chmod("compile_mac.sh", S_IRUSR | S_IWUSR | S_IXUSR);
#endif
}

bool check_compiler() {
    cout << "Проверка наличия компилятора C++...\n";
#ifdef _WIN32
    int result = system("g++ --version > nul 2>&1");
#else
    int result = system("g++ --version > /dev/null 2>&1");
#endif
    return (result == 0);
}

bool compile_program() {
    cout << "Запуск компиляции...\n";
#ifdef _WIN32
    int result = system("compile_windows.bat");
#else
#ifdef __linux__
    int result = system("./compile_linux.sh");
#elif __APPLE__
    int result = system("./compile_mac.sh");
#else
    cout << "Неподдерживаемая платформа!\n";
    return false;
#endif
#endif
    return (result == 0);
}

void installer_menu() {
    cout << "\n============ УСТАНОВЩИК ============\n";
    cout << "1. Сгенерировать скрипты компиляции\n";
    cout << "2. Скомпилировать программу\n";
    cout << "3. Скомпилировать DLL (Windows)\n";
    cout << "4. Проверить наличие компилятора\n";
    cout << "5. Вернуться в главное меню\n";

    int choice;
    cout << "Ваш выбор: ";
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        create_compilation_scripts();
        cout << "Скрипты компиляции созданы!\n";
        cout << " - Windows: compile_windows.bat\n";
        cout << " - DLL Windows: compile_dll_windows.bat\n";
        cout << " - Linux: compile_linux.sh\n";
        cout << " - macOS: compile_mac.sh\n";
        break;
    case 2:
        if (!check_compiler()) {
            cout << "Компилятор не найден! Сначала установите компилятор:\n";
            cout << " - Windows: установите MinGW\n";
            cout << " - Linux: sudo apt install g++\n";
            cout << " - macOS: xcode-select --install\n";
            break;
        }
        if (compile_program()) {
            cout << "Программа успешно скомпилирована!\n";
        }
        else {
            cout << "Ошибка компиляции! Смотрите сообщения выше.\n";
        }
        break;
    case 3:
#ifdef _WIN32
        if (check_compiler()) {
            cout << "Компиляция DLL...\n";
            if (system("compile_dll_windows.bat") == 0) {
                cout << "DLL успешно скомпилирована (cipher.dll)\n";
            }
            else {
                cout << "Ошибка компиляции DLL!\n";
            }
        }
        else {
            cout << "Компилятор не найден!\n";
        }
#else
        cout << "DLL поддерживается только на Windows!\n";
#endif
        break;
    case 4:
        if (check_compiler()) {
            cout << "Компилятор C++ найден!\n";
        }
        else {
            cout << "Компилятор не найден!\n";
        }
        break;
    case 5:
        return;
    default:
        cout << "Неверный выбор!\n";
    }
}