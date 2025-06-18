#include "ui_utilities.h"
#include "installer.h"
#include "converter_utils.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <vector>

using namespace std;

bool authenticate(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input == "6230";
}

string input_path(const string& prompt, bool allow_empty, bool check_existence, bool allow_overwrite) {
    string path;
    while (true) {
        cout << prompt;
        getline(cin, path);

        path.erase(0, path.find_first_not_of(" \t\n\r\f\v"));
        path.erase(path.find_last_not_of(" \t\n\r\f\v") + 1);

        if (path.empty() && !allow_empty) {
            cout << "Ошибка: путь не может быть пустым. Попробуйте снова.\n";
            continue;
        }

        if (check_existence && file_exists(path)) {
            if (allow_overwrite) {
                cout << "Внимание: файл уже существует! Перезаписать? (y/n): ";
                string response;
                getline(cin, response);

                transform(response.begin(), response.end(), response.begin(), ::tolower);
                response.erase(0, response.find_first_not_of(" \t\n\r\f\v"));
                response.erase(response.find_last_not_of(" \t\n\r\f\v") + 1);

                if (response == "y" || response == "yes") {
                    return path;
                }
                else {
                    cout << "Файл не будет перезаписан. Пожалуйста, введите новый путь.\n";
                    continue;
                }
            }
            else {
                cout << "Ошибка: файл уже существует. Пожалуйста, введите другой путь.\n";
                continue;
            }
        }

        return path;
    }
}

int safe_input_int(const string& prompt, int min_val, int max_val) {
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);

        input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
        input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);

        if (input.empty()) {
            cout << "Ошибка: ввод не может быть пустым. Попробуйте снова.\n";
            continue;
        }

        bool valid = true;
        bool has_digit = false;
        for (size_t i = 0; i < input.size(); ++i) {
            char c = input[i];
            if (i == 0 && c == '-') continue;
            if (!isdigit(static_cast<unsigned char>(c))) {
                valid = false;
                break;
            }
            else {
                has_digit = true;
            }
        }

        if (!valid || !has_digit) {
            cout << "Ошибка: введите целое число без дополнительных символов. Попробуйте снова.\n";
            continue;
        }

        try {
            size_t pos;
            int value = stoi(input, &pos);
            if (pos != input.size()) {
                cout << "Ошибка: введите только число без дополнительных символов. Попробуйте снова.\n";
                continue;
            }

            if (value < min_val || value > max_val) {
                cout << "Ошибка: число должно быть от " << min_val << " до " << max_val << ". Попробуйте снова.\n";
                continue;
            }

            return value;
        }
        catch (const out_of_range&) {
            cout << "Ошибка: число слишком большое. Попробуйте снова.\n";
        }
        catch (const invalid_argument&) {
            cout << "Ошибка: введите корректное целое число. Попробуйте снова.\n";
        }
    }
}

void process_operation(unique_ptr<ICipher>& cipher, int operation, bool is_file) {
    bool auth_success = authenticate("Введите пароль для операции: ");
    if (!auth_success) {
        cout << "Неверный пароль! Операция отменена.\n";
        return;
    }

    wstring key;
    string key_str;
    bool key_generated = false;
    string key_output_file;

    if (cipher->requiresKey()) {
        if (operation == 2) {
            cout << "\nВыберите источник ключа:\n"
                << "1. Ввести вручную\n"
                << "2. Загрузить из файла\n";

            int key_choice = safe_input_int("Ваш выбор: ", 1, 2);

            if (key_choice == 1) {
                cout << "Введите ключ: ";
                string user_key;
                getline(cin, user_key);
                key = s2ws(user_key);
                key_str = user_key;
            }
            else if (key_choice == 2) {
                string file_path = input_path("Введите путь к файлу с ключом:\n> ");
                try {
                    wstring key_content = read_text_file(file_path);
                    key = key_content;
                    key_str = ws2s(key);
                    wcout << L"Ключ загружен: " << key << endl;
                }
                catch (const exception& e) {
                    cerr << "Ошибка загрузки ключа: " << e.what() << endl;
                    return;
                }
            }
        }
        else {
            cout << "\nВыберите источник ключа:\n"
                << "1. Ввести вручную\n"
                << "2. Сгенерировать автоматически\n";

            int key_choice = safe_input_int("Ваш выбор: ", 1, 2);

            if (key_choice == 1) {
                cout << "Введите ключ: ";
                string user_key;
                getline(cin, user_key);
                key = s2ws(user_key);
                key_str = user_key;
            }
            else if (key_choice == 2) {
                key = cipher->generateKey();
                key_str = ws2s(key);
                wcout << L"\nСгенерированный ключ: " << key << endl;
                cout << "Текстовый формат: " << key_str << endl;
                key_generated = true;
            }
        }
    }

    vector<unsigned char> binary_data;
    wstring input_text;
    string input_path_value;
    bool is_binary = false;
    size_t original_size = 0;

    if (is_file) {
        input_path_value = input_path("\nВведите путь к исходному файлу:\n> ");

        try {
            if (!file_exists(input_path_value)) {
                throw runtime_error("Файл не существует: " + input_path_value);
            }

            if (cipher->supportsBinary()) {
                ifstream file;
#ifdef _WIN32
                wstring wide_path = s2ws(input_path_value);
                file.open(wide_path, ios::binary | ios::ate);
#else
                file.open(input_path_value, ios::binary | ios::ate);
#endif
                if (!file.is_open()) {
                    throw runtime_error("Не удалось открыть файл: " + input_path_value);
                }
                streamsize size = file.tellg();
                file.close();

                if (size == -1) {
                    throw runtime_error("Не удалось определить размер файла: " + input_path_value);
                }

                const uintmax_t MAX_FILE_SIZE = 1024 * 1024 * 1024;
                if (static_cast<uintmax_t>(size) > MAX_FILE_SIZE) {
                    cout << "Предупреждение: файл очень большой ("
                        << static_cast<double>(size) / (1024.0 * 1024.0) << " MB).\n"
                        << "Рекомендуется использовать файлы до "
                        << MAX_FILE_SIZE / (1024 * 1024) << " MB.\n";
                    cout << "Продолжить? (y/n): ";
                    string response;
                    getline(cin, response);
                    transform(response.begin(), response.end(), response.begin(), ::tolower);
                    if (response != "y" && response != "yes") {
                        return;
                    }
                }

                binary_data = read_binary_file(input_path_value);
                original_size = binary_data.size();
                is_binary = true;

                if (binary_data.empty()) {
                    cout << "Ошибка: файл пуст или не содержит данных.\n";
                    return;
                }
            }
            else {
                input_text = read_text_file(input_path_value);
                if (input_text.empty()) {
                    cout << "Ошибка: файл пуст или не содержит текстовых данных.\n";
                    return;
                }
            }
        }
        catch (const exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
            return;
        }
    }
    else {
        while (true) {
            cout << "\nВведите текст:\n> ";
            string text_str;
            getline(cin, text_str);

            text_str.erase(0, text_str.find_first_not_of(" \t\n\r\f\v"));
            text_str.erase(text_str.find_last_not_of(" \t\n\r\f\v") + 1);

            if (!text_str.empty()) {
                input_text = s2ws(text_str);

                string op_str = (operation == 1) ? "encrypt" : "decrypt";
                string timestamp = to_string(time(nullptr));
                string input_filename = "input_" + op_str + "_" + timestamp + ".txt";

                write_text_file(input_filename, input_text);
                cout << "Исходный текст записан в файл: " << input_filename << endl;
                break;
            }
            cout << "Ошибка: текст не может быть пустым. Попробуйте снова.\n";
        }
    }

    vector<unsigned char> output_binary_data;
    wstring output_text;

    try {
        if (is_binary) {
            if (cipher->supportsBinary()) {
                if (operation == 1) {
                    output_binary_data = cipher->encryptBinary(binary_data, key);
                }
                else {
                    output_binary_data = cipher->decryptBinary(binary_data, key);
                }
            }
            else {
                throw runtime_error("Шифр не поддерживает бинарные данные");
            }
        }
        else {
            if (operation == 1) {
                output_text = cipher->encrypt(input_text, key);
            }
            else {
                output_text = cipher->decrypt(input_text, key);
            }
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка при обработке: " << e.what() << endl;
        return;
    }

    string output_file;
    while (true) {
        output_file = input_path("\nВведите путь для сохранения результата:\n> ", false, true, true);

        if (is_file && operation == 1 && output_file == input_path_value) {
            cout << "Ошибка: нельзя сохранять результат в исходный файл! Это приведет к потере данных.\n";
            continue;
        }

        break;
    }

    if (key_generated) {
        while (true) {
            key_output_file = input_path("Введите путь для сохранения ключа (ОБЯЗАТЕЛЬНО!):\n> ", false, true, true);

            if (key_output_file == output_file) {
                cout << "Ошибка: файл для ключа не может совпадать с файлом результата!\n";
                continue;
            }

            if (is_file && key_output_file == input_path_value) {
                cout << "Ошибка: файл для ключа не может совпадать с исходным файлом!\n";
                continue;
            }

            break;
        }

        ofstream key_file(key_output_file);
        key_file << key_str;
        key_file.close();
        cout << "\n==============================================\n";
        cout << "Ключ сохранен в файле: " << key_output_file << endl;
        cout << "ВАЖНО! Без этого ключа вы НЕ сможете расшифровать данные!\n";
        cout << "==============================================\n\n";
    }

    try {
        if (is_binary) {
            write_binary_file(output_file, output_binary_data);
            cout << "\nБинарные данные успешно обработаны и сохранены в " << output_file << endl;

            if (operation == 2) {
                cout << "\nПроверка целостности:\n";
                cout << " - Оригинальный размер: " << original_size << " байт\n";
                cout << " - Результат: " << output_binary_data.size() << " байт\n";

                if (original_size == output_binary_data.size()) {
                    cout << " - Размеры совпадают: ✓\n";
                }
                else {
                    cout << " - Размеры не совпадают: ⚠\n";
                }
            }
        }
        else {
            write_text_file(output_file, output_text);

            string result_str = ws2s(output_text);
            string label = (operation == 1) ? "ЗАШИФРОВАННЫЙ РЕЗУЛЬТАТ" : "РАСШИФРОВАННЫЙ РЕЗУЛЬТАТ";

            if (result_str.size() > 2000) {
                cout << "\n" << label << " (первые 2000 символов):\n"
                    << result_str.substr(0, 2000)
                    << "\n... [далее опущено, всего " << result_str.size() << " символов] ...\n";
            }
            else {
                cout << "\n" << label << ":\n" << result_str << "\n";
            }
            cout << "\nРезультат сохранен в " << output_file << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Ошибка при сохранении файла: " << e.what() << endl;
    }
}

void cipher_menu(unique_ptr<ICipher>& cipher) {
    wstring cipher_name = cipher->name();
    wcout << L"\n============ " << cipher_name << L" ============\n";

    while (true) {
        cout << "Выберите операцию:\n"
            << "1. Шифровать текст\n"
            << "2. Шифровать файл\n"
            << "3. Расшифровать текст\n"
            << "4. Расшифровать файл\n"
            << "5. Сгенерировать ключ\n"
            << "6. Вернуться в главное меню\n";

        int choice = safe_input_int("Ваш выбор: ", 1, 6);

        if (choice == 6) return;

        try {
            switch (choice) {
            case 1: process_operation(cipher, 1, false); break;
            case 2: process_operation(cipher, 1, true); break;
            case 3: process_operation(cipher, 2, false); break;
            case 4: process_operation(cipher, 2, true); break;
            case 5:
                if (cipher->requiresKey()) {
                    wstring key = cipher->generateKey();
                    wcout << L"\nСгенерированный ключ: " << key << endl;
                    cout << "Текстовый формат: " << ws2s(key) << endl;
                }
                else {
                    cout << "Данный шифр не требует ключа\n";
                }
                break;
            default:
                cout << "Неверный выбор! Попробуйте снова.\n";
            }
        }
        catch (const exception& e) {
            cerr << "Ошибка: " << e.what() << endl;
        }
    }
}

void main_menu() {
    while (true) {
        cout << "\n============ ГЛАВНОЕ МЕНЮ ============\n"
            << "Выберите действие:\n"
            << "1. Работа с шифрами\n"
            << "2. Установщик (компиляция программы)\n"
            << "3. Выход\n";

        int choice = safe_input_int("Ваш выбор: ", 1, 3);

        if (choice == 3) {
            cout << "Завершение работы программы...\n";
            break;
        }

        switch (choice) {
        case 1: {
            while (true) {
                cout << "\n============ ВЫБОР ШИФРА ============\n"
                    << "Выберите шифр:\n"
                    << "1. Шифр Атбаш\n"
                    << "2. Шифр Виженера\n"
                    << "3. Табличная перестановка\n"
                    << "4. Вернуться в главное меню\n";

                int cipher_choice = safe_input_int("Ваш выбор: ", 1, 4);

                if (cipher_choice == 4) break;

                try {
                    auto cipher = CipherLoader::createCipher(cipher_choice);
                    cipher_menu(cipher);
                }
                catch (const exception& e) {
                    cerr << "Ошибка: " << e.what() << endl;
                }
            }
            break;
        }
        case 2:
            installer_menu();
            break;
        }
    }
}