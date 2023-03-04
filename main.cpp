#include "http_server.hpp"

void helloworld_handler(const Request& req, Response& res){
    res.body_ ="Hello, World!";
}

int main(){
    HttpServer server(38080);
    server.register_handler("/helloworld", helloworld_handler);
    server.run();

    while(true);

    return 0;
}
