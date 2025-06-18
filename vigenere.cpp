#include "vigenere.h"
#include <iostream>
#include <codecvt>
#include <locale>
#include <algorithm>
#include <random>
#include <stdexcept>

using namespace std;

wstring VigenereCipher::encrypt(const wstring& text, const wstring& key) {
    if (key.empty()) return text;
    
    wstring result;
    for (size_t i = 0; i < text.length(); ++i) {
        wchar_t c = text[i];
        wchar_t k = key[i % key.length()];
        result += c + k;
    }
    return result;
}

wstring VigenereCipher::decrypt(const wstring& text, const wstring& key) {
    if (key.empty()) return text;
    
    wstring result;
    for (size_t i = 0; i < text.length(); ++i) {
        wchar_t c = text[i];
        wchar_t k = key[i % key.length()];
        result += c - k;
    }
    return result;
}

wstring VigenereCipher::generateKey() {
    const int length = 12;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(1, 65535);  

    int random_value = dis(gen);  
    wchar_t random_char = static_cast<wchar_t>(random_value);  

    wstring key;
    for (int i = 0; i < length; ++i) {
        key += dis(gen);
    }
    return key;
}

bool VigenereCipher::requiresKey() const {
    return true;
}

bool VigenereCipher::supportsBinary() const {
    return true;
}

vector<unsigned char> VigenereCipher::encryptBinary(const vector<unsigned char>& data, const wstring& key) {
    if (key.empty()) {
        throw runtime_error("Ключ не может быть пустым");
    }

    vector<unsigned char> result;
    for (size_t i = 0; i < data.size(); ++i) {
        wchar_t k = key[i % key.length()];
        unsigned char encrypted = data[i] + (k % 256);
        result.push_back(encrypted);
    }
    return result;
}

vector<unsigned char> VigenereCipher::decryptBinary(const vector<unsigned char>& data, const wstring& key) {
    if (key.empty()) {
        throw runtime_error("Ключ не может быть пустым");
    }

    vector<unsigned char> result;
    for (size_t i = 0; i < data.size(); ++i) {
        wchar_t k = key[i % key.length()];
        unsigned char decrypted = data[i] - (k % 256);
        result.push_back(decrypted);
    }
    return result;
}

wstring VigenereCipher::name() const {
    return L"Универсальный шифр Виженера";
}