#pragma once

#include <string>
#include <map>

struct Response {
    Response(int status_code = 200) : status_code_(status_code) {}
    int status_code_;
    std::map<std::string, std::string> headers_;
    std::string body_;

    // TODO: define enum for status_code
    std::string to_string(int status_code) const {
        switch(status_code) {
            case 200:
            case 204:
                return "OK";
            case 404:
                return "Not Found";
            default:
                return "";
        }
    }

    std::string to_string() const {
        std::ostringstream oss;

        oss << "HTTP/1.1 " << status_code_ << ' ' << to_string(status_code_) << "\r\n";
        for (const auto& kv : headers_) {
            oss << kv.first << ": " << kv.second << "\r\n";
        }

        if (!body_.empty()) {
            oss << "Content-Length: " << body_.size() << "\r\n\r\n";
            oss << body_;
        }

        return oss.str();
    }
};
