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
#include <unistd.h>
#include <time.h>
#include <iomanip>
#include <sstream>

#define ROOT "./html"
#define SERVER_NAME "NaiveHTTPServer/0.0.1"

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
            HTTPResponse response = request.getResponse(raw_str);
            send(clnt_params.socket, response.response_str.data(), response.response_str.length(), 0);
            raw_str = response.next_raw_str;
        }
    };
    return 0;
}

HTTPResponse HTTPRequest::getResponse(std::string raw_str)
{
    HTTPResponse response = HTTPResponse();

    if (this->method == "GET")
    {
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
            response.type = "image/jpeg";
        }
        else if (ext == "png" || ext == "PNG")
        {
            response.type = "image/png";
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
            response.content = "404 Not Found";
        }
    }
    else if (this->method == "POST")
    {
        if (this->path == "/dopost")
        {
            if (header["Content-Length"].size())
            {
                size_t index = std::stoi(header["Content-Length"]);
                std::string data = raw_str.substr(0, index);
                if ((index + 1) < raw_str.size())
                {

                    raw_str = raw_str.substr(index + 1);
                }
                else
                {
                    raw_str = "";
                }
                response.status = "200 OK";
                response.type = "text/html";
                if (data == "login=3150104717&pass=4717")
                {
                    response.content = "<html><body>Login Successful!</body></html>";
                }
                else
                {
                    response.content = "<html><body>Login Failed!</body></html>";
                }
            }
            else
            {
                response.status = "403 Forbidden";
                response.content = "403 Forbidden";
            }
        }
        else
        {
            response.status = "404 Not Found";
            response.content = "404 Not Found";
        }
    }

    response.next_raw_str = raw_str;

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

    this->datetime = time_buffer.str();
    this->server = SERVER_NAME;
    this->mtime = time_buffer.str();
    this->content = "200 OK";
    this->length = this->content.size();
    this->type = "text/plain";
}

void HTTPResponse::toString()
{
    header["Date"] = this->datetime;
    header["Server"] = this->server;
    header["Last-Modified"] = this->mtime;
    header["Content-Length"] = std::to_string(this->content.size());
    header["Content-Type"] = this->type;

    //status
    this->response_str = "HTTP/1.1 " + this->status + "\r\n";

    // other header
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
    this->response_str += "\r\n" + this->content + "\r\n\r\n";
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

    // other header
    int i = 1;
    int length = lines.size();
    while (i < length)
    {
        index = lines[i].find(":");
        if (index < lines[i].length() && index != 0)
        {
            this->header[lines[i].substr(0, index)] = lines[i].substr(index + 2);
        }
        i++;
    }

    // path
    if (this->method != "NOTHANDLED")
    {
        this->path = lines[0].substr(0, lines[0].find(" "));
    }
}
