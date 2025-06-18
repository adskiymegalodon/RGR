#ifndef CONVERTER_UTILS_H
#define CONVERTER_UTILS_H

#include <iostream> // Добавлен недостающий заголовок
#include <string>
#include <codecvt>
#include <locale>


// Функция преобразования string -> wstring
inline std::wstring s2ws(const std::string& str) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }
    catch (const std::range_error& e) {
        std::cout << "Ошибка преобразования строки: " << e.what() << std::endl; // Исправлено cert -> cerr
        return L"";
    }
}

// Функция преобразования wstring -> string
inline std::string ws2s(const std::wstring& wstr) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    }
    catch (const std::range_error& e) {
        std::cerr << "Ошибка преобразования строки: " << e.what() << std::endl; // Исправлено cert -> cerr
        return "";
    }
}

#endif // CONVERTER_UTILS_H