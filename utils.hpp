#pragma once

#include <string>

static const std::string white_space_chars = " \t\n\r\f\v";

std::string ltrim(std::string s, const std::string& chars = white_space_chars);

std::string rtrim(std::string s, const std::string& chars = white_space_chars);

std::string trim(std::string s, const std::string& chars = white_space_chars);
