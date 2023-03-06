#include "http_request.hpp"
#include "http_response.hpp"
#include "http_server.hpp"

void helloworld_handler(const http_message::HttpRequest& req, http_message::HttpResponse& res){
    const std::string hello_html = R"(<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>Hello!</title>
    </head>
    <body>
        <h1>Hello World!</h1>
        <p>This is a simple paragraph.</p>
    </body>
</html>)";

    res.set_header("Content-Type", "text/html");
    res.set_body(hello_html);
    res.set_status(http_message::HttpStatus::OK);
}

int main() {
    auto server = http_server::HttpServer<http_message::HttpRequest, http_message::HttpResponse>(38080);
    server.register_handler("/helloworld", helloworld_handler);

    server.start();

    std::cout << "Enter [quit] to graceful shutdown the server!" << std::endl;

    std::string cmd;
    while (std::cin >> cmd, cmd != "quit") {}

    server.stop();

    return 0;
}
