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

#include "epoll_event.hpp"

class EpollServer {
public:
    EpollServer(int port);
    virtual ~EpollServer();

    void init();
    void worker(int worker_id);
    void run();

    virtual void handle_epollin(EpollEvent& epoll, int client_fd, std::string&& buffer) = 0;
    virtual void handle_epollout(EpollEvent& epoll, int client_fd, std::string&& buffer) = 0;

private:
    const int max_workers_;
    static constexpr int MAX_BACKLOG_ = 10000;

    int server_fd_;
    int port_;

    bool stop_ = false; // TODO: use it to graceful shutdown
    std::vector<std::thread> workers_;
    std::vector<EpollEvent> epoll_events_;
    EpollEvent server_epoll_;
};
