#pragma once
#include <core/types.h>
#include <string>
#include <vector>
namespace futureio {

    std::string bin2hex(const std::string& data);

    std::string hex2bin(const std::string &hex);

    // Encode visible characters with AES
    // return hex string
    std::string aes_encode(std::string const& key, std::string const& plain);

    // Decode cipher encoded by aes_encode 
    // cipher: hex string which generated by aes_encode
    // return plain text of visible characters
    std::string aes_decode(std::string const& key, std::string const& cipher);

    // execute system command and return the output
    // return the output
    std::string exec_cmd(std::string const& cmd);
}

