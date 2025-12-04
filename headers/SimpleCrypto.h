#pragma once
#include <string>

namespace SimpleCrypto {
    std::string simpleEncrypt(const std::string& data);
    std::string simpleDecrypt(const std::string& data);
    bool compareEncrypted(const std::string& input, const std::string& encrypted);
}