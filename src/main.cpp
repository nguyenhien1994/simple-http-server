#include "http_request.hpp"
#include "http_response.hpp"
#include "http_server.hpp"

void helloworld_handler(const Request& req, Response& res){
    res.headers_["Content-Type"] = "text/html";
    res.body_ = R"(<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>Hello!</title>
    </head>
    <body>
        <h1>Hello World!</h1>
        <p>This is a simple paragraph.</p>
    </body>
</html>
    )";
    res.status_code_ = 200;
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
