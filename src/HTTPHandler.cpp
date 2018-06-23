#include "SocketServer.hpp"
#include "HTTPHandler.hpp"
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <iostream>
#include <sys/socket.h>
#include <fstream>
#include <streambuf>
#include <time.h>
#include <iomanip>
#include <sstream>

#define ROOT "./html"

std::ifstream t("file.txt");
std::string str((std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());

#define BUFFER_SIZE 1024

// to get the request str from the buffer
int HTTP_handler(client_params clnt_params, std::string raw_str)
{
    std::size_t index;
    std::string request_str;
    while (raw_str.length() && (bool)(index = raw_str.find("\r\n\r\n")))
    {
        request_str = raw_str.substr(0, index);
        std::cout << request_str << "\n\n";
        if (index + 4 < raw_str.length())
        {
            raw_str = raw_str.substr(index + 4);
        }
        else
        {
            raw_str = "";
        }
        HTTPRequest request = HTTPRequest(request_str);
        if (request.method != "NOTHANDLED")
        {
            HTTPResponse response = request.getResponse();
            send(clnt_params.socket, response.response_str.data(), response.response_str.length(), 0);
        }
    };
    return 0;
}

HTTPResponse HTTPRequest::getResponse()
{
    HTTPResponse response = HTTPResponse();

    // type
    if (this->path.back() == '/')
    {
        this->path += "index.html";
    }
    std::size_t index = this->path.find_last_of(".");
    std::string ext;
    if (index + 1 < this->path.length())
    {
        ext = this->path.substr(index + 1);
    }
    if (ext == "html")
    {
        response.type = "text/html";
    }
    else if (ext == "jpg" || ext == "jpeg" || ext == "JPG" || ext == "JPEG")
    {
        response.type = "image/jpg";
    }
    else
    {
        response.type = "text/txt";
    }

    std::ifstream file(ROOT + this->path);
    if (file)
    {
        std::string file_str((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        response.content = file_str;
    }
    else
    {
        response.status = "404 Not Found";
        response.content = "";
    }

    response.toString();
    return response;
}

HTTPResponse::HTTPResponse()
{
    this->status = "200 OK";

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    std::stringstream time_buffer;
    time_buffer << std::put_time(timeinfo, "%a %b %d %H:%M:%S %Y");

    this->datetime = "Sun, 18 Oct 2009 08:56:53 GMT";
    this->datetime = time_buffer.str();
    this->server = "NaiveHTTPServer/0.0.1";
    this->mtime = "Sat, 20 Nov 2004 07:16:26 GMT";
    this->mtime = time_buffer.str();
    this->content = "<html><body><h1>It works!</h1></body></html>";
    this->length = this->content.size();
    this->type = "text/html";
}

void HTTPResponse::toString()
{
    std::map<std::string, std::string> header;
    header["Date"] = this->datetime;
    header["Server"] = this->server;
    header["Last-Modified"] = this->mtime;
    header["Content-Length"] = std::to_string(this->content.size());
    header["Content-Type"] = this->type;

    //status
    this->response_str = "HTTP/1.1 " + this->status + "\r\n";

    // other
    std::string text = "";
    std::map<std::string, std::string>::iterator iter;
    iter = header.begin();
    while (iter != header.end())
    {
        text += iter->first + ": " + iter->second + "\r\n";
        iter++;
    }
    this->response_str += text;

    // content
    this->response_str += "\r\n" + this->content;
}

HTTPRequest::HTTPRequest(std::string request_str)
{
    request_str = request_str;
    size_t index;
    std::vector<std::string> lines;
    while (true)
    {
        index = request_str.find("\r\n");
        if (index > request_str.length() || index == 0)
        {
            break;
        }
        lines.push_back(request_str.substr(0, index));
        request_str = request_str.substr(index + 2);
    };

    // method
    if (lines.size() == 0)
    {
        lines.push_back("NOTHANDLED");
    }
    if (lines[0].substr(0, 3) == "GET")
    {
        this->method = "GET";
        lines[0] = lines[0].substr(4);
    }
    else if (lines[0].substr(0, 4) == "POST")
    {
        this->method = "POST";
        lines[0] = lines[0].substr(5);
    }
    else
    {
        this->method = "NOTHANDLED";
    }

    // path
    if (this->method != "NOTHANDLED")
    {
        this->path = lines[0].substr(0, lines[0].find(" "));
    }
}
