#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <cstring>

class EpollWrapper {
public:
    EpollWrapper() {
        epoll_fd_ = epoll_create1(0);
        max_events_ = 4096;
        if (epoll_fd_ == -1) {
            throw std::runtime_error("Failed to create epoll");
        }
        events_.resize(max_events_);
    }

    ~EpollWrapper() {
        close(epoll_fd_);
    }

    void add(int fd, uint32_t events) {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = events;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) < 0) {
            throw std::runtime_error("Failed to add fd to epoll");
        }
    }

    void modify(int fd, uint32_t events) {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = events;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event) < 0) {
            throw std::runtime_error("Failed to modify fd to epoll");
        }
    }

    void remove(int fd) {
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr) < 0) {
            throw std::runtime_error("Failed to remove fd to epoll");
        }
    }

    std::vector<struct epoll_event> wait() {
        int num_events = epoll_wait(epoll_fd_, events_.data(), max_events_, -1);
        std::vector<struct epoll_event> result(events_.begin(), events_.begin() + num_events);
        return std::move(result);
    }

private:
    int epoll_fd_;
    int max_events_;
    std::vector<struct epoll_event> events_;
};