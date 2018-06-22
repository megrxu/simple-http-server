#include <stdio.h>
#include "SocketServer.hpp"

int main(){
    SocketServer server;
    server_params params;
    server.start(params);
    printf("Test.");
}