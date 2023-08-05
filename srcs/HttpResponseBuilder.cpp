#include "HttpResponseBuilder.hpp"

HttpResponseBuilder::HttpResponseBuilder(HttpRequestMessage & requestMessage, WebservValues &webservValues, const ServerConfig::LocationMap &locationMap)
 : requestMessage(requestMessage), webservValues(webservValues)
{
    if (requestMessage.getChunkedFlag()) {
        requestBody = requestMessage.getBody();
    }
    needMoreMessageFlag = requestMessage.getChunkedFlag();
    initWebservValuesFromRequestMessage(locationMap);
}

// <경로>;<파라미터>?<질의>#<프래그먼트> 경로조각은 없다고 가정
void HttpResponseBuilder::initWebservValues(const ServerConfig::LocationMap &locationMap)
{
    string requestUri = requestMessage.getUri();
    // $request_uri 초기화
    webservValues.insert("request_uri", requestUri);
    
    // $uri 초기화
    size_t pos = requestUri.find_first_of(";?#");
    if (pos == string::npos) {
        webservValues.insert("uri", requestUri);
    }
    else {
        webservValues.insert("uri", requestUri.substr(0, pos));
    }

    // $document_uri 초기화
    webservValues.insert("document_uri", webservValues.getValue("document_uri"));
    
    // $request_filename 및 resourcePath 초기화
    const LocationConfig & clientConfig = locationMap.getLocConf(webservValues.getValue("uri"));
    vector<string> indexes = clientConfig.getIndexes();
    struct stat statbuf;
    string tmpPath = clientConfig.getRoot()+webservValues.getValue("uri");
    if (stat(tmpPath.c_str(), &statbuf) < 0) {
        cout << "stat error" << endl;
        exit(1);
    }
    if(S_ISDIR(statbuf.st_mode)) { // regular 파일이 없을 때
        webservValues.insert("request_filename", "");
        for (int i = 0; i < indexes.size(); i++) {
            string resourcePathTmp = tmpPath+indexes.at(i);
            if (access(resourcePathTmp.c_str(), F_OK) == 0) {
                resourcePath = resourcePathTmp;
                break;
            }
        }
    }
    else if (S_ISREG(statbuf.st_mode)) { // regular 파일이 있을 때
        webservValues.insert("request_filename", requestUri.substr(requestUri.find_last_of("/")+1, pos-requestUri.find_last_of("/")-1));
        resourcePath = tmpPath;
    }

    // $args 초기화
    pos = requestUri.find(";");
    if (pos != string::npos) {
        webservValues.insert("args", requestUri.substr(pos+1, min(requestUri.find("?"), requestUri.length())-pos-1));
    }

    // $query_string 초기화
    pos = requestUri.find("?");
    if (pos != string::npos) {
        webservValues.insert("query_string", requestUri.substr(pos+1, min(requestUri.find("#"), requestUri.length())-pos-1));
    }

    // CGI 변수 쓸지말지 체크
    /*if (clientConfig.isCGI()) {
        needCgiFlag = true;
    }*/

    // $method 초기화
    webservValues.insert("method", requestMessage.getMethod());

    // $host 초기화
    webservValues.insert("host", requestMessage.getHeader("host"));

    // $content-type 초기화
    webservValues.insert("content_type", clientConfig.getType());
    // 
}

void HttpResponseBuilder::addRequestMessage(const string &request)
{
    HttpRequestMessage newRequestMessage(request);
    // 메소드체크! 이걸 내가 해야되나...??????
    // if (newRequestMessage.getMethod() == requestMessage.getMethod())
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

void HttpResponseBuilder::addResponseHeaders()
{

}

void HttpResponseBuilder::build(const IMethodExecutor & methodExecutor)
{
    int statusCode;
    string method = requestMessage.getMethod();
    
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
    //응답헤더 넣기
    int contentLength = responseMessage.getBody().length();
    responseMessage.addHeader("Content-Length", contentLength);
    responseMessage.addHeader("Content-Type", con)
    //config.getType("scriptname"); 미메타입 찾아주는 메소드..
}

string HttpResponseBuilder::getResourcePath() const
{
    return resourcePath;
}

string HttpResponseBuilder::findReasonPhrase(const int &statusCode)
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