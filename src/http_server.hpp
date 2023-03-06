#pragma once

#include <functional>

#include "epoll_server.hpp"

namespace http_server {

template <typename RequestType, typename ResponseType>
class HttpServer : public server::EpollServer {
public:
    using HandlerFunc = std::function<void(const RequestType&, ResponseType&)>;

    HttpServer(int port) : server::EpollServer(port) {
    }

    void register_handler(const std::string& uri, const HandlerFunc handler) {
        handlers_[uri] = handler;
    }

    void handle_epollin(server::EpollEvent& epoll, int client_fd, std::string&& buffer) final {
        try {
            RequestType req(std::move(buffer));
            ResponseType res;

            auto it = handlers_.find(req.get_uri());
            if (it != handlers_.end()) {
                it->second(req, res);
            } else {
                res.set_status(http_message::HttpStatus::NotFound);
                res.set_body("NOT FOUND");
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
                    ::close(client_fd);
                    return;
                }
                response_len -= sent_bytes;
            }

            // close connection with HTTP/1.0 version
            if (req.get_version() == http_message::HttpVersion::HTTP_1_0) {
                epoll.remove(client_fd);
                ::close(client_fd);
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void handle_epollout(server::EpollEvent& epoll, int client_fd, std::string&& buffer) final {
    }

private:
    std::map<std::string, HandlerFunc> handlers_;
};

} // namespace http_server
