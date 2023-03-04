
#include "http_server.hpp"

void HttpServer::register_handler(const std::string& path, const handler_func_t handler) {
    std::cout << "register_handler: " << path << std::endl;
    handlers_[path] = handler;
}

void HttpServer::listener() {
    std::cout << "Started listenning...\n";
    // sockaddr_in client_addr;
    // socklen_t client_addr_len = sizeof(client_addr);
    while (!stop_) {
        int client_fd = accept(sockfd_, NULL, NULL);
        // int client_fd = accept4(sockfd_, (sockaddr*)&client_addr, &client_addr_len, SOCK_NONBLOCK);
        if (client_fd == -1) {
            // std::cerr << "Failed to accept connection\n";
            continue;
        }

        epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = client_fd;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
            std::cerr << "Failed to add epoll fd\n";
            continue;
        }
    }
}

void HttpServer::processor() {
    std::cout << "Started processing...\n";
    while (!stop_) {
        int nbr_events = epoll_wait(epoll_fd_, epoll_events_, MAX_EVENTS, 0);
        for (int i = 0; i < nbr_events; ++i) {
            auto req = parse_request(epoll_events_[i].data.fd);
            Response res;

            auto it = handlers_.find(req.path_);
            if (it != handlers_.end()) {
                it->second(req, res);
            } else {
                res.status_code_ = 404; // TODO: define enum for status code
                res.body_ = "NOT FOUND";
            }

            send_response(epoll_events_[i].data.fd, res);
            close(epoll_events_[i].data.fd);
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

    listen(sockfd_, 5);

    if ((epoll_fd_ = epoll_create1(0)) == -1) {
        throw std::runtime_error("Failed to create epoll");
    }

    listener_ = std::thread(&HttpServer::listener, this);
    worker_ = std::thread(&HttpServer::processor, this);
}

Request HttpServer::parse_request(int client_fd) {
    char buffer[1024];
    ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
    if (n == -1) {
        std::cerr << "Failed to receive request\n";
        return Request("");
    }
    buffer[n] = '\0';
    
    auto reqstr = std::string(buffer);
    auto req = Request(reqstr);
    return std::move(req);
}

void HttpServer::send_response(int client_fd, const Response& res){
    auto response_buffer = res.to_string();
    auto n = send(client_fd, response_buffer.c_str(), response_buffer.length(), 0);
    if (n == -1) {
        std::cerr << "Failed to send response\n";
    }
}
