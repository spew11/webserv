#include "HttpResponseBuilder.hpp"
#include "ServerConfig.hpp"
#include "CgiMethodExecutor.hpp"
#include "DefaultMethodExecutor.hpp"
#include "Server.hpp"
#include "errno.h"
#include <sstream>
#include <algorithm>
#include <vector>

HttpResponseBuilder::HttpResponseBuilder(const Server *server, WebservValues & webservValues)
    : server(server)
{
    this->webservValues = &webservValues;
    this->webservValues->initEnvList();
}

void HttpResponseBuilder::initiate(const string & request)
{
    clear();
    requestMessage = new HttpRequestMessage(request);
    responseMessage = new HttpResponseMessage();
    if (this->requestMessage->getChunkedFlag()) {
        requestBody = this->requestMessage->getBody();
    }
    needMoreMessageFlag = this->requestMessage->getChunkedFlag();
    locationConfig = server->getConfig(requestMessage->getHeader("Host")).getLocConf(requestMessage->getUri());
    needCgiFlag = locationConfig.isCgi();
    ready = verifyUrl();
    if (ready == false) {
        ready = checkAcceptMethod();
        if (ready == false) {
            initWebservValues();
        }
    }
}

int HttpResponseBuilder::verifyUrl()
{
    string httpMethod = requestMessage->getHttpMethod();
    vector<string> indexes = locationConfig.getIndexes();
    struct stat statbuf;
    string tmpPath;
    
    if (httpMethod == "GET" or (httpMethod == "POST" and locationConfig.isCgi())) { // 요청한 리소스가 무조건 존재해야됌
        tmpPath = locationConfig.getRoot() + requestMessage->getUri();
        
        // 리소스가 존재하는가, 읽을 수 있는가 체크
        if (access(tmpPath.c_str(), F_OK) != 0) {
            errorCode = 404;
            // errorPhrase = "File not found";
            return 1;
        }
        else if (access(tmpPath.c_str(), R_OK) != 0) {
            errorCode = 403;
            // errorPhrase = "File access denied";
            return 1;
        }

        // 리소스가 레귤러파일인가 디렉터리인가 체크
        if (stat(tmpPath.c_str(), &statbuf) < 0) {
            errorCode = 500;
            // errorPhrase = "oops, looks like somethig went worng.";
            return 1;
        }

        if(S_ISDIR(statbuf.st_mode)) { // 디렉터리 일때
            bool exist = false;
            for (int i = 0; i < indexes.size(); i++) {
                string resourcePathTmp = tmpPath+indexes.at(i);
                if (access(resourcePathTmp.c_str(), R_OK) == 0) {
                    resourcePath = resourcePathTmp;
                    exist = true;
                    break;
                }
            }
            if (exist == false) {
                // bad request 응답하거나 인덱스 페이지를 보여주거나 선택사항임, 지금은 전자.
                errorCode = 400;
                // errorPhrase = "Directory file";
                return 1;
            }
        }
        else if (S_ISREG(statbuf.st_mode)) { // 레귤러 파일 일때
            // 앞에서 읽을 수 있는 파일인지 이미 체크했음.
            resourcePath = tmpPath;
        }
    }
    else if (httpMethod == "POST") { // 요청한 패스가 존재할 수 도 없을 수 도 있음
        // 경로가 존재하는데 폴더면 문제 발생
        if (access(tmpPath.c_str(), F_OK) == 0) {
            if (S_ISDIR(statbuf.st_mode)) { // 디렉터리 일때
                // bad request 응답하거나 인덱스 페이지를 보여주거나 선택사항임, 지금은 전자.
                errorCode = 400;
                // errorPhrase = "Directory file";
                return 1;
            }
        }
        resourcePath = tmpPath;
    }
    else if (httpMethod == "DELETE") {
        // 리소스가 존재하는가, 삭제 할 수 있는가 체크
        if (access(tmpPath.c_str(), F_OK) != 0) {
            errorCode = 404;
            // errorPhrase = "File not found";
            return 1;
        }
        else if (access(tmpPath.c_str(), W_OK) != 0) {
            errorCode = 403;
            // errorPhrase = "File access denied";
            return 1;
        }
        resourcePath = tmpPath;
    }
    return 0;
}

int HttpResponseBuilder::checkAcceptMethod()
{
    string httpMethod = requestMessage->getHttpMethod();
    const vector<string> acceptMethods = locationConfig.getAcceptMethods();
    
    if (find(acceptMethods.begin(), acceptMethods.end(), httpMethod) == acceptMethods.end()) {
        errorCode = 405;
        return 1;
    }
    return 0;
}

void HttpResponseBuilder::initWebservValues()
{    
    webservValues->insert("args", requestMessage->getArgs());
    webservValues->insert("query_string", requestMessage->getQueryString());
    webservValues->insert("method", requestMessage->getHttpMethod());
    webservValues->insert("host", requestMessage->getHeader("Host"));
    webservValues->insert("content_type", locationConfig.getType(resourcePath));
    webservValues->insert("request_filename", resourcePath);
    webservValues->insert("request_uri", requestMessage->getRequestUri());
    webservValues->insert("uri", requestMessage->getUri());
    webservValues->insert("document_uri", requestMessage->getUri());
}

void HttpResponseBuilder::addRequestMessage(const string &request)
{
    HttpRequestMessage newRequestMessage(request);
    needMoreMessageFlag = newRequestMessage.getChunkedFlag();
    requestBody.append(newRequestMessage.getBody());
    
    /* 이전과 같은 chunk 요청인지 구별하는 방법은 HTTP 메서드와 URL이 동일함을 확인, Contetn-Length 헤더가 없는것을 확인
    Transfer-Encoding: chunked 헤더가 지정되어 있는지를 확인하면 됌 */
}

bool HttpResponseBuilder::getNeedMoreMessageFlag() const
{
    return needMoreMessageFlag;
}

bool HttpResponseBuilder::getNeedCgiFlag() const
{
    return needCgiFlag;
}

void HttpResponseBuilder::build(IMethodExecutor & methodExecutor)
{
    string httpMethod = requestMessage->getHttpMethod();
    string response;
    int statusCode;
    
    // 'if-None-Match', 'if-Match' 와 같은 요청 헤더 지원할 거면 여기서 분기 한번 들어감(선택사항임)
    
    if (httpMethod == "GET") {
        statusCode = methodExecutor.getMethod(resourcePath, response);
    }
    else if(httpMethod == "POST") {
        statusCode = methodExecutor.postMethod(resourcePath, requestBody, response);
    }
    else if(httpMethod == "DELETE") {
        statusCode = methodExecutor.deleteMethod(resourcePath);
    }
    
    responseMessage->setServerProtocol(requestMessage->getServerProtocol());
    responseMessage->setStatusCode(statusCode);
    responseMessage->setReasonPhrase(findReasonPhrase(statusCode));
    
    ResponseHeaderAdder responseHeaderAdder(*requestMessage, *responseMessage, locationConfig, response, resourcePath);
    responseHeaderAdder.executeAll();
    //이후에 response가 적절하게 파싱되어있을 거임
    responseMessage->setBody(response);
}

string HttpResponseBuilder::getResourcePath() const
{
    return resourcePath;
}

string HttpResponseBuilder::findReasonPhrase(const int &statusCode)
{
    switch(statusCode) {
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 204:
            return "No Content";
        case 400:
            return "Bad Request";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 500:
            return "Internal Server Error";
    }
    return "Internal Server Error";
}

HttpResponseMessage HttpResponseBuilder::getResponseMessage() const
{
    return *responseMessage;
}

HttpRequestMessage HttpResponseBuilder::getRequestMessage() const
{
    return *requestMessage;
}

void HttpResponseBuilder::clear()
{
    if (responseMessage) {
        delete responseMessage;
        responseMessage = 0;
    }
    if (requestMessage) {
        delete requestMessage;
        responseMessage = 0;
    }
    webservValues->clear();
    resourcePath = "";
    requestBody = "";
    needMoreMessageFlag = false;
    needCgiFlag = false;
}

LocationConfig HttpResponseBuilder::getLocationConfig() const
{
    return locationConfig;
}

void HttpResponseBuilder::setErrorCode(const int & num)
{
    errorCode = num;
}

int HttpResponseBuilder::getErrorCode() const
{
    return errorCode;
}

void HttpResponseBuilder::setErrorPhrase(const string & errorPhrase)
{
    this->errorPhrase = errorPhrase;
}

string HttpResponseBuilder::getErrorPhrase() const
{
    return errorPhrase;
}