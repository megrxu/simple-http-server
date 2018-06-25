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
  std::map<std::string, std::string> header;

  std::string response_str;
  std::string next_raw_str;
  HTTPResponse();
  void toString();
};

class HTTPRequest
{
public:
  std::string method;
  std::string path;
  std::string root;
  std::string request_str;
  std::map<std::string, std::string> header;

  HTTPRequest(std::string request_str);

  HTTPResponse getResponse(std::string raw_str);
};

#endif // !HTTP_HANDLER_H