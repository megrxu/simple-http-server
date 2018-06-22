#ifndef SOCKET_SERVER
#include <iostream>
#include <string>

struct server_params
{
    std::string address;
    short port;
    long serverId;
};

class SocketServer
{
  public:
    void start(server_params params);
};

#endif // !SOCKET_SERVER