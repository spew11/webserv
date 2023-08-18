#ifndef HTTP_REQUEST_MESSAGE_HPP
#define HTTP_REQUEST_MESSAGE_HPP
#include "HttpMessage.hpp"
#include "Utils.hpp"

using namespace std;

class HttpRequestMessage : public HttpMessage
{
private:
    string httpMethod;
    string requestTarget;
    bool needMoreChunk;
    bool chunked;
    bool connection;
    int errorCode;

public:
    void setFlag();
    HttpRequestMessage(const string httpMethod,
                       const string requestTarget,
                       string serverProtocol,
                       map<string, string> headers,
                       string body,
                       bool needMoreChunk);
    string getHttpMethod() const;
    string getRequestTarget() const;
    bool getNeedMoreChunked() const;
    bool getConnection() const;
    bool isChunked() const;
    int getErrorCode() const;
};

#endif