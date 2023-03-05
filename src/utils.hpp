#pragma once

#include <string>

static const std::string white_space_chars = " \t\n\r\f\v";

inline std::string ltrim(std::string s, const std::string& chars = white_space_chars) {
    s.erase(0, s.find_first_not_of(chars));
    return s;
}

inline std::string rtrim(std::string s, const std::string& chars = white_space_chars) {
    s.erase(s.find_last_not_of(chars) + 1);
    return s;
}

inline std::string trim(std::string s, const std::string& chars = white_space_chars) {
    return ltrim(rtrim(s, chars), chars);
}
