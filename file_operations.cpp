#include "file_operations.h"
#include "converter_utils.h"
#include <sys/stat.h>
#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#define stat _stat
#else
#include <unistd.h>
#endif

bool file_exists(const std::string& filename) {
#ifdef _WIN32
    std::wstring wide_path = s2ws(filename);
    struct _stat buffer;
    return (_wstat(wide_path.c_str(), &buffer) == 0);
#else
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
#endif
}

std::wstring read_text_file(const std::string& filename) {
#ifdef _WIN32
    std::wstring wide_path = s2ws(filename);
    std::wifstream wif(wide_path);
#else
    std::wifstream wif(filename);
#endif
    if (!wif) {
        throw std::runtime_error("Не удалось открыть файл: " + filename);
    }
    wif.imbue(std::locale(std::locale::classic(), new std::codecvt_utf8<wchar_t>));
    std::wstring content((std::istreambuf_iterator<wchar_t>(wif)),
        std::istreambuf_iterator<wchar_t>());
    return content;
}

std::vector<unsigned char> read_binary_file(const std::string& filename) {
#ifdef _WIN32
    std::wstring wide_path = s2ws(filename);
    std::ifstream file(wide_path, std::ios::binary | std::ios::ate);
#else
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
#endif
    if (!file) {
        throw std::runtime_error("Не удалось открыть файл: " + filename);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(size);
    if (size > 0 && !file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Ошибка чтения файла: " + filename);
    }
    return buffer;
}

void write_text_file(const std::string& filename, const std::wstring& content) {
#ifdef _WIN32
    std::wstring wide_path = s2ws(filename);
    std::wofstream wof(wide_path);
#else
    std::wofstream wof(filename);
#endif
    wof.imbue(std::locale(std::locale::classic(), new std::codecvt_utf8<wchar_t>));
    wof << content;
}

void write_binary_file(const std::string& filename, const std::vector<unsigned char>& content) {
#ifdef _WIN32
    std::wstring wide_path = s2ws(filename);
    std::ofstream file(wide_path, std::ios::binary);
#else
    std::ofstream file(filename, std::ios::binary);
#endif
    file.write(reinterpret_cast<const char*>(content.data()), content.size());
}