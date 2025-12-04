#include "../headers/SimpleCrypto.h"
#include <string>
#include <algorithm>

namespace SimpleCrypto {
    std::string simpleEncrypt(const std::string& data) {
        std::string result = data;
        for (size_t i = 0; i < result.length(); ++i) {
            
            result[i] = (result[i] + 13) ^ 0xAA;
        }
        return result;
    }

    std::string simpleDecrypt(const std::string& data) {
        std::string result = data;
        for (size_t i = 0; i < result.length(); ++i) {
            
            result[i] = (result[i] ^ 0xAA) - 13;
        }
        return result;
    }

    bool compareEncrypted(const std::string& input, const std::string& encrypted) {
        return simpleEncrypt(input) == encrypted;
    }
}