#include "SocketServer.hpp"
#include <iostream>
#include <string>
#include <pthread.h>
#include <netinet/in.h> // for sockaddr_in
#include <sys/types.h>  // for socket
#include <sys/socket.h> // for socket
#include <unistd.h>
#include <string.h>
#include <netinet/tcp.h>
#include <cstring>
#include <signal.h>
#include <arpa/inet.h>
#include <vector>

#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024



void *clntThread(void *arg);

int SocketServer::start()
{
    listen(this->srv_socket, LENGTH_OF_LISTEN_QUEUE);
    std::cout << "Started." << '\n';
    client_params clnt_params;
    clnt_params.server = (void *)this;
    int id;
    do
    {
        struct sockaddr_in clnt_addr;
        socklen_t length = sizeof(clnt_addr);
        int clnt_socket = accept(srv_socket, (struct sockaddr *)&clnt_addr, &length);

        clnt_params.address = inet_ntoa(clnt_addr.sin_addr);
        clnt_params.port = ntohs(clnt_addr.sin_port);
        clnt_params.socket = clnt_socket;
        
        this->clnt_list.push_back(clnt_params);

        id = clnt_list.size() - 1;
        std::cout << "Client " << clnt_list[id].address << ":" << clnt_list[id].port << " connected." << '\n';
        // update the clnt list

        // new thread
        pthread_t process;
        pthread_create(&process, nullptr, clntThread, (void *)(&clnt_list[id]));
    } while (true);
}

SocketServer::SocketServer(server_params srv_params)
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(srv_params.port);
    srv_socket = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&(server_addr.sin_zero), 8);
    bind(srv_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    std::cout << "Initialized the server." << '\n';
}

SocketServer::~SocketServer()
{
    this->exit();
}

int SocketServer::exit()
{
    // destory clients' threads then
    close(srv_socket);
    std::cout << "Exited." << '\n';
    return 0;
}

void *clntThread(void *arg)
{
    client_params clnt_params = *((client_params *)arg);
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    socklen_t length;
    do
    {
        length = recv(clnt_params.socket, buffer, BUFFER_SIZE, 0);
        printf("%s", buffer);
        // check if alive
    } while (length > 0);
    // delete from the clnt list
    std::cout << clnt_params.address << ":" << clnt_params.port << " left." << '\n';
    return 0;
}