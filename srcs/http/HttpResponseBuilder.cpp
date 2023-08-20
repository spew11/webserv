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
	pathInfo = "";
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
  
    string root = locationConfig.getRoot();
	int idx = 0;
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
				return 1;
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

	if (idx == string::npos && uri[uri.length() - 1] != '/')
	{
		string absolutePath = root + uri;
		if (access(absolutePath.c_str(), F_OK) == 0)
		{
			struct stat statbuf;
			if (stat(absolutePath.c_str(), &statbuf) < 0)
			{
				statusCode = 500;
				return 1;
			}
			if (S_ISDIR(statbuf.st_mode))
			{
				uri += "/";
			}
		}
	}
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

int HttpResponseBuilder::checkClientMaxBodySize(const int &clientMaxBodySize)
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

void HttpResponseBuilder::setSpecifiedErrorPage(const int &errorCode)
{
    string errorPage = locationConfig.getErrPage(statusCode);
    ResponseStatusManager responseStatusManager;
    ServerAutoIndexSimulator serverAutoIndexSimulator;
    LocationConfig redirectedLocationConfig;
    redirectedLocationConfig = server->getConfig(requestMessage->getHeader("host")).getLocConf(errorPage);
    string root = redirectedLocationConfig.getRoot();

    string path = root + errorPage;
    // 1. 에러페이지 경로 존재 유무 확인
    if (access(path.c_str(), F_OK) != 0)
    {
        // a. 경로가 존재하지 않으면 기본 내장 파일 사용 (but, fall back 에러 페이지가 있다면 그걸 사용)
        responseBody = responseStatusManager.generateResponseHtml(errorCode);
        return ;
    }
    // 2. 에러페이지 읽기 권한 확인
    if (access(path.c_str(), R_OK) != 0)
    {
        // a. 권한없으면 403 에러 대신 띄움 (but, fall back 에러 페이지가 있다면 그걸 사용)
        statusCode = 403;
        if (locationConfig.isErrCode(statusCode) == true)
        {
            setSpecifiedErrorPage(statusCode);
        }
        return ;
    }
    // 여기서 부터 에러페이지가 실존하고 권한도 있는 상태
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) < 0)
    {
        statusCode = 500;
        if (locationConfig.isErrCode(statusCode) == true)
        {
            setSpecifiedErrorPage(statusCode);
        }
        return ;
    }
    // 3. 에러페이지가 폴더다
    if (S_ISDIR(statbuf.st_mode))
    {
        // a. index지시어가 지정되어 있고, index파일이 있다면 그 파일로 대체
        const vector<string> &indexes = redirectedLocationConfig.getIndexes();
        bool exist = false;
        for (size_t i = 0; i < indexes.size(); i++)
        {
            string tmpPath = path + indexes.at(i);
            if (access(tmpPath.c_str(), R_OK) == 0)
            {
                exist = true;
                // reponse body에 저장(GET Method와 동일)
                ifstream file(tmpPath);
                if (file.fail())
                {
                    if (redirectedLocationConfig.isErrCode(500) == true)
                    {
                        statusCode = 500;
                        setSpecifiedErrorPage(statusCode);
                        return;
                    }
                }
                getline(file, responseBody, '\0');
                file.close();
                return ;
            }
        }
        // b. index지시어가 없거나 유효한 index.html이 없다.
        if(exist == false)
        {
            if (redirectedLocationConfig.isAutoIndex() == true)
            {
                // auto index가 on 이면 폴더의 내용을 나열한다.
                responseBody = serverAutoIndexSimulator.generateAutoIndexHtml(root, errorPage);
            }
            else
            {
                // 모두 다 해당하지 않는다면 내장 에러페이지 사용한다.
                responseBody = responseStatusManager.generateResponseHtml(statusCode);
            }
        }
    }
    else if(S_ISREG(statbuf.st_mode))
    {
        // 4. 에러페이지가 파일이면 바로 저장한다.
        ifstream file(path);
        if (file.fail())
        {
            if (redirectedLocationConfig.isErrCode(500) == true)
            {
                statusCode = 500;
                setSpecifiedErrorPage(statusCode);
                return;
            }
        }
        getline(file, responseBody, '\0');
        file.close();
    }
}

void HttpResponseBuilder::execute(IMethodExecutor &methodExecutor)
{
	string httpMethod = requestMessage->getHttpMethod();

	// 'if-None-Match', 'if-Match' 와 같은 요청 헤더 지원할 거면 여기서 분기 한번 들어감(선택사항임)
	if (httpMethod == "GET")
	{
		statusCode = methodExecutor.getMethod(resourcePath, responseBody);
	}
	else if (httpMethod == "POST")
	{
		statusCode = methodExecutor.postMethod(resourcePath, requestBody, responseBody);
	}
	else if (httpMethod == "DELETE")
	{
		statusCode = methodExecutor.deleteMethod(resourcePath);
	}
	else if (httpMethod == "PUT")
	{
		statusCode = methodExecutor.putMethod(resourcePath, requestBody, responseBody);
	}
	else if (httpMethod == "HEAD")
	{
		statusCode = methodExecutor.headMethod(resourcePath, responseBody);
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

void HttpResponseBuilder::createInvalidResponseMessage()
{
    ResponseStatusManager responseStatusManager;
    
    responseMessage = new HttpResponseMessage();
    ResponseHeaderAdder responseHeaderAdder;

    statusCode = 400;
    connection = false; // 커넥션 끊기
    if (locationConfig.isErrCode(statusCode))
    {
        setSpecifiedErrorPage(statusCode);
    }
    else
    {
        responseBody = responseStatusManager.generateResponseHtml(statusCode);
    }
    responseMessage->setStatusCode(statusCode);
    responseMessage->setReasonPhrase(responseStatusManager.findReasonPhrase(statusCode));
    responseMessage->setBody(responseBody);
    responseHeaderAdder.executeAll(*this);
}

void HttpResponseBuilder::createResponseMessage()
{
    string httpMethod = requestMessage->getHttpMethod();

    responseMessage = new HttpResponseMessage();
    ResponseStatusManager responseStatusManager;
    //커스텀 에러페이지 있는지 체크
    if (locationConfig.isErrCode(statusCode))
    {
        setSpecifiedErrorPage(statusCode);
    }
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
    ResponseHeaderAdder responseHeaderAdder;
    responseHeaderAdder.executeAll(*this);
}

bool HttpResponseBuilder::isAllowedRequestMessage()
{
    const vector<string> &acceptMethods = locationConfig.getAcceptMethods();
    const string &httpMethod = requestMessage->getHttpMethod();
    const string &serverProtocol = requestMessage->getServerProtocol();
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

void HttpResponseBuilder::initiate(HttpRequestMessage *requestMessage)
{
    
    clear();
    this->requestMessage = requestMessage;
    // 1. uri 구하기
    if ((end = parseRequestUri()) == 1)
    {
        createResponseMessage();
        return ;
    }
    // 2. uri 바탕으로 locationConfig 구하기
    locationConfig = server->getConfig(requestMessage->getHeader("host")).getLocConf(uri);
    // 3. 리다이렉트 지시문 체크
    if ((end = isRedirectRequest()) == 1)
    {
        createResponseMessage();
        return ;
    }
    // 4. locationConfig 메서드를 이용해서 accept_method, accepted_method, client_max_body_size 체크
    if ((end = isAllowedRequestMessage()) == 1)
    {
        createResponseMessage();
        return ;
    }
    // 5. 폴더인데 이름 끝에 '/'가 없다면 '/' 붙이기
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
    // 6. uri 경로 유효성 검사하기
    if ((end = isValidateResource()) == 1)
    {
        createResponseMessage();
        return ;
    }
    // 7. webserv 변수 초기화하기
    initWebservValues();
    // 8. ResponseBuilder 클래스 플래그들 초기화하기
    needMoreMessage = requestMessage->getChunked();
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
    needMoreMessage = newRequestMessage->getChunked();
    connection = newRequestMessage->getConnection();
    requestBody.append(newRequestMessage->getBody());

    /* 이전과 같은 chunk 요청인지 구별하는 방법은 HTTP 메서드와 requestTarget이 동일함을 확인, Contetn-Length 헤더가 없는것을 확인
    Transfer-Encoding: chunked 헤더가 지정되어 있는지를 확인하면 됌 */
}

void HttpResponseBuilder::build(IMethodExecutor &methodExecutor)
{
    execute(methodExecutor);
    createResponseMessage();
    end = true;  
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
