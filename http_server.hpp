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

#include "http_request.hpp"
#include "http_response.hpp"
#include "epoll_wrapper.hpp"

using HandlerFunc = std::function<void(const Request&, Response&)>;

class HttpServer {
public:
    HttpServer(int port);

    void register_handler(const std::string& path, const HandlerFunc handler);
    void listener();
    void processor(int);
    void run();
    void handle_request(EpollWrapper& epoll, int client_fd, Request&& req);

private:
    static constexpr int MAX_WORKER_ = 24;
    static constexpr int MAX_BACKLOG_ = 10000;

    int sockfd_;
    int port_;

    std::map<std::string, HandlerFunc> handlers_;
    bool stop_ = false; // TODO: use it to graceful shutdown
    std::thread listener_;
    std::vector<std::thread> workers_;
    std::vector<EpollWrapper> epoll_events_;
};
