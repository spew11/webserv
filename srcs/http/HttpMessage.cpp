#include "HttpMessage.hpp"

HttpMessage::HttpMessage(const string serverProtocol, const map<string, string> headers, const string body)
 : serverProtocol(serverProtocol), headers(headers), body(body)
{

}

string HttpMessage::getBody() const
{
    return body;
}

string HttpMessage::getServerProtocol() const
{
    return serverProtocol;
}

map<string, string> HttpMessage::getHeaders() const
{
    return headers;
}

string HttpMessage::getHeader(const string & headerType) const
{
    if (headers.find(headerType) != headers.end()) {
        return headers.at(headerType);
    }
    return "";
}