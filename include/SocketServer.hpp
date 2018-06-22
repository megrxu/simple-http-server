#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <sys/types.h>
#include <set>
#include <map>
#include <functional>
#include <pthread.h>
#include <vector>

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
};

class SocketServer
{
  public:
    int start();
    int exit();
    int srv_socket;
    std::set<pthread_t> pids;
    std::vector<client_params> clnt_list;

    explicit SocketServer(server_params srv_params);
    ~SocketServer();
};

#endif // !SOCKET_SERVER