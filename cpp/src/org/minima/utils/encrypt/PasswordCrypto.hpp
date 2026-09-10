#pragma once
#include "GenerateKey.hpp"
namespace minima {
class PasswordCrypto {
public:
    static MiniData encryptPassword(const std::string& password,const MiniData& data);
    static MiniData decryptPassword(const std::string& password,const MiniData& encrypted);
};
}
