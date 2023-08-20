#ifndef HTTP_RESPONSE_MESSAGE_HPP
#define HTTP_RESPONSE_MESSAGE_HPP
#include "ResponseStatusManager.hpp"
#include "HttpMessage.hpp"
#include "Utils.hpp"

using namespace std;

class HttpResponseMessage : public HttpMessage
{
private:
    int statusCode;
    string reasonPhrase;

public:
    HttpResponseMessage();
    int getStatusCode() const;
    string getReasonPhrase() const;
    void setStatusCode(int statusCode);
    void setReasonPhrase(const string &reasonPhrase);
    void setBody(const string &body);
    void addHeader(string headerType, string headerValue);
    void setServerProtocol(const string &serverProtocol);
};
#endif