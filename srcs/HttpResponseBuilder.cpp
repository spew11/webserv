#include "HttpResponseBuilder.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
//locations 매개변수로 있어야됌
HttpResponseBuilder::HttpResponseBuilder(const string &request, EnvironmentValues &envRepo, const ServerConfig::LocationMap &locations)
{
    HttpRequestMessage requestMessage(request);
    if (requestMessage.getChunkedFlag() == true) {
        requestBody = requestMessage.getBody();
    }
    requestMessageList.push_back(HttpRequestMessage(requestMessage));
    needMoreMessageFlag = requestMessage.getChunkedFlag();
    setEnvironmentValue();

    //환경변수에 세팅해주기
    envRepo.setPair("method", method);
    envRepo.setPair("host", host);
    envRepo.setPair("uri", uri);
    envRepo.setPair("args", args);
    envRepo.setPair("query_string", queryString);
    envRepo.setPair("request_uri", requestUri);
    envRepo.setPair("request_filename", requestFilename);
    //기타등등
    //지금은 그냥 주소를 직접 넣어줬지만, 나중에는 우진이의 locations 객체를 통해서 진짜 경로로 초기화할것임
    const LocationConfig & config = locations.getLocConf(uri);

    config.getRoot();
    config.getIndexes();
    config.getType("scriptname");
    resourcePath = "/Users/eunjilee/42Seoul/42cursus/webserv/resource/image.jpg";
}

void HttpResponseBuilder::setEnvironmentValue()
{
    HttpRequestMessage requestMessage = requestMessageList.at(0); 
    // <경로>;<파라미터>?<질의>#<프래그먼트> 경로조각은 없다고 가정
    map<string, string> headers = requestMessage.getHeaders();
    method = requestMessage.getMethod();
    requestUri = requestMessage.getUri();
    //host parsing
    if (headers.find("Host") != headers.end()) {
        host = headers.at("Host");
    }
    //uri parsing
    size_t pos = requestUri.find_first_of(";?#");
    if (pos == string::npos) {
        uri = requestUri;
    }
    else {
        uri = requestUri.substr(0, pos);
    }
    //requestFilename parsing
    requestFilename = requestUri.substr(requestUri.find_last_of("/")+1, pos-requestUri.find_last_of("/")-1);
    //args parsing
    pos = requestUri.find(";");
    if (pos != string::npos) {
        args = requestUri.substr(pos+1, min(requestUri.find("?"), requestUri.length())-pos-1);
    }
    //queryString parsing
    pos = requestUri.find("?");
    if (pos != string::npos) {
        queryString = requestUri.substr(pos+1, min(requestUri.find("#"), requestUri.length())-pos-1);
    }
    //CGI 변수 쓸지말지 체크해야됌
}

void HttpResponseBuilder::addRequestMessage(const string &request)
{
    HttpRequestMessage requestMessage(request);
    requestMessageList.push_back(HttpRequestMessage(requestMessage));
    needMoreMessageFlag = requestMessage.getChunkedFlag();
    requestBody.append(requestMessage.getBody());
}

string HttpResponseBuilder::getMethod() const
{
    return method;
}

string HttpResponseBuilder::getHost() const
{
    return host;
}

string HttpResponseBuilder::getUri() const
{
    return uri;
}

string HttpResponseBuilder::getArgs() const
{
    return args;
}

string HttpResponseBuilder::getQueryString() const
{
    return queryString;
}

string HttpResponseBuilder::getRequestUri() const
{
    return requestUri;
}

string HttpResponseBuilder::getRequestFilename() const
{
    return requestFilename;
}

bool HttpResponseBuilder::getNeedMoreMessageFlag() const
{
    return needMoreMessageFlag;
}

bool HttpResponseBuilder::getNeedCgiFlag() const
{
    return needCgiFlag;
}

void HttpResponseBuilder::execute(MethodExecutor &methodExecutor)
{
    HttpRequestMessage requestMessage = requestMessageList.at(0);
    int statusCode;
    if (method == "GET") {
        // 이전에 요청헤더타입 체크해야함. 정상이면 다음으로 넘어감
        string content;
        statusCode = methodExecutor.getMethod(resourcePath, content);
        if (statusCode == 200) {
            responseMessage.setBody(content);
        }
    }
    else if(method == "POST") {
        // 이전에요청헤더타입 체크
        statusCode = methodExecutor.postMethod(resourcePath, requestBody);
    }
    else if(method == "DELETE") {
        statusCode = methodExecutor.deleteMethod(resourcePath);
    }
    responseMessage.setStatusCode(statusCode);
    responseMessage.setReasonPhrase(findReasonPhrase(statusCode));
    //응답헤더를 박아..
}

string HttpResponseBuilder::getResourcePath() const
{
    return resourcePath;
}

string findReasonPhrase(const int &statusCode)
{
    //string reasonPrase;
    switch(statusCode) {
        case 200:
            return "OK";
        case 404:
            return "Not Found";
        case 500:
            return "Internal Server Error";
    }
    return "Internal Server Error";
}