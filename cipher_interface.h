#pragma once

#ifdef _WIN32
    #ifdef CIPHER_EXPORTS
        #define CIPHER_API __declspec(dllexport)
    #else
        #define CIPHER_API __declspec(dllimport)
    #endif
#else
    #define CIPHER_API __attribute__((visibility("default")))
#endif

typedef void* CipherHandle;

extern "C" {
    CIPHER_API CipherHandle create_cipher(int type);
    CIPHER_API void free_cipher(CipherHandle handle);
    CIPHER_API const wchar_t* encrypt_text(CipherHandle handle, const wchar_t* text, const wchar_t* key);
    CIPHER_API const wchar_t* decrypt_text(CipherHandle handle, const wchar_t* text, const wchar_t* key);
    CIPHER_API const wchar_t* get_cipher_name(CipherHandle handle);
    CIPHER_API const wchar_t* generate_key(CipherHandle handle);
    CIPHER_API void free_string(const wchar_t* str);
}