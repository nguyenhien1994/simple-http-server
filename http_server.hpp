#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include <cstring>
#include <iostream>
#include <functional>

#include "http_request.hpp"
#include "http_response.hpp"

using handler_func_t = std::function<void(const Request&, Response&)>;

class HttpServer {
public:
    HttpServer(int port) {
        if ((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            throw std::runtime_error("Failed to create socket");
        }

        int opt = 1;
        if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
            throw std::runtime_error("Failed to set socket options");
        }

        sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        if (bind(sockfd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
            std::cerr << "Failed to bind socket, errno: " << std::strerror(errno) << std::endl;
            throw std::runtime_error("Failed to bind socket");
        }

        listen(sockfd_, 5);
    };

    void register_handler(const std::string& path, const handler_func_t handler) {
        std::cout << "register_handler: " << path << std::endl;
        handlers_[path] = handler;
    }

    void run() {
        while (!should_stop_) {
            int fd = accept(sockfd_, NULL, NULL);
            if (fd == -1) {
                std::cerr << "Failed to accept connection\n";
                continue;
            }

            auto req = parse_request(fd);
            Response res;

            auto it = handlers_.find(req.path_);
            if (it != handlers_.end()) {
                it->second(req, res);
            } else {
                res.status_code_ = 404; // TODO: define enum for status code
                res.body_ = "NOT FOUND";
            }

            send_response(fd, res);
            close(fd);
        }
    }

private:
    int sockfd_;
    std::map<std::string, handler_func_t> handlers_;
    bool should_stop_ = false; // TODO: use it to graceful shutdown

    Request parse_request(int sockfd) {
        char buffer[1024];
        ssize_t n = recv(sockfd, buffer, sizeof(buffer), 0);
        if (n == -1) {
            std::cerr << "Failed to receive request\n";
            return Request("");
        }
        buffer[n] = '\0';
        
        auto reqstr = std::string(buffer);
        auto req = Request(reqstr);
        return std::move(req);
    }

    void send_response(int fd, const Response& res){
        auto response_buffer = res.to_string();
        auto n = send(fd, response_buffer.c_str(), response_buffer.length(), 0);
        if (n == -1) {
            std::cerr << "Failed to send response\n";
        }
    }
};

