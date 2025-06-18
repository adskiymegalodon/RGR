#ifndef CIPHER_H
#define CIPHER_H

#include <string>
#include <vector>
#include <memory>
#include "converter_utils.h"

#ifdef CIPHERLIB_EXPORTS
#define CIPHERLIB_API __declspec(dllexport)
#else
#define CIPHERLIB_API 
#endif

class ICipher {
public:
    virtual ~ICipher() = default;
    virtual std::wstring encrypt(const std::wstring& text, const std::wstring& key) = 0;
    virtual std::wstring decrypt(const std::wstring& cipher_text, const std::wstring& key) = 0;
    virtual std::vector<unsigned char> encryptBinary(const std::vector<unsigned char>& data, const std::wstring& key) = 0;
    virtual std::vector<unsigned char> decryptBinary(const std::vector<unsigned char>& data, const std::wstring& key) = 0;
    virtual std::wstring generateKey() = 0;
    virtual bool requiresKey() const = 0;
    virtual bool supportsBinary() const = 0;
    virtual std::wstring name() const = 0;
};

class CipherLoader {
public:
    static CIPHERLIB_API std::unique_ptr<ICipher> createCipher(int type);
};

#endif // CIPHER_H