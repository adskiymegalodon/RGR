#ifndef VIGENERE_H
#define VIGENERE_H

#include "cipher.h"
#include <string>
#include <vector>

class VigenereCipher : public ICipher {
public:
    std::wstring encrypt(const std::wstring& text, const std::wstring& key) override;
    std::wstring decrypt(const std::wstring& text, const std::wstring& key) override;
    std::vector<unsigned char> encryptBinary(const std::vector<unsigned char>& data, const std::wstring& key) override;
    std::vector<unsigned char> decryptBinary(const std::vector<unsigned char>& data, const std::wstring& key) override;
    std::wstring generateKey() override;
    bool requiresKey() const override;
    bool supportsBinary() const override;
    std::wstring name() const override;

private:
    wchar_t getAlphabetBase(wchar_t c);
    int getAlphabetSize(wchar_t c);
    std::wstring normalizeKey(const std::wstring& key);
};

#endif // VIGENERE_H