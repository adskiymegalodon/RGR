#include "cipher.h"
#include "file_operations.h"
#include "ui_utilities.h"
#include "installer.h"
#include "cipher_interface.h"
#include <iostream>
#include <locale>
#include <memory>
#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

using namespace std;

// Прототипы функций для динамической загрузки
typedef CipherHandle(*CreateCipherFunc)(int);
typedef void (*FreeCipherFunc)(CipherHandle);
typedef const wchar_t* (*EncryptTextFunc)(CipherHandle, const wchar_t*, const wchar_t*);
typedef const wchar_t* (*DecryptTextFunc)(CipherHandle, const wchar_t*, const wchar_t*);
typedef const wchar_t* (*GetCipherNameFunc)(CipherHandle);
typedef const wchar_t* (*GenerateKeyFunc)(CipherHandle);
typedef void (*FreeStringFunc)(const wchar_t*);

class DllCipherAdapter : public ICipher {
public:
    DllCipherAdapter(void* dll_handle, int type)
        : handle(nullptr), dll_handle(dll_handle) {

        auto create = (CreateCipherFunc)getFunc("create_cipher");
        auto get_name = (GetCipherNameFunc)getFunc("get_cipher_name");

        if (create && get_name) {
            handle = create(type);
            name_str = get_name(handle);
        }
    }

    ~DllCipherAdapter() {
        if (handle) {
            auto free_cipher = (FreeCipherFunc)getFunc("free_cipher");
            if (free_cipher) free_cipher(handle);
        }
    }

    wstring encrypt(const wstring& text, const wstring& key) override {
        auto func = (EncryptTextFunc)getFunc("encrypt_text");
        if (!func) return L"";
        const wchar_t* result = func(handle, text.c_str(), key.c_str());
        wstring res(result);
        auto free_str = (FreeStringFunc)getFunc("free_string");
        if (free_str) free_str(result);
        return res;
    }

    wstring decrypt(const wstring& text, const wstring& key) override {
        auto func = (DecryptTextFunc)getFunc("decrypt_text");
        if (!func) return L"";
        const wchar_t* result = func(handle, text.c_str(), key.c_str());
        wstring res(result);
        auto free_str = (FreeStringFunc)getFunc("free_string");
        if (free_str) free_str(result);
        return res;
    }

    vector<unsigned char> encryptBinary(const vector<unsigned char>& data, const wstring& key) override {
        // Для простоты примера не реализовано
        throw runtime_error("Бинарные операции не поддерживаются через DLL");
    }

    vector<unsigned char> decryptBinary(const vector<unsigned char>& data, const wstring& key) override {
        // Для простоты примера не реализовано
        throw runtime_error("Бинарные операции не поддерживаются через DLL");
    }

    wstring generateKey() override {
        auto func = (GenerateKeyFunc)getFunc("generate_key");
        if (!func) return L"";
        const wchar_t* result = func(handle);
        wstring res(result);
        auto free_str = (FreeStringFunc)getFunc("free_string");
        if (free_str) free_str(result);
        return res;
    }

    bool requiresKey() const override {
        // Для простоты всегда true
        return true;
    }

    bool supportsBinary() const override {
        return false;
    }

    wstring name() const override {
        return name_str;
    }

private:
    void* getFunc(const char* name) {
#ifdef _WIN32
        return GetProcAddress((HMODULE)dll_handle, name);
#else
        return dlsym(dll_handle, name);
#endif
    }

    CipherHandle handle;
    void* dll_handle;
    wstring name_str;
};

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    // Установка локали для Linux
    setlocale(LC_ALL, "ru_RU.UTF-8");
    locale::global(locale("ru_RU.UTF-8"));
#endif

    if (!authenticate("Введите пароль для доступа к программе: ")) {
        cout << "Неверный пароль! Программа будет закрыта.\n";
        return 1;
    }

    cout << "\nДоступ разрешен. Добро пожаловать в систему шифрования!\n";
    cout << "=== Технические характеристики ===\n"
        << "- Поддержка Windows/Linux/macOS\n"
        << "- Кодировка UTF-8\n"
        << "- Соответствие ГОСТ 19.201-78, ГОСТ Р ИСО/МЭК 25010-2015\n"
        << "- Работа с текстовыми и бинарными файлами\n\n"
        << "=== Инструкция по установке ===\n"
        << "1. В главном меню выберите 'Установщик'\n"
        << "2. Сгенерируйте скрипты компиляции\n"
        << "3. Проверьте наличие компилятора\n"
        << "4. Скомпилируйте программу\n"
        << "5. Пароль по умолчанию: 6230\n\n"
        << "=== Инструкция по использованию ===\n"
        << "1. Для файлов указывайте полные пути\n"
        << "2. Все алгоритмы работают с кириллицей и латиницей\n"
        << "3. Табличная перестановка поддерживает бинарные данные\n"
        << "4. Запрещено сохранять результат и ключ в одном файле\n";

    try {
        main_menu();
    }
    catch (const exception& e) {
        cerr << "Критическая ошибка: " << e.what() << endl;
        return 1;
    }
    return 0;
}