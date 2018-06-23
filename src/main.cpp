#include "SocketServer.hpp"
#include "HTTPHandler.hpp"

#define PORT 4717

int main()
{
    server_params srv_params;
    srv_params.port = PORT;

    auto server = new SocketServer(srv_params);
    server->setHandler(&HTTP_handler);
    server->start();
    server->exit();
}