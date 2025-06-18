#include "cipher.h"
#include "atbash.h"
#include "vigenere.h"
#include "table_transposition.h"
#include <memory>
#include <stdexcept>

using namespace std;

CIPHERLIB_API unique_ptr<ICipher> CipherLoader::createCipher(int type) {
    switch (type) {
    case 1: return make_unique<AtbashCipher>();
    case 2: return make_unique<VigenereCipher>();
    case 3: return make_unique<TableTranspositionCipher>();
    default: throw invalid_argument("Неизвестный тип шифра");
    }
}