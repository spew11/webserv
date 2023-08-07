#include "HttpResponseMessage.hpp"


int HttpResponseMessage::getStatusCode() const
{
    return statusCode;
}

string HttpResponseMessage::getReasonPhrase() const
{
    return reasonPhrase;
}

void HttpResponseMessage::setStatusCode(const int &statusCode)
{
    this->statusCode = statusCode;
}

void HttpResponseMessage::setReasonPhrase(const string &reasonPhrase)
{
    this->reasonPhrase = reasonPhrase;
}

void HttpResponseMessage::setBody(const string &body)
{
    this->body = body;
}

void HttpResponseMessage::addHeader(string headerType, string headerValue)
{
    headers.insert(make_pair(headerType, headerValue));
}

string HttpResponseMessage::toString() const
{
    map<string, string>::const_iterator it = headers.begin();
    string response = getServerProtocol() + " " + to_string(getStatusCode()) + "\r\n";
    
    for (it = headers.begin(); it != headers.end(); it++) {
        if (it->first != "") {
            response += it->first + ": " + it->second + "\r\n";
        }
    }
    response += "\r\n";
    response += getBody();
    return response;
}
