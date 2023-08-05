#include "ResponseHeaderAdder.hpp"

ResponseHeaderAdder::ResponseHeaderAdder(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessage, const LocationConfig & locationConfig, const string & requestBody)
    : requestMessage(requestMessage), responseMessage(responseMessage), locationConfig(locationConfig), requestBody(requestBody){}


void ResponseHeaderAdder::executeAll()
{
    addContentLengthHeader(locationConfig.getType(requestMessage.getFilename()));
    addContentLengthHeader(to_string(requestBody.length())); // ASCII로 이루어진 문자만 제대로 셀 수 있음. 
}

//void ResponseHeaderAdder::addCacheControlHeader(){}

void ResponseHeaderAdder::addContentTypeHeader(const string & contentType)
{
    responseMessage.addHeader("Content-Type", contentType);
}

void ResponseHeaderAdder::addContentLengthHeader(const string & requestBody)
{
    responseMessage.addHeader("Content-Length", to_string(requestBody.length()));
}