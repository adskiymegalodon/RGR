#include "table_transposition.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include "converter_utils.h"

using namespace std;

wstring TableTranspositionCipher::encrypt(const wstring& text, const wstring& key) {
    if (key.empty()) throw invalid_argument("Ключ не может быть пустым");
    vector<int> order = getColumnOrder(key);
    size_t cols = key.size();
    size_t rows = (text.size() + cols - 1) / cols;

    vector<vector<wchar_t>> table(rows, vector<wchar_t>(cols, L'\0'));
    for (size_t i = 0; i < text.size(); ++i) {
        table[i / cols][i % cols] = text[i];
    }

    vector<vector<wchar_t>> sorted_table(rows, vector<wchar_t>(cols, L'\0'));
    for (size_t col = 0; col < cols; ++col) {
        size_t new_col = order[col];
        for (size_t row = 0; row < rows; ++row) {
            sorted_table[row][new_col] = table[row][col];
        }
    }

    wstring result;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            if (sorted_table[row][col] != L'\0') {
                result += sorted_table[row][col];
            }
        }
    }
    return result;
}

wstring TableTranspositionCipher::decrypt(const wstring& cipher_text, const wstring& key) {
    if (key.empty()) throw invalid_argument("Ключ не может быть пустым");
    vector<int> order = getColumnOrder(key);
    size_t cols = key.size();
    size_t rows = (cipher_text.size() + cols - 1) / cols;

    vector<vector<wchar_t>> table(rows, vector<wchar_t>(cols, L'\0'));
    size_t index = 0;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            if (index < cipher_text.size()) {
                table[row][col] = cipher_text[index++];
            }
        }
    }

    vector<vector<wchar_t>> original_table(rows, vector<wchar_t>(cols, L'\0'));
    for (size_t new_col = 0; new_col < cols; ++new_col) {
        auto it = find(order.begin(), order.end(), new_col);
        if (it == order.end()) throw runtime_error("Ошибка в порядке столбцов");
        size_t orig_col = distance(order.begin(), it);
        for (size_t row = 0; row < rows; ++row) {
            original_table[row][orig_col] = table[row][new_col];
        }
    }

    wstring result;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            if (original_table[row][col] != L'\0') {
                result += original_table[row][col];
            }
        }
    }
    return result;
}

wstring TableTranspositionCipher::generateKey() {
    const int length = 8; // Длина ключа в байтах
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, 255);

    wstring key;
    for (int i = 0; i < length; ++i) {
        key += static_cast<wchar_t>(dist(gen));
    }
    return key;
}

bool TableTranspositionCipher::requiresKey() const {
    return true;
}

bool TableTranspositionCipher::supportsBinary() const {
    return true;
}

vector<unsigned char> TableTranspositionCipher::encryptBinary(const vector<unsigned char>& data, const wstring& key) {
    if (key.empty()) throw invalid_argument("Ключ не может быть пустым");
    vector<int> order = getColumnOrder(key);
    size_t cols = key.size();
    size_t data_size = data.size();

    vector<unsigned char> header(8);
    for (int i = 0; i < 8; ++i) {
        header[i] = static_cast<unsigned char>((data_size >> (i * 8)) & 0xFF);
    }

    size_t block_size = cols;
    size_t padded_size = ((data_size + block_size - 1) / block_size) * block_size;
    vector<unsigned char> padded_data = data;
    padded_data.resize(padded_size, 0x00);

    size_t rows = padded_size / cols;
    vector<vector<unsigned char>> table(rows, vector<unsigned char>(cols));

    for (size_t i = 0; i < padded_size; ++i) {
        table[i / cols][i % cols] = padded_data[i];
    }

    vector<vector<unsigned char>> sorted_table(rows, vector<unsigned char>(cols));
    for (size_t col = 0; col < cols; ++col) {
        size_t new_col = order[col];
        for (size_t row = 0; row < rows; ++row) {
            sorted_table[row][new_col] = table[row][col];
        }
    }

    vector<unsigned char> result = header;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            result.push_back(sorted_table[row][col]);
        }
    }
    return result;
}

vector<unsigned char> TableTranspositionCipher::decryptBinary(const vector<unsigned char>& data, const wstring& key) {
    if (key.empty()) throw invalid_argument("Ключ не может быть пустым");
    if (data.size() < 8) throw runtime_error("Неверный формат данных");

    size_t original_size = 0;
    for (int i = 0; i < 8; ++i) {
        original_size |= static_cast<size_t>(data[i]) << (i * 8);
    }

    vector<unsigned char> encrypted(data.begin() + 8, data.end());
    vector<int> order = getColumnOrder(key);
    size_t cols = key.size();
    size_t encrypted_size = encrypted.size();

    if (encrypted_size == 0 || encrypted_size % cols != 0) {
        throw runtime_error("Некорректный размер данных для дешифрования");
    }

    size_t rows = encrypted_size / cols;
    vector<vector<unsigned char>> table(rows, vector<unsigned char>(cols));
    size_t index = 0;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            table[row][col] = encrypted[index++];
        }
    }

    vector<vector<unsigned char>> original_table(rows, vector<unsigned char>(cols));
    for (size_t new_col = 0; new_col < cols; ++new_col) {
        auto it = find(order.begin(), order.end(), new_col);
        if (it == order.end()) throw runtime_error("Ошибка порядка столбцов");
        size_t orig_col = distance(order.begin(), it);
        for (size_t row = 0; row < rows; ++row) {
            original_table[row][orig_col] = table[row][new_col];
        }
    }

    vector<unsigned char> result;
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            result.push_back(original_table[row][col]);
        }
    }

    result.resize(original_size);
    return result;
}

wstring TableTranspositionCipher::name() const {
    return L"Табличная перестановка (ГОСТ-совместимая)";
}

vector<int> TableTranspositionCipher::getColumnOrder(const wstring& key) {
    vector<pair<wchar_t, size_t>> key_chars;
    for (size_t i = 0; i < key.size(); ++i) {
        key_chars.emplace_back(key[i], i);
    }

    stable_sort(key_chars.begin(), key_chars.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    vector<int> order(key.size());
    for (size_t i = 0; i < key_chars.size(); ++i) {
        order[key_chars[i].second] = i;
    }
    return order;
}