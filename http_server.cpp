
#include "http_server.hpp"

HttpServer::HttpServer(int port) : port_(port) {
    workers_.resize(MAX_WORKER_);
    epoll_events_.resize(MAX_WORKER_);

    // TODO: use first instance for Server to accept new client connection
}

void HttpServer::register_handler(const std::string& path, const HandlerFunc handler) {
    std::cout << "register_handler: " << path << std::endl;
    handlers_[path] = handler;
}

void HttpServer::listener() {
    std::cout << "Started listenning...\n";
    int worker_id = 0;
    while (!stop_) {
        int client_fd = accept(sockfd_, NULL, NULL);
        if (client_fd == -1) {
            continue;
        }

        try {
            epoll_events_[worker_id].add(client_fd, EPOLLIN);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        worker_id++;
        if (worker_id == MAX_WORKER_) {
            worker_id = 0;
        }
    }
}

void HttpServer::handle_request(EpollWrapper& epoll, int client_fd, Request&& req) {
    try {
        Response res;

        auto it = handlers_.find(req.path_);
        if (it != handlers_.end()) {
            it->second(req, res);
        } else {
            res.status_code_ = 404; // TODO: define enum for status code
            res.body_ = "NOT FOUND";
        }

        // TODO: send response via epoll EPOLLOUT
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
            epoll.remove(client_fd);
            close(client_fd);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void HttpServer::processor(int worker_id) {
    std::cout << "Started processing...\n";
    auto epoll = epoll_events_[worker_id];
    while (!stop_) {
        auto events = epoll.wait();
        for (const auto& event : events) {
            if (event.events & EPOLLIN) {
                char buffer[4096];
                ssize_t received_bytes = recv(event.data.fd, buffer, sizeof(buffer), 0);
                if (received_bytes <= 0) {
                    // std::cerr << "Client close connection or error\n";
                    // Client close connection or error
                    epoll.remove(event.data.fd);
                    close(event.data.fd);
                    continue;
                }
                buffer[received_bytes] = '\0';
                handle_request(epoll, event.data.fd, Request(std::string(buffer)));
            } else if (event.events & EPOLLOUT) {
                // std::cout << "event EPOLLOUT\n";
            } else if (event.events & (EPOLLHUP | EPOLLERR)) {
                epoll.remove(event.data.fd);
                close(event.data.fd);
            } else {
                std::cout << "something unexpected\n";
            }
        }
    }
}

void HttpServer::run() {
    if ((sockfd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    int opt = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        throw std::runtime_error("Failed to set socket options");
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_);

    if (bind(sockfd_, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        std::cerr << "Failed to bind socket, errno: " << std::strerror(errno) << std::endl;
        throw std::runtime_error("Failed to bind socket");
    }

    listen(sockfd_, MAX_BACKLOG_);

    for (int i = 0; i < MAX_WORKER_; ++i) {
        workers_[i] = std::thread(&HttpServer::processor, this, i);
    }

    listener_ = std::thread(&HttpServer::listener, this);
}
