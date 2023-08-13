#ifndef HTTP_REQUEST_MESSAGE_HPP
# define HTTP_REQUEST_MESSAGE_HPP
#include "HttpMessage.hpp"
#include "Utils.hpp"

class HttpRequestMessage : public HttpMessage
{
    private:
        string httpMethod;
        string requestTarget;
        bool chunked;
        bool connection;
        int parseRequestMessage(const string &requestMessage);
        int errorCode;
    public:
        HttpRequestMessage(const string &requestMessage);
        string getHttpMethod() const;
        string getRequestTarget() const;
        bool getChunked() const;
        bool getConnection() const;
        int getErrorCode() const;
};

#endif