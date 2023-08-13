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

void HttpResponseMessage::setServerProtocol(const string &serverProtocol)
{
    this->serverProtocol = serverProtocol;
}