#include "http_request.hpp"
#include "http_response.hpp"
#include "http_server.hpp"

void helloworld_handler(const Request& req, Response& res){
    res.body_ = "Hello, World!";
}

int main() {
    auto server = HttpServer<Request, Response>(38080);
    server.register_handler("/helloworld", helloworld_handler);

    server.start();

    std::cout << "Enter [quit] to graceful shutdown the server!" << std::endl;

    std::string cmd;
    while (std::cin >> cmd, cmd != "quit") {}

    server.stop();

    return 0;
}
