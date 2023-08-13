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

void HttpResponseBuilder::parseRequestUri(const string & requestTarget)
{
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
}

int HttpResponseBuilder::checkAcceptMethod(const vector<string> & acceptMethods, const string & httpMethod)
{
    if (find(acceptMethods.begin(), acceptMethods.end(), httpMethod) == acceptMethods.end()) {
        statusCode = 405;
        return 1;
    }
    return 0;
}

int HttpResponseBuilder::validateResource(const vector<string> & indexes, const string & httpMethod)
{
    struct stat statbuf;
    string tmpPath;
    
    tmpPath = locationConfig.getRoot() + uri;

    if (httpMethod == "GET" or (httpMethod == "POST" and locationConfig.isCgi()) \
        or (httpMethod == "PUT" and locationConfig.isCgi()) or httpMethod == "HEAD") { 
        cout << "*************** GET METHOD **************" << endl;
        if (access(tmpPath.c_str(), F_OK) != 0) {
            cout << "*************** faile **************" << endl;
            statusCode = 404;
            return 1;
        }
        else if (access(tmpPath.c_str(), R_OK) != 0) {
            statusCode = 403;
            return 1;
        }
        cout << "*************** pass **************" << endl;

        if (stat(tmpPath.c_str(), &statbuf) < 0) {
            statusCode = 500;
            return 1;
        }

        if(S_ISDIR(statbuf.st_mode)) {
            bool exist = false;
            if (tmpPath[tmpPath.length()-1] != '/') {
                tmpPath += "/";
            }
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
                    cout << "^^^^^^^^^^^^^" << locationConfig.isAutoIndex() << endl;
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

    if (statusCode == 200 && locationConfig.isCgi()) {
        parseCgiProduct();
    }
}

void HttpResponseBuilder::parseCgiProduct()
{
    size_t newline = responseBody.find("\n\n");
    if (newline != string::npos) {
        string header = responseBody.substr(0, newline);
        if (header.find(":") != string::npos) {
            if (header.substr(0, 12) == "Content-type") {
                contentType = Utils::ltrim(header.substr(13, header.length()-13));
                responseBody = responseBody.substr(newline+2, responseBody.length()-newline-2);
            }
        }
    }
    //여기서 헤더 박아버려
}

void HttpResponseBuilder::createResponseMessage() {
    string httpMethod = requestMessage->getHttpMethod();
    responseMessage = new HttpResponseMessage();
    ResponseStatusManager responseStatusManager;

    responseMessage->setServerProtocol(requestMessage->getServerProtocol());
    responseMessage->setStatusCode(statusCode);
    responseMessage->setReasonPhrase(responseStatusManager.findReasonPhrase(statusCode));
    if (autoIndex) {
        ServerAutoIndexSimulator serverAutoIndexSimulator;
        responseBody = serverAutoIndexSimulator.generateAutoIndexHtml(resourcePath);
    }
    else if (statusCode != 200 && responseBody == "") {
        responseBody = responseStatusManager.generateResponseHtml(statusCode);
    }
    if (httpMethod != "HEAD") {
        responseMessage->setBody(responseBody);
    }
    contentType = locationConfig.getType(resourcePath); //이따 고치자
    ResponseHeaderAdder responseHeaderAdder(*requestMessage, *responseMessage, locationConfig, responseBody, resourcePath, contentType);
    responseHeaderAdder.executeAll();
}

void HttpResponseBuilder::initiate(const string & request)
{
    clear();
    requestMessage = new HttpRequestMessage(request);
    
    parseRequestUri(requestMessage->getRequestTarget());
    locationConfig = server->getConfig(requestMessage->getHeader("Host")).getLocConf(uri);
    if ((end = checkAcceptMethod(locationConfig.getAcceptMethods(), requestMessage->getHttpMethod())) == true) {
        createResponseMessage();
        return;
    }
    if ((end = validateResource(locationConfig.getIndexes(), requestMessage->getHttpMethod())) == true) {
        createResponseMessage();
        return;
    }
    initWebservValues();
    needMoreMessage = requestMessage->getChunked();
    connection = requestMessage->getConnection();
    needCgi = locationConfig.isCgi();
    requestBody = requestMessage->getBody();
}

void HttpResponseBuilder::addRequestMessage(const string &request)
{
    HttpRequestMessage newRequestMessage(request);
    needMoreMessage = newRequestMessage.getChunked();
    connection = newRequestMessage.getConnection();
    requestBody.append(newRequestMessage.getBody());

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