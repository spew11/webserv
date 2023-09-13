#include "HttpResponseBuilder.hpp"

HttpResponseBuilder::HttpResponseBuilder(const Server *server, WebservValues &webservValues)
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
    pathInfo = "";
    redirectUri = "";
	resourcePath = "";
	requestBody = "";
	contentType = "";
	responseBody = "";
	statusCode = -1;
	needMoreMessage = false;
	needCgi = false;
	end = false;
	connection = true;
	autoIndex = false;
    previousStatusCode = 0;

    methodExecutor = NULL;
}

HttpResponseBuilder::~HttpResponseBuilder()
{
    if (responseMessage)
    {
        delete responseMessage;
        responseMessage = 0;
    }
    if (requestMessage)
    {
        delete requestMessage;
        requestMessage = 0;
    }
	if (methodExecutor)
	{
		delete methodExecutor;
        methodExecutor = 0;
	}
}

void HttpResponseBuilder::clear()
{
	if (responseMessage)
	{
		delete responseMessage;
		responseMessage = 0;
	}
	if (requestMessage)
	{
		delete requestMessage;
		responseMessage = 0;
	}
	webservValues->clear();
	requestUri = "";
	uri = "";
	filename = "";
	args = "";
	queryString = "";
    pathInfo = "";
    redirectUri = "";
	resourcePath = "";
	requestBody = "";
	contentType = "";
	responseBody = "";
	statusCode = -1;
	needMoreMessage = false;
	needCgi = false;
	end = false;
	connection = true;
	autoIndex = false;
    previousStatusCode = 0;

    if (methodExecutor)
    {
        delete methodExecutor;
        methodExecutor = NULL;
    }
}

int HttpResponseBuilder::parseRequestUri()
{
    const string &requestTarget = requestMessage->getRequestTarget();
    requestUri = requestTarget;
    
    size_t pos = requestUri.find_first_of(";?#");
    if (pos == string::npos)
    {
        uri = requestUri;
    }
    else
    {
        uri = requestUri.substr(0, pos);
    }

    // filename 필요없을 듯
    filename = uri;
  
    pos = requestUri.find(";");
    if (pos != string::npos)
    {
        args = requestUri.substr(pos+1, min(requestUri.find("?"), requestUri.length()));
    }

    pos = requestUri.find("?");
    if (pos != string::npos)
    {
        queryString = requestUri.substr(pos+1, min(requestUri.find("#"), requestUri.length())-pos-1);
    }
    return 0;
}

int HttpResponseBuilder::checkClientMaxBodySize(const size_t &clientMaxBodySize)
{
    if (requestMessage->getBody().length() > clientMaxBodySize)
    {
        statusCode = 413; // "413 Request Entity Too Large" 
        return 1;
    }
    return 0;
}

bool HttpResponseBuilder::isValidateResource()
{
    const vector<string> &indexes = locationConfig.getIndexes();
    const string &httpMethod = requestMessage->getHttpMethod();
    struct stat statbuf;
    string tmpPath = locationConfig.getRoot() + uri;

    if (httpMethod == "GET" || (httpMethod == "POST" && locationConfig.isCgi()) \
        || (httpMethod == "PUT" && locationConfig.isCgi()) || httpMethod == "HEAD")
    { 
        if (access(tmpPath.c_str(), F_OK) != 0)
        {
            statusCode = 404;
            return 1;
        }
        else if (access(tmpPath.c_str(), R_OK) != 0)
        {
            statusCode = 403;
            return 1;
        }

        if (stat(tmpPath.c_str(), &statbuf) < 0)
        {
            statusCode = 500;
            return 1;
        }

        if(S_ISDIR(statbuf.st_mode))
        {
            bool exist = false;
            for(size_t i = 0; i < indexes.size(); i++)
            {
                string resourcePathTmp = tmpPath + indexes.at(i);
                if (access(resourcePathTmp.c_str(), R_OK) == 0)
                {
                    resourcePath = resourcePathTmp;
                    exist = true;
                    break;
                }
            }
            if (exist == false)
            {
                resourcePath = tmpPath;
                if (locationConfig.isAutoIndex())
                {
                    if (opendir(resourcePath.c_str()) == 0)
                    {
                        statusCode = 500;
                    }
                    else
                    {
                        statusCode = 200;
                        autoIndex = true;
                    }
                }
                else
                {
                    statusCode = 404;
                }
                return 1;
            }
        }
        else if (S_ISREG(statbuf.st_mode))
        { 
            resourcePath = tmpPath;
        }
    }
    else if (httpMethod == "POST" || httpMethod == "PUT")
    {
        if (access(tmpPath.c_str(), F_OK) == 0)
        {
            if (stat(tmpPath.c_str(), &statbuf) < 0)
            {
                statusCode = 500;
                return 1;
            }
            if (S_ISDIR(statbuf.st_mode))
            { 
                statusCode = 400;
                return 1;
            }
        }
        else
        { // 존재하지 않는 경로
            // 존재하지않는상위디렉터리/new_file.txt 인 경우 400을 리턴하도록 추가
            size_t slashPos = uri.find_last_of('/');
            if (slashPos != string::npos)
            {   // 상위디렉터리/정규파일 형식인 것임
                // 상위디렉터리까지만 잘라서 존재하는지 체크
                string tmpPath2 = locationConfig.getRoot() + uri.substr(0, slashPos);
                if (access(tmpPath2.c_str(), F_OK) == 0)
                {
                    if (stat(tmpPath2.c_str(), &statbuf) < 0)
                    {
                        statusCode = 500;
                        return 1;
                    }
                    if (!S_ISDIR(statbuf.st_mode))
                    {
                        statusCode = 400;
                        return 1;
                    }
                }
                else
                {
                    statusCode = 400;
                    return 1;
                }

            }
        }
        resourcePath = tmpPath;
    }
    else if (httpMethod == "DELETE")
    {
        if (access(tmpPath.c_str(), F_OK) != 0)
        {
            statusCode = 404;
            return 1;
        }
        else if (access(tmpPath.c_str(), W_OK) != 0)
        {
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
	webservValues->insert("request_method", requestMessage->getHttpMethod());
	webservValues->insert("host", requestMessage->getHeader("host"));
	webservValues->insert("content_type", locationConfig.getType(resourcePath));
	webservValues->insert("request_filename", resourcePath);
	webservValues->insert("request_uri", requestUri);
	webservValues->insert("uri", uri);
	webservValues->insert("document_uri", uri);
	webservValues->insert("fastcgi_path_info", pathInfo);
}

void HttpResponseBuilder::execute(const int &exitCode)
{
	string httpMethod = requestMessage->getHttpMethod();
	// 'if-None-Match', 'if-Match' 와 같은 요청 헤더 지원할 거면 여기서 분기 한번 들어감(선택사항임)
	if (httpMethod == "GET")
	{
		statusCode = methodExecutor->getMethod(resourcePath, responseBody, exitCode);
	}
	else if (httpMethod == "POST")
	{
		statusCode = methodExecutor->postMethod(resourcePath, requestBody, responseBody, exitCode);
	}
	else if (httpMethod == "DELETE")
	{
		statusCode = methodExecutor->deleteMethod(resourcePath, exitCode);
	}
	else if (httpMethod == "PUT")
	{
		statusCode = methodExecutor->putMethod(resourcePath, requestBody, responseBody, exitCode);
	}
	else if (httpMethod == "HEAD")
	{
		statusCode = methodExecutor->headMethod(resourcePath, responseBody, exitCode);
	}
}

void HttpResponseBuilder::parseCgiProduct()
{
    // \n\n이 없다면 header가 없는 것으로 정하였음
    size_t lflf = responseBody.find("\n\n");
    if (lflf != string::npos)
    {
        // \n 기준으로 스플릿된 아이들을 모두 헤더임
        string headersLine = responseBody.substr(0, lflf);
        cout << headersLine << endl;
        vector<string> headers = Utils::split(headersLine, "\n");
        for (size_t i = 0; i < headers.size(); i++) 
        {
            vector<string> header = Utils::split(headers[i], ":");
            responseMessage->addHeader(header[0], Utils::ltrim(header[1]));
        }
        // 나머지는 모두 바디
        responseBody = responseBody.substr(lflf+2, -1);
    }
}

string HttpResponseBuilder::getResponse() const
{
    string response = responseMessage->getServerProtocol() + " ";
    response += Utils::itoa(responseMessage->getStatusCode()) + " ";
    response += responseMessage->getReasonPhrase() + "\r\n";
    map<string, string> headers = responseMessage->getHeaders();
    
    for (map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
    {
        if (it->second != "")
        {
            response += it->first + ": " + it->second + "\r\n";
        }
    }
    response += "\r\n";
    if (requestMessage && requestMessage->getHttpMethod() != "HEAD")
    {
        response += responseMessage->getBody();
    }
    else if (!requestMessage)
    {
        response += responseMessage->getBody();
    }
    return response;
}

void HttpResponseBuilder::createResponseMessage()
{

    responseMessage = new HttpResponseMessage();
    ResponseStatusManager responseStatusManager;
    ResponseHeaderAdder responseHeaderAdder;
    
    if (!requestMessage)
    {
        statusCode = 400;
        connection = false; // 커넥션 끊기
        responseMessage->setStatusCode(statusCode);
        responseMessage->setReasonPhrase(responseStatusManager.findReasonPhrase(statusCode));
        responseBody = responseStatusManager.generateResponseHtml(statusCode);
        responseMessage->setBody(responseBody);
        responseHeaderAdder.executeAll(*this);
        return ;
    }
    
    string httpMethod = requestMessage->getHttpMethod();

    responseMessage->setStatusCode(statusCode);
    responseMessage->setReasonPhrase(responseStatusManager.findReasonPhrase(statusCode));
    if (autoIndex)
    {
        ServerAutoIndexSimulator serverAutoIndexSimulator;
        responseBody = serverAutoIndexSimulator.generateAutoIndexHtml(locationConfig.getRoot(), uri);
    }
    else if (statusCode == 200 && locationConfig.isCgi())
    {
        parseCgiProduct();
    }
    else if (statusCode != 200 && responseBody == "")
    {
        responseBody = responseStatusManager.generateResponseHtml(statusCode);
    }
    responseMessage->setBody(responseBody);
    responseHeaderAdder.executeAll(*this);
}

bool HttpResponseBuilder::isAllowedRequestMessage()
{
    const vector<string> &acceptMethods = locationConfig.getAcceptMethods();
    const string &httpMethod = requestMessage->getHttpMethod();
    const string &serverProtocol = requestMessage->getServerProtocol();
    // not implemented method check
    if (httpMethod == "NONE")
    {
        statusCode = 501;
        return 1;
    }
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

bool HttpResponseBuilder::isRedirectRequest()
{
    if (locationConfig.isRedirect() == true)
    {
        redirectUri = locationConfig.getRedirectUri();
        if (args != "") 
        {
            redirectUri += ";" + args;
        }
        if (queryString != "")
        {
            redirectUri += "?" + queryString;
        }
        statusCode = locationConfig.getRedirectStatusCode();
        return 1;
    }
    return 0;
}

void HttpResponseBuilder::initiate(HttpRequestMessage *requestMessage, int previousStatusCode)
{
    
    clear();
    this->previousStatusCode = previousStatusCode;
    this->requestMessage = requestMessage;
    if (!requestMessage)
    {   // isHttp()가 -1을 리턴한 경우
        createResponseMessage();
        return ;
    }
    this->needMoreMessage = requestMessage->getNeedMoreChunked();
    // 1. uri 구하기
    parseRequestUri();
    // 2. uri 바탕으로 locationConfig 구하기
    locationConfig = server->getConfig(requestMessage->getHeader("host")).getLocConf(uri);
    // 3. 리다이렉트 지시문 체크
    if ((end = isRedirectRequest()) == 1)
    {
        return ;
    }
    // 4. locationConfig 메서드를 이용해서 accept_method, client_max_body_size 체크
    if ((end = isAllowedRequestMessage()) == 1)
    {
        return ;
    }
    // 5. pathinfo parsing
    string root = locationConfig.getRoot();
	size_t idx = 0;
	while (true)
	{
		idx = uri.find_first_of("/", idx + 1);
		if (idx == string::npos)
			break;
		string tmp = root + uri.substr(0, idx);
		if (access(tmp.c_str(), F_OK) == 0)
		{
			struct stat statbuf;
			if (stat(tmp.c_str(), &statbuf) < 0)
			{
				statusCode = 500;
				return ;
			}
			if (S_ISDIR(statbuf.st_mode))
				filename += uri.substr(0, idx) + "/";
			else if (S_ISREG(statbuf.st_mode))
			{
				filename = uri.substr(0, idx);
				pathInfo = uri.substr(idx);
				uri = filename;
				break;
			}
		}
	}
    // 6. 폴더인데 이름 끝에 '/'가 없다면 '/' 붙이기
    if (uri[uri.length()-1] != '/')
    {
        string absolutePath = locationConfig.getRoot() + uri;
        if (access(absolutePath.c_str(), F_OK) == 0)
        {
            struct stat statbuf;
            if (stat(absolutePath.c_str(), &statbuf) < 0)
            {
                statusCode = 500;
                return;           
            }
            if (S_ISDIR(statbuf.st_mode))
            {
                uri += "/";
            }
        }
    }
    // 7. uri 경로 유효성 검사하기
    if ((end = isValidateResource()) == 1)
    {
        return ;
    }
    // 8. webserv 변수 초기화하기
    initWebservValues();
    // 9. Content-type 초기화
    contentType = locationConfig.getType(resourcePath);
    // 10. ResponseBuilder 클래스 플래그들 초기화하기
    needMoreMessage = requestMessage->getNeedMoreChunked();
    connection = requestMessage->getConnection();
    needCgi = locationConfig.isCgi();
    requestBody = requestMessage->getBody();
}

void HttpResponseBuilder::addRequestMessage(HttpRequestMessage *newRequestMessage)
{
    if ((end = checkClientMaxBodySize(newRequestMessage->getBody().length() + requestBody.length())) == 1)
    {
        return ;
    }
    needMoreMessage = newRequestMessage->getNeedMoreChunked();
    connection = newRequestMessage->getConnection();
    requestBody.append(newRequestMessage->getBody());
}

void HttpResponseBuilder::changeRequestMessage(const int &errorCode)
{
    string errorPageName = locationConfig.getErrPage(errorCode);
    map<string, string> headers;
    headers.insert(make_pair<string, string>("host", requestMessage->getHeader("host")));
    //response builder 초기화 안해도 됌 initate에서 초기화 해주기 때문에.
    initiate(new HttpRequestMessage("GET", errorPageName, "HTTP/1.1", headers, "", false), statusCode);
}

void HttpResponseBuilder::build(const int &exitCode)
{
    if (previousStatusCode)
    {   // custom Page를 GET하러 온 경우임
        if (!end)
        {   // 유효성 검사 성공
            execute(exitCode);
        }
    }
    else
    {   // 첫 요청이 유효성 검사에 성공한 경우
        execute(exitCode);
        cout << "STATUS CODE: " << statusCode << endl;
        if (locationConfig.isErrCode(statusCode) && requestMessage->getHeaders().size())
        {
            changeRequestMessage(statusCode);
            build(exitCode);
        }
    }

    if (statusCode == 0)
    {
        return ;
    }
    else if (previousStatusCode)
    {
        statusCode = previousStatusCode;
    }

    createResponseMessage();
    end = true;

	if (methodExecutor)
	{
		delete methodExecutor;
        methodExecutor = 0;
	}
}

HttpRequestMessage HttpResponseBuilder::getRequestMessage() const
{
	return *requestMessage;
}

HttpResponseMessage &HttpResponseBuilder::getResponseMessage()
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

void HttpResponseBuilder::print()
{
    cout << "***** HttpResponseBuilder print start *****" << endl;
    cout << "requestUri: " << requestUri << endl;
    cout << "uri: " << uri << endl;
    cout << "filename: " << filename << endl;
    cout << "args: " << args << endl;
    cout << "query string: " << queryString << endl;
    cout << "resourcePath: " << resourcePath << endl;
    cout << "requestBody: " << requestBody << endl;
    cout << "contentType: " << contentType << endl;
    cout << "responseBody: " << responseBody << endl;
    cout << "statusCode: " << statusCode << endl;
    cout << "needMoreMessage: " << needMoreMessage << endl;
    cout << "needCgi: " << needCgi << endl;
    cout << "end: " << end << endl;
    cout << "connection: " << connection << endl;
    cout << "autoIndex: " << autoIndex << endl;
    cout << "previousStatusCode: " << previousStatusCode << endl; 
    cout << "***** HttpResponseBuilder print end *****" << endl;
}

string HttpResponseBuilder::getResourcePath() const
{
    return resourcePath;
}

string HttpResponseBuilder::getRedirectUri() const
{
    return redirectUri;
}

string HttpResponseBuilder::getContentType() const
{
    return contentType;
}

void HttpResponseBuilder::setMethodExecutor(IMethodExecutor *methodExecutor)
{
    this->methodExecutor = methodExecutor;
}

int HttpResponseBuilder::getStatusCode() const
{
    return this->statusCode;
}