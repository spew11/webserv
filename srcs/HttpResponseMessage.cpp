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

void HttpResponseMessage::addHeader(const string headerType, const string headerValue)
{
    headers.insert(headerType, headerValue);
}

string HttpResponseMessage::toString() const
{
    string response = getServerProtocol() + " " + getStatusCode() + "\r\n";
    for (pair<string, string> &pair : headers) {
        if (pair.first != "") {
            response += pair.first + ": " + pair.second + "\r\n";
        }
    }
    response += "\r\n";
    response += getBody();
    return response;
}
