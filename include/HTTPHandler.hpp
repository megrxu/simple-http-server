#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <string>

int HTTP_handler(client_params clnt_params, std::string request_str);

class HTTPResponse
{
public:
  std::string status;
  std::string content;
  std::string datetime;
  std::string server;
  std::string mtime;
  int length;
  std::string type;
  std::string response_str;
  HTTPResponse();
  void toString();
};

class HTTPRequest
{
public:
  std::string method;
  std::string path;
  std::string host;
  std::string root;
  std::string request_str;

  HTTPRequest(std::string request_str);

  HTTPResponse getResponse();
};

#endif // !HTTP_HANDLER_H