#ifndef ATBASH_H
#define ATBASH_H

#include "cipher.h"
#include <string>
#include <vector>

class AtbashCipher : public ICipher {
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
    std::wstring transform(const std::wstring& text);
    wchar_t transformChar(wchar_t c);
};

#endif // ATBASH_H