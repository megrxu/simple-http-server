#include "SocketServer.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <map>

#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024

void *clntThread(void *arg);

int SocketServer::start()
{
    listen(this->srv_socket, LENGTH_OF_LISTEN_QUEUE);
    client_params clnt_params;
    clnt_params.server = (void *)this;
    int id;
    do
    {
        // init address params
        struct sockaddr_in clnt_addr;
        socklen_t length = sizeof(clnt_addr);
        int clnt_socket = accept(srv_socket, (struct sockaddr *)&clnt_addr, &length);

        // update client list
        clnt_params.server = (void *)this;
        clnt_params.address = inet_ntoa(clnt_addr.sin_addr);
        clnt_params.port = ntohs(clnt_addr.sin_port);
        clnt_params.socket = clnt_socket;
        id = clnt_params.id = ++this->clnt_num;
        this->clnt_cnt++;
        this->clnt_list[id] = clnt_params;

        // new thread
        pthread_t process;
        pthread_create(&process, nullptr, clntThread, (void *)(&clnt_list[id]));
    } while (true);

    return 0;
}

SocketServer::SocketServer(server_params srv_params)
{
    // set bind props
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(srv_params.port);
    srv_socket = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&(server_addr.sin_zero), 8);

    // bing
    bind(srv_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // init variables
    this->clnt_cnt = 0;
    this->clnt_num = 0;

    // log
    // std::cout << "Initialized the server." << '\n';
}

SocketServer::~SocketServer()
{
    this->exit();
}

int SocketServer::exit()
{
    // destory clients' threads then
    close(this->srv_socket);
    // std::cout << "Exited." << '\n';
    return 0;
}

int SocketServer::setHandler(int (*thepHandler)(client_params clnt_params, std::string request_str))
{
    this->pHandler = thepHandler;
    return 0;
}

void *clntThread(void *arg)
{
    client_params clnt_params = *((client_params *)arg);
    char buffer[BUFFER_SIZE + 1];
    bzero(buffer, BUFFER_SIZE + 1);
    int current_cnt;
    socklen_t length;
    std::string request_str;

    do
    {
        request_str = "";
        bzero(buffer, BUFFER_SIZE + 1);
        length = recv(clnt_params.socket, buffer, BUFFER_SIZE, 0);
        buffer[BUFFER_SIZE] = 0;
        request_str = buffer;

        (*(((SocketServer *)(clnt_params.server))->pHandler))(clnt_params, request_str);
        // check if alive
    } while (length > 0);

    // delete from the clnt list
    ((SocketServer *)(clnt_params.server))->clnt_list.erase(clnt_params.id);
    current_cnt = --(((SocketServer *)(clnt_params.server))->clnt_cnt);
    return 0;
}