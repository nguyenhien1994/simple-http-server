#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>

#include <cstring>
#include <iostream>
#include <functional>
#include <thread>
#include <vector>

#include "epoll_server.hpp"

template <typename RequestType, typename ResponseType>
class HttpServer : public EpollServer {
public:
    using HandlerFunc = std::function<void(const RequestType&, ResponseType&)>;

    HttpServer(int port) : EpollServer(port) {
    }

    void register_handler(const std::string& path, const HandlerFunc handler) {
        std::cout << "register_handler: " << path << std::endl;
        handlers_[path] = handler;
    }

    void handle_epollin(EpollEvent& epoll, int client_fd, std::string&& buffer) {
        try {
            RequestType req(std::move(buffer));
            ResponseType res;

            auto it = handlers_.find(req.path_);
            if (it != handlers_.end()) {
                it->second(req, res);
            } else {
                res.status_code_ = 404; // TODO: define enum for status code
                res.body_ = "NOT FOUND";
            }

            // TODO: send response via epoll EPOLLOUT?
            auto response_buffer = res.to_string();
            auto response_len = response_buffer.length();

            while (response_len > 0) {
                auto sent_bytes = send(client_fd, response_buffer.c_str(), response_buffer.length(), 0);
                if (sent_bytes < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        // retry
                        continue;
                    }
                    epoll.remove(client_fd);
                    close(client_fd);
                    return;
                }
                response_len -= sent_bytes;
            }

            if (req.version_ == "HTTP/1.0") {
                // std::cout << "req.version_: " << req.version_ << std::endl;
                epoll.remove(client_fd);
                close(client_fd);
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void handle_epollout(EpollEvent& epoll, int client_fd, std::string&& buffer) {
    }


private:
    std::map<std::string, HandlerFunc> handlers_;
};
