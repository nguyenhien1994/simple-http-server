#include "utils.hpp"

std::string ltrim(std::string s, const std::string& chars) {
    s.erase(0, s.find_first_not_of(chars));
    return s;
}

std::string rtrim(std::string s, const std::string& chars) {
    s.erase(s.find_last_not_of(chars) + 1);
    return s;
}

std::string trim(std::string s, const std::string& chars) {
    return ltrim(rtrim(s, chars), chars);
}
