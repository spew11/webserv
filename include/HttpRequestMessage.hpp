#ifndef HTTP_REQUEST_MESSAGE_HPP
# define HTTP_REQUEST_MESSAGE_HPP
#include "HttpMessage.hpp"
#include "Utils.hpp"

class HttpRequestMessage : public HttpMessage
{
    private:
        string method;
        string uri;
        void requestMessageParser(const string &requestMessage);
        bool chunkedFlag;
    public:
        HttpRequestMessage(const string &requestMessage);
        string getMethod() const;
        string getUri() const;
        int getChunkedFlag() const;
};

#endif