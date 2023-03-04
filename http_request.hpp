#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <map>

#include "utils.hpp"

struct Request {
    Request(const std::string &raw_string) {
        size_t start = 0, end = 0;

        // parse the first line
        end = raw_string.find("\r\n", start);
        if (end == std::string::npos) {
            throw std::invalid_argument("Invalid request string");
        }
        std::istringstream iss(raw_string.substr(start, end - start));
        iss >> method_ >> path_ >> version_;
        if (!iss.good() && !iss.eof()) {
            throw std::invalid_argument("Invalid request string");
        }

        // parse headers
        start = end + 2; // skip \r\n
        end = raw_string.find("\r\n\r\n", start);
        if (end != std::string::npos) {
            iss.clear();
            iss.str(raw_string.substr(start, end - start));

            std::string line;
            std::string key, value;
            while (std::getline(iss, line)) {
                std::istringstream ss(line);
                std::getline(ss, key, ':');
                std::getline(ss, value);

                headers_[trim(key)] = trim(value);
            }
        }

        // parse body
        start = end + 4; // skip \r\n\r\n
        end = raw_string.length();
        if (start < end) {
            body_ = raw_string.substr(start, end - start);
        }

        // print_debug();
    }

    void print_debug() const {
        std::cout << method_ << " " << path_ << " " << version_ << std::endl;
        for (const auto& kv : headers_) {
            std::cout << kv.first << ": " << kv.second << std::endl;
        }
        std::cout << body_ << std::endl;
    }

    std::string method_;
    std::string path_;
    std::string version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};

