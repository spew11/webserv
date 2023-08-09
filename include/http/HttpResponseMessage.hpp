#ifndef HTTP_RESPONSE_MESSAGE_HPP
#define HTTP_RESPONSE_MESSAGE_HPP
#include "HttpMessage.hpp"
#include <sstream>

class HttpResponseMessage : public HttpMessage
{
    private:
        int statusCode;
        string reasonPhrase;
    public:
        int getStatusCode() const;
        string getReasonPhrase() const;
        void setStatusCode(const int &statusCode);
        void setReasonPhrase(const string &reasonPhrase);
        void setBody(const string &body);
        void addHeader(string headerType, string headerValue);
        void setServerProtocol(const string &serverProtocol);
        string toString() const;
};
#endif