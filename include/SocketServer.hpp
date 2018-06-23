#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <sys/types.h>
#include <pthread.h>
#include <map>
#include <string>

struct server_params
{
    short port;
    int srv_socket;
};

struct client_params
{
    void *server;
    std::string address;
    short port;
    int socket;
    int id;
};

class SocketServer
{
  public:
    int srv_socket;
    std::map<int, client_params> clnt_list;
    int clnt_cnt; //count
    int clnt_num; //next id
    int (*pHandler)(client_params clnt_params, std::string request);

    int start();
    int exit();
    int setHandler(int (*thepHandler)(client_params clnt_params, std::string request));

    explicit SocketServer(server_params srv_params);
    ~SocketServer();
};

#endif // !SOCKET_SERVER