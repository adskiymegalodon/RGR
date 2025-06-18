#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <string>
#include <vector>
#include <fstream>
#include <codecvt>
#include <locale>

bool file_exists(const std::string& filename);
std::wstring read_text_file(const std::string& filename);
std::vector<unsigned char> read_binary_file(const std::string& filename);
void write_text_file(const std::string& filename, const std::wstring& content);
void write_binary_file(const std::string& filename, const std::vector<unsigned char>& content);

#endif // FILE_OPERATIONS_H