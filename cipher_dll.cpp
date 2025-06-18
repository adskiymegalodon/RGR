#define CIPHER_EXPORTS  // Должно быть ПЕРЕД включением заголовочного файла

#include "cipher_interface.h"
#include "cipher.h"
#include <memory>
#include <string>
#include <cstring>

extern "C" {

    CIPHER_API CipherHandle create_cipher(int type) {
        try {
            auto cipher = CipherLoader::createCipher(type);
            return cipher.release();
        }
        catch (...) {
            return nullptr;
        }
    }

    CIPHER_API void free_cipher(CipherHandle handle) {
        delete static_cast<ICipher*>(handle);
    }

    CIPHER_API const wchar_t* encrypt_text(CipherHandle handle, const wchar_t* text, const wchar_t* key) {
        // Проверка входных параметров
        if (!handle || !text || !key) return nullptr;

        ICipher* cipher = static_cast<ICipher*>(handle);
        try {
            std::wstring result = cipher->encrypt(text, key);
            wchar_t* output = new (std::nothrow) wchar_t[result.size() + 1];
            if (!output) return nullptr; // Не удалось выделить память

            wcscpy_s(output, result.size() + 1, result.c_str());
            return output;
        }
        catch (...) {
            return nullptr;
        }
    }

    CIPHER_API const wchar_t* decrypt_text(CipherHandle handle, const wchar_t* text, const wchar_t* key) {
        if (!handle || !text || !key) return nullptr;

        ICipher* cipher = static_cast<ICipher*>(handle);
        try {
            std::wstring result = cipher->decrypt(text, key);
            wchar_t* output = new (std::nothrow) wchar_t[result.size() + 1];
            if (!output) return nullptr;

            wcscpy_s(output, result.size() + 1, result.c_str());
            return output;
        }
        catch (...) {
            return nullptr;
        }
    }

    CIPHER_API const wchar_t* get_cipher_name(CipherHandle handle) {
        if (!handle) return nullptr;

        ICipher* cipher = static_cast<ICipher*>(handle);
        try {
            std::wstring name = cipher->name();
            wchar_t* output = new (std::nothrow) wchar_t[name.size() + 1];
            if (!output) return nullptr;

            wcscpy_s(output, name.size() + 1, name.c_str());
            return output;
        }
        catch (...) {
            return nullptr;
        }
    }

    CIPHER_API const wchar_t* generate_key(CipherHandle handle) {
        if (!handle) return nullptr;

        ICipher* cipher = static_cast<ICipher*>(handle);
        try {
            std::wstring key = cipher->generateKey();
            wchar_t* output = new (std::nothrow) wchar_t[key.size() + 1];
            if (!output) return nullptr;

            wcscpy_s(output, key.size() + 1, key.c_str());
            return output;
        }
        catch (...) {
            return nullptr;
        }
    }

    CIPHER_API void free_string(const wchar_t* str) {
        delete[] str;
    }

} // extern "C"