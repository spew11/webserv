#include "ResponseHeaderAdder.hpp"

ResponseHeaderAdder::ResponseHeaderAdder(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessage, \
    const LocationConfig & locationConfig, string & response, const string & resourcePath)
    : requestMessage(requestMessage), responseMessage(responseMessage), locationConfig(locationConfig), \
        response(response), resourcePath(resourcePath) {}


void ResponseHeaderAdder::executeAll()
{
    string contentType;
    const int statusCode = responseMessage.getStatusCode();
    
    addDateHeader();
    
    if (statusCode == 200) { // 요청한 리소스 반환 성공
        if (locationConfig.isCgi()) {
            parseCgiProduct(response, contentType);
            if (contentType != "") {
                addContentTypeHeader(contentType);
            }
            else {
                addContentTypeHeader(locationConfig.getType(requestMessage.getUri()));
            }
            addContentLengthHeader(response);
        }
        else {
            addContentTypeHeader(locationConfig.getType(requestMessage.getUri()));
            addContentLengthHeader(response);
        }
    }
    else if (statusCode == 201) { // 리소스 생성 성공
        addContentTypeHeader(locationConfig.getType(requestMessage.getUri()));
        addContentLengthHeader(response);
        addLocationHeader(resourcePath); 

    }
    else if (statusCode == 204) { // 리소스 삭제 성공
        //본문 없으면 Content-Type, Content-Length 생략해도됌
        if (response != "") {
            addContentTypeHeader(locationConfig.getType(requestMessage.getUri()));
            addContentLengthHeader(response);
        }
    }
    else if (statusCode == 400) { // 클라이언트가 잘못된 요청을 보냄
        //본문 없으면 Content-Type, Content-Length 생략해도됌
        if (response != "") {
            addContentTypeHeader(locationConfig.getType(requestMessage.getUri()));
            addContentLengthHeader(response);
        }
    }
    else if (statusCode == 404) { // 리소스 찾을 수 없음
        //본문 없으면 Content-Type, Content-Length 생략해도됌
        if (response != "") {
            addContentTypeHeader(locationConfig.getType(requestMessage.getUri()));
            addContentLengthHeader(response);
        }
    }
    else if (statusCode == 405) { // 허락하지 않은 메소드를 받음
        //본문 없으면 Content-Type, Content-Length 생략해도됌
        if (response != "") {
            addContentTypeHeader(locationConfig.getType(requestMessage.getUri()));
            addContentLengthHeader(response);
            addAllowHeader(locationConfig.getAcceptMethods());
        }
    }
    else if (statusCode == 500) { // 서버 에러
        //왠만하면 본문을 만들어서 주는게 좋음
        if (response != "") {
            addContentTypeHeader(locationConfig.getType(requestMessage.getUri()));
            addContentLengthHeader(response);
        }
    }
}

//void ResponseHeaderAdder::addCacheControlHeader(){}

void ResponseHeaderAdder::addContentTypeHeader(const string & contentType)
{
    responseMessage.addHeader("Content-Type", contentType);
}

void ResponseHeaderAdder::addContentLengthHeader(const string & responseBody)
{
    responseMessage.addHeader("Content-Length", to_string(responseBody.length()));
}


void ResponseHeaderAdder::addLocationHeader(const string & location)
{
    responseMessage.addHeader("Location", location);
}

void ResponseHeaderAdder::addAllowHeader(const vector<string> & acceptMethods)
{
    string methods;
    for (int i = 0; i < acceptMethods.size()-1; i++) {
        methods += acceptMethods.at(i) + ", ";
    }
    methods += acceptMethods.at(acceptMethods.size()-1);
    responseMessage.addHeader("Allow", methods);
}

void ResponseHeaderAdder::addDateHeader()
{
    time_t now = time(0);
    tm *gmt = gmtime(&now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S %Z", gmt);
    responseMessage.addHeader("Date", string(buffer));
}

void ResponseHeaderAdder::parseCgiProduct(string & response, string & contentType)
{
    Utils utils;

    size_t newline = response.find("\n\n");
    if (newline != string::npos) {
        string header = response.substr(0, newline);
        if (header.find(":") != string::npos) {
            if (header.substr(0, 12) == "Content-type") {
                contentType = utils.ltrim(header.substr(13, header.length()-13));
                response = response.substr(newline+2, response.length()-newline-2);
            }
        }
    }
}