#include "ResponseHeaderAdder.hpp"

ResponseHeaderAdder::ResponseHeaderAdder(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessage),
    const LocationConfig & locationConfig, const string & requestBody)
    : responseMessage(responseMessage), responseMessage(responseMessage), locationConfig(locationConfig), requestBody(requestBody){}


virtual void ResponseHeaderAdder::executeAll()
{
    addContentLengthHeader(locationConfig.getType());
    addContentLengthHeader(requestBody.length());
}

//void ResponseHeaderAdder::addCacheControlHeader(){}

void ResponseHeaderAdder::addContentTypeHeader(const string & contentType)
{
    responseMessage.addHeader("Content-Type", contentType);
}

void ResponseHeaderAdder::addContentLengthHeader(const string & requestBody)
{
    responseMessage.addHeader("Content-Length", requestBody.length());
}