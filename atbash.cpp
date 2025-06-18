#include "atbash.h"
#include <algorithm>
#include <random>

using namespace std;

wstring AtbashCipher::encrypt(const wstring& text, const wstring& key) {
    return transform(text);
}

wstring AtbashCipher::decrypt(const wstring& text, const wstring& key) {
    return transform(text);
}

wstring AtbashCipher::generateKey() {
    return L"";
}

bool AtbashCipher::requiresKey() const {
    return false;
}

bool AtbashCipher::supportsBinary() const {
    return true;
}

vector<unsigned char> AtbashCipher::encryptBinary(const vector<unsigned char>& data, const wstring& key) {
    vector<unsigned char> result(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = 255 - data[i];
    }
    return result;
}

vector<unsigned char> AtbashCipher::decryptBinary(const vector<unsigned char>& data, const wstring& key) {
    return encryptBinary(data, key);
}

wstring AtbashCipher::name() const {
    return L"Шифр Атбаш (Гост-совместимый)";
}

wstring AtbashCipher::transform(const wstring& text) {
    wstring result;
    for (wchar_t c : text) {
        result += transformChar(c);
    }
    return result;
}

wchar_t AtbashCipher::transformChar(wchar_t c) {
    if (c >= 0x0410 && c <= 0x042F) {
        return 0x0410 + (0x042F - c);
    }
    else if (c >= 0x0430 && c <= 0x044F) {
        return 0x0430 + (0x044F - c);
    }
    else if (c >= L'A' && c <= L'Z') {
        return L'Z' - (c - L'A');
    }
    else if (c >= L'a' && c <= L'z') {
        return L'z' - (c - L'a');
    }
    return c;
}