#ifndef HTTP_REQUEST_MESSAGE_HPP
# define HTTP_REQUEST_MESSAGE_HPP
#include "HttpMessage.hpp"
#include "Utils.hpp"

class HttpRequestMessage : public HttpMessage
{
    private:
        string httpMethod;
        string requestTarget;
        bool chunkedFlag;
        bool last;
        void parseRequestMessage(const string &requestMessage);
    public:
        HttpRequestMessage(const string &requestMessage);
        string getHttpMethod() const;
        string getRequestTarget() const;
        bool getChunkedFlag() const;
};

#endif