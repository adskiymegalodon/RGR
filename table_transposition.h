#ifndef TABLE_TRANSPOSITION_H
#define TABLE_TRANSPOSITION_H

#include "cipher.h"
#include <string>
#include <vector>
#include <algorithm>

class TableTranspositionCipher : public ICipher {
public:
    std::wstring encrypt(const std::wstring& text, const std::wstring& key) override;
    std::wstring decrypt(const std::wstring& cipher_text, const std::wstring& key) override;
    std::vector<unsigned char> encryptBinary(const std::vector<unsigned char>& data, const std::wstring& key) override;
    std::vector<unsigned char> decryptBinary(const std::vector<unsigned char>& data, const std::wstring& key) override;
    std::wstring generateKey() override;
    bool requiresKey() const override;
    bool supportsBinary() const override;
    std::wstring name() const override;

private:
    std::vector<int> getColumnOrder(const std::wstring& key);
};

#endif // TABLE_TRANSPOSITION_H