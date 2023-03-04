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

#include "http_request.hpp"
#include "http_response.hpp"

using handler_func_t = std::function<void(const Request&, Response&)>;

class HttpServer {
public:
    HttpServer(int port) : port_(port) {}

    void register_handler(const std::string& path, const handler_func_t handler);
    void listener();
    void processor();
    void run();
    Request parse_request(int client_fd);
    void send_response(int client_fd, const Response& res);

private:
    static constexpr int MAX_EVENTS = 10000;
    int sockfd_;
    int port_;
    std::map<std::string, handler_func_t> handlers_;
    bool stop_ = false; // TODO: use it to graceful shutdown
    int epoll_fd_;
    std::thread listener_;
    std::thread worker_;
    epoll_event epoll_events_[MAX_EVENTS];
};

