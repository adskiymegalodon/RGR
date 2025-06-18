#ifndef UI_UTILITIES_H
#define UI_UTILITIES_H

#include "cipher.h"
#include "file_operations.h"
#include <string>
#include <memory>

bool authenticate(const std::string& prompt = "Пароль: ");
std::string input_path(const std::string& prompt, bool allow_empty = false, 
                      bool check_existence = false, bool allow_overwrite = false);
int safe_input_int(const std::string& prompt, int min_val, int max_val);
void process_operation(std::unique_ptr<ICipher>& cipher, int operation, bool is_file);
void cipher_menu(std::unique_ptr<ICipher>& cipher);
void main_menu();

#endif // UI_UTILITIES_H