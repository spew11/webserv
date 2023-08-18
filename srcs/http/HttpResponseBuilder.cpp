#include "HttpResponseBuilder.hpp"

HttpResponseBuilder::HttpResponseBuilder(const Server *server, WebservValues & webservValues)
    : server(server)
{
    this->webservValues = &webservValues;
    this->webservValues->initEnvList();
    requestMessage = NULL;
    responseMessage = NULL;
    requestUri = "";
    uri = "";
    filename = "";
    args = "";
    queryString = "";
    resourcePath = "";
    requestBody = "";
    contentType = "";
    responseBody = "";
    statusCode = 500;
    needMoreMessage = false;
    needCgi = false;
    end = false;
    connection = true;
    autoIndex = false;
}

HttpResponseBuilder::~HttpResponseBuilder()
{
    if (responseMessage) {
        delete responseMessage;
        responseMessage = 0;
    }
    if (requestMessage) {
        delete requestMessage;
        responseMessage = 0;
    }
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
    requestUri = "";
    uri = "";
    filename = "";
    args = "";
    queryString = "";
    resourcePath = "";
    requestBody = "";
    contentType = "";
    responseBody = "";
    statusCode = 500;
    needMoreMessage = false;
    needCgi = false;
    end = false;
    connection = true;
    autoIndex = false;
}

int HttpResponseBuilder::parseRequestUri()
{
    const string & requestTarget = requestMessage->getRequestTarget();
    requestUri = requestTarget;
    
    size_t pos = requestUri.find_first_of(";?#");
    if (pos == string::npos) {
        uri = requestUri;
    }
    else {
        uri = requestUri.substr(0, pos);
    }

    filename = uri;

    pos = requestUri.find(";");
    if (pos != string::npos) {
        args = requestUri.substr(pos+1, min(requestUri.find("?"), requestUri.length()));
    }

    pos = requestUri.find("?");
    if (pos != string::npos) {
        queryString = requestUri.substr(pos+1, min(requestUri.find("#"), requestUri.length())-pos-1);
    }
    return 0;
}

int HttpResponseBuilder::checkClientMaxBodySize(const int & clientMaxBodySize)
{
    if (requestMessage->getBody().length() > clientMaxBodySize)
    {
        statusCode = 413; // "413 Request Entity Too Large" 
        return 1;
    }
    return 0;
}

int HttpResponseBuilder::isValidateResource()
{
    const vector<string> & indexes = locationConfig.getIndexes();
    const string & httpMethod = requestMessage->getHttpMethod();
    struct stat statbuf;
    string tmpPath = locationConfig.getRoot() + uri;

    if (httpMethod == "GET" or (httpMethod == "POST" and locationConfig.isCgi()) \
        or (httpMethod == "PUT" and locationConfig.isCgi()) or httpMethod == "HEAD") { 
        
        if (access(tmpPath.c_str(), F_OK) != 0) {
            statusCode = 404;
            return 1;
        }
        else if (access(tmpPath.c_str(), R_OK) != 0) {
            statusCode = 403;
            return 1;
        }

        if (stat(tmpPath.c_str(), &statbuf) < 0) {
            statusCode = 500;
            return 1;
        }

        if(S_ISDIR(statbuf.st_mode)) {

            bool exist = false;
            for (int i = 0; i < indexes.size(); i++) {
                string resourcePathTmp = tmpPath + indexes.at(i);
                if (access(resourcePathTmp.c_str(), R_OK) == 0) {
                    resourcePath = resourcePathTmp;
                    exist = true;
                    break;
                }
            }
            if (exist == false) {
                resourcePath = tmpPath;
                if (locationConfig.isAutoIndex()) {
                    if (opendir(resourcePath.c_str()) == 0) {
                        statusCode = 500;
                    }
                    else {
                        statusCode = 200;
                        autoIndex = true;
                    }
                }
                else {
                    statusCode = 404;
                }
                return 1;
            }
        }
        else if (S_ISREG(statbuf.st_mode)) { 
            resourcePath = tmpPath;
        }
    }
    else if (httpMethod == "POST" or httpMethod == "PUT") {
        if (access(tmpPath.c_str(), F_OK) == 0) {
            if (stat(tmpPath.c_str(), &statbuf) < 0) {
                statusCode = 500;
                return 1;
            }
            if (S_ISDIR(statbuf.st_mode)) { 
                statusCode = 400;
                return 1;
            }
        }
        resourcePath = tmpPath;
    }
    else if (httpMethod == "DELETE") {
        if (access(tmpPath.c_str(), F_OK) != 0) {
            statusCode = 404;
            return 1;
        }
        else if (access(tmpPath.c_str(), W_OK) != 0) {
            statusCode = 403;
            return 1;
        }
        resourcePath = tmpPath;
    }
    return 0;
}

void HttpResponseBuilder::initWebservValues()
{    
    webservValues->insert("args", args);
    webservValues->insert("query_string", queryString);
    webservValues->insert("method", requestMessage->getHttpMethod());
    webservValues->insert("host", requestMessage->getHeader("Host"));
    webservValues->insert("content_type", locationConfig.getType(resourcePath));
    webservValues->insert("request_filename", resourcePath);
    webservValues->insert("request_uri", requestUri);
    webservValues->insert("uri", uri);
    webservValues->insert("document_uri", uri);
}

void HttpResponseBuilder::execute(IMethodExecutor & methodExecutor)
{
    string httpMethod = requestMessage->getHttpMethod();
    
    // 'if-None-Match', 'if-Match' 와 같은 요청 헤더 지원할 거면 여기서 분기 한번 들어감(선택사항임)
    if (httpMethod == "GET") {
        statusCode = methodExecutor.getMethod(resourcePath, responseBody);
    }
    else if(httpMethod == "POST") {
        statusCode = methodExecutor.postMethod(resourcePath, requestBody, responseBody);
    }
    else if(httpMethod == "DELETE") {
        statusCode = methodExecutor.deleteMethod(resourcePath);
    }
    else if(httpMethod == "PUT") {
        statusCode = methodExecutor.putMethod(resourcePath, requestBody, responseBody);
    }
    else if (httpMethod == "HEAD") {
        statusCode = methodExecutor.headMethod(resourcePath, responseBody);
    }
}

void HttpResponseBuilder::parseCgiProduct()
{
    size_t lflf = responseBody.find("\n\n");
    if (lflf != string::npos) {
        string headersLine = responseBody.substr(0, lflf);
        vector<string> headers = Utils::split(headersLine, "\n");
        for (size_t i = 0; i < headers.size(); i++) 
        {
            vector<string> header = Utils::split(headers[i], ":");
            responseMessage->addHeader(header[0], Utils::ltrim(header[1]));
        }
        responseBody = responseBody.substr(lflf+2, -1);
    }
}

string HttpResponseBuilder::getResponse() const {
    
    string response = responseMessage->getServerProtocol() + " ";
    response += Utils::itoa(responseMessage->getStatusCode()) + " ";
    response += responseMessage->getReasonPhrase() + "\r\n";
    
    map<string, string> headers = responseMessage->getHeaders();
    for (map<string, string>::iterator it = headers.begin(); it != headers.end(); it++) {
        if (it->second != "") {
            response += it->first + ": " + it->second + "\r\n";
        }
    }
    response += "\r\n";
    if (requestMessage->getHttpMethod() != "HEAD") {
        response += responseMessage->getBody();
    }
    return response;
}

void HttpResponseBuilder::createInvalidResponseMessage()
{
    ResponseStatusManager responseStatusManager;
    ServerAutoIndexSimulator serverAutoIndexSimulator;

    responseMessage = new HttpResponseMessage();
    statusCode = 400;
    responseBody = serverAutoIndexSimulator.generateAutoIndexHtml(locationConfig.getRoot(), uri);
    responseMessage->setStatusCode(statusCode);
    responseMessage->setReasonPhrase(responseStatusManager.findReasonPhrase(statusCode));
    responseMessage->setBody(responseBody);
    // 에러 리다이렉션 있으면 여기서 체크 (따로 모듈화하기)
    if (locationConfig.isErrCode(statusCode)) {
        string errPageUrl = locationConfig.getErrPage(statusCode);
        // 폴더인지 파일인지 체크하고 열고 읽을수있는지 체크하고 리스폰스바디에 저장하고...
    }
}

void HttpResponseBuilder::createResponseMessage() {
    string httpMethod = requestMessage->getHttpMethod();
    responseMessage = new HttpResponseMessage();
    ResponseStatusManager responseStatusManager;

    responseMessage->setStatusCode(statusCode);
    responseMessage->setReasonPhrase(responseStatusManager.findReasonPhrase(statusCode));
    if (autoIndex) {
        ServerAutoIndexSimulator serverAutoIndexSimulator;
        responseBody = serverAutoIndexSimulator.generateAutoIndexHtml(locationConfig.getRoot(), uri);
    }
    else if (statusCode == 200 && locationConfig.isCgi()) {
        parseCgiProduct();
    }
    else if (statusCode != 200 && responseBody == "") {
        responseBody = responseStatusManager.generateResponseHtml(statusCode);
    }
    responseMessage->setBody(responseBody);
    ResponseHeaderAdder responseHeaderAdder(*requestMessage, *responseMessage, locationConfig, resourcePath);
    responseHeaderAdder.executeAll();
}

int HttpResponseBuilder::isAllowedRequestMessage()
{
    const vector<string> & acceptMethods = locationConfig.getAcceptMethods();
    const string & httpMethod = requestMessage->getHttpMethod();
    const string & serverProtocol = requestMessage->getServerProtocol();
    // protocol version check
    if (serverProtocol != "HTTP/1.1")
    {
        statusCode = 505;
        return 1;
    }
    // method check
    if (find(acceptMethods.begin(), acceptMethods.end(), httpMethod) == acceptMethods.end())
    {
        statusCode = 405;
        return 1;
    }
    // client max body size check
    if (checkClientMaxBodySize(locationConfig.getClientMaxBodySize()))
    {
        return 1;
    }
    return 0;
}

void HttpResponseBuilder::initiate(HttpRequestMessage *requestMessage)
{
    clear();
    this->requestMessage = requestMessage;
    cout << requestMessage->getHttpMethod() << endl;
    // 1. uri 구하기
    if ((end = parseRequestUri()) == 1) {
        createResponseMessage();
        return ;
    }
    // 2. uri 바탕으로 locationConfig 구하기
    locationConfig = server->getConfig(requestMessage->getHeader("host")).getLocConf(uri);
    // 3. locationConfig 메서드를 이용해서 accept_method, client_max_body_size 체크
    if ((end = isAllowedRequestMessage()) == 1)
    {
        createResponseMessage();
        return ;
    }
    // 4. 폴더면 '/' 붙이기
    if (uri[uri.length()-1] != '/') {
        string absolutePath = locationConfig.getRoot() + uri;
        if (access(absolutePath.c_str(), F_OK) == 0) {
            struct stat statbuf;
            if (stat(absolutePath.c_str(), &statbuf) < 0) {
                statusCode = 500;
                end = 1;
                createResponseMessage();
                return ;           
            }
            if (S_ISDIR(statbuf.st_mode)) {
                uri += "/";
            }
        }
    }
    // 5. uri 경로 유효성 검사하기
    if ((end = isValidateResource()) == 1)
    {
        createResponseMessage();
        return ;
    }
    // 6. webserv 변수 초기화하기
    initWebservValues();
    // 7. ResponseBuilder 클래스 플래그들 초기화하기
    needMoreMessage = requestMessage->getChunked();
    connection = requestMessage->getConnection();
    needCgi = locationConfig.isCgi();
    string cgiXX = locationConfig.getCgi();
    cout << "CGI 확장자" << cgiXX << endl;

    requestBody = requestMessage->getBody();
}

void HttpResponseBuilder::addRequestMessage(HttpRequestMessage *newRequestMessage)
{
    if ((end = checkClientMaxBodySize(newRequestMessage->getBody().length() + requestBody.length())) == 1) {
        return ;
    }
    needMoreMessage = newRequestMessage->getChunked();
    connection = newRequestMessage->getConnection();
    requestBody.append(newRequestMessage->getBody());

    /* 이전과 같은 chunk 요청인지 구별하는 방법은 HTTP 메서드와 requestTarget이 동일함을 확인, Contetn-Length 헤더가 없는것을 확인
    Transfer-Encoding: chunked 헤더가 지정되어 있는지를 확인하면 됌 */
}

void HttpResponseBuilder::build(IMethodExecutor & methodExecutor)
{
    execute(methodExecutor);
    createResponseMessage();
    end = true;  
}

HttpRequestMessage HttpResponseBuilder::getRequestMessage() const
{
    return *requestMessage;
}

HttpResponseMessage HttpResponseBuilder::getResponseMessage() const
{
    return *responseMessage;
}

LocationConfig HttpResponseBuilder::getLocationConfig() const
{
    return locationConfig;
}

bool HttpResponseBuilder::getNeedCgi() const
{
    return needCgi;
}

bool HttpResponseBuilder::getEnd() const
{
    return end;   
}

bool HttpResponseBuilder::getNeedMoreMessage() const
{
    return needMoreMessage;
}

bool HttpResponseBuilder::getConnection() const
{
    return connection;
}