#include <stdio.h>
#include "SocketServer.hpp"

#define PORT 4717

int main(){


    server_params srv_params;
    srv_params.port = PORT;

    auto server = new SocketServer(srv_params);
    server->start();
    server->exit();
}