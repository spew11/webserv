#include "ExtendedResponseHeaderAdder.hpp"

ExtendedResponseHeaderAdder::ExtendedResponseHeaderAdder(const HttpRequestMessage & requestMessage, \
    HttpResponseMessage & responseMessage), const LocationConfig & locationConfig, const string & requestBody) \
    : ResponseHeaderAdder(requestMessage, responseMessage, locationConfig, requestBody)

void ExtendedResponseHeaderAdder::executeAll()
{
    ResponseHeaderAdder::executeAll();
}

void ExtendedResponseHeaderAdder::addDateHeader()
{
    time_t now = time(0);
    tm *gmt = gmtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", gmt);
    responseMessage.addHeader("Date", string(buffer));
}