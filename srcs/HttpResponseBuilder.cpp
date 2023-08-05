#include "HttpResponseBuilder.hpp"
#include "ServerConfig.hpp"

void HttpResponseBuilder::initiate(const string & request, WebservValues &webservValues, const ServerConfig::LocationMap &locationMap)
{
    clear();
    requestMessage = new HttpRequestMessage(request);
    responseMessage = new HttpResponseMessage();
    if (requestMessage->getChunkedFlag()) {
        requestBody = requestMessage->getBody();
    }
    needMoreMessageFlag = requestMessage->getChunkedFlag();
    needCgiFlag = locationConfig->isCgi();
    *locationConfig = locationMap.getLocConf(requestMessage->getUri());
    initWebservValues();
}

void HttpResponseBuilder::initWebservValues()
{
    webservValues->insert("request_uri", requestMessage->getRequestUri());
    webservValues->insert("uri", requestMessage->getUri());
    webservValues->insert("document_uri", requestMessage->getUri());
    
    // $request_filename 및 resourcePath 초기화
    vector<string> indexes = locationConfig->getIndexes();
    struct stat statbuf;
    string tmpPath = locationConfig->getRoot() + requestMessage->getUri();
    if (stat(tmpPath.c_str(), &statbuf) < 0) {
        // 예외처리 하기
        cout << "stat error" << endl;
        exit(1);
    }
    if(S_ISDIR(statbuf.st_mode)) { // regular 파일이 없을 때
        for (int i = 0; i < indexes.size(); i++) {
            string resourcePathTmp = tmpPath+indexes.at(i);
            if (access(resourcePathTmp.c_str(), F_OK) == 0) {
                webservValues->insert("request_filename", "");
                resourcePath = resourcePathTmp;
                break;
            }
        }
    }
    else if (S_ISREG(statbuf.st_mode)) { // regular 파일이 있을 때
        webservValues->insert("request_filename", requestMessage->getFilename());
        resourcePath = tmpPath;
    }
    // $args 초기화
    webservValues->insert("args", requestMessage->getArgs());

    // $query_string 초기화
    webservValues->insert("query_string", requestMessage->getQueryString());

    // $method 초기화
    webservValues->insert("method", requestMessage->getHttpMethod());

    // $host 초기화
    webservValues->insert("host", requestMessage->getHeader("host"));

    // $content-type 초기화
    webservValues->insert("content_type", locationConfig->getType(requestMessage->getFilename()));
}

void HttpResponseBuilder::addRequestMessage(const string &request)
{
    HttpRequestMessage newRequestMessage(request);
    // post 요청인데 chunk 데이터로 오고 있을 때 동일한 메소드인지 여부 체크하는걸 여기서 할지는 추후에 논의할 예정

    needMoreMessageFlag = newRequestMessage.getChunkedFlag();
    requestBody.append(newRequestMessage.getBody());
}

bool HttpResponseBuilder::getNeedMoreMessageFlag() const
{
    return needMoreMessageFlag;
}

bool HttpResponseBuilder::getNeedCgiFlag() const
{
    return needCgiFlag;
}

void HttpResponseBuilder::build(const IMethodExecutor & methodExecutor)
{
    ResponseHeaderAdder responseHeaderAdder(*requestMessage, *responseMessage, *locationConfig, requestBody);

    int statusCode;
    string httpMethod = requestMessage->getHttpMethod();
    // 'if-None-Match', 'if-Match' 와 같은 요청 헤더 지원할 거면 여기서 분기 한번 들어감(선택사항임) (엔진엑스는 요청 들어오면 다 대응해주는 듯)
    if (httpMethod == "GET") {
        string content;
        statusCode = methodExecutor.getMethod(resourcePath, content);
        if (statusCode == 200) {
            responseMessage->setBody(content);
        }
    }
    else if(httpMethod == "POST") {
        statusCode = methodExecutor.postMethod(resourcePath, requestBody);
    }
    else if(httpMethod == "DELETE") {
        statusCode = methodExecutor.deleteMethod(resourcePath);
    }
    responseMessage->setStatusCode(statusCode);
    responseMessage->setReasonPhrase(findReasonPhrase(statusCode));
    responseHeaderAdder.executeAll();
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
        case 404:
            return "Not Found";
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
    delete requestMessage;
    delete responseMessage;
    locationConfig = 0;
    webservValues = 0;
    resourcePath = "";
    requestBody = "";
    needMoreMessageFlag = false;
    needCgiFlag = false;
}