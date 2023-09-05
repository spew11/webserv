#ifndef HTTP_RESPONSE_BUILDER_HPP
#define HTTP_RESPONSE_BUILDER_HPP
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "DefaultMethodExecutor.hpp"
#include "CgiMethodExecutor.hpp"
#include "WebservValues.hpp"
#include "ServerConfig.hpp"
#include "ServerHandler.hpp"
#include "Server.hpp"
#include "LocationConfig.hpp"
#include "ResponseHeaderAdder.hpp"
#include "ResponseStatusManager.hpp"
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstdlib>
#include "ServerAutoIndexSimulator.hpp"

using namespace std;

class Server;

class HttpResponseBuilder
{
private:
    HttpRequestMessage *requestMessage;
    HttpResponseMessage *responseMessage;
    LocationConfig locationConfig;
    const Server *server;
    WebservValues *webservValues;

    // requestUril parsing 결과
    string requestUri;
    string uri;
    string filename;
    string args;
    string queryString;
    string pathInfo;
    
    string redirectUri;

    // request info
    string resourcePath;
    string requestBody;

    // reponse information 
    string contentType;
    string responseBody;
    int statusCode;
    
    bool needMoreMessage;
    bool needCgi;
    bool end;
    bool connection;
    bool autoIndex;
    int previousStatusCode;
    IMethodExecutor *methodExecutor;

    void clear();
    int parseRequestUri();
    int checkClientMaxBodySize(const size_t &clientMaxBodySize);
    bool isValidateResource();
    void initWebservValues();
    void execute(const int &exitCode);
    void parseCgiProduct();
    bool isAllowedRequestMessage();
    bool isRedirectRequest();

public:
    void createResponseMessage();
    void changeRequestMessage(const int &errorCode);
    HttpResponseBuilder(const Server *server, WebservValues &webservValues);
    ~HttpResponseBuilder();
    void initiate(HttpRequestMessage *requestMessage, int preStatusCode = 0);
    void addRequestMessage(HttpRequestMessage *newRequestMessage);
    void build(const int &exitCode);
    string getResponse() const;
    //getter setter
    HttpResponseMessage &getResponseMessage();
    HttpRequestMessage getRequestMessage() const;
    LocationConfig getLocationConfig() const;
    bool getNeedMoreMessage() const;
    bool getNeedCgi() const;
    bool getEnd() const;
    bool getConnection() const;
    void print();
    string getResourcePath() const;
    string getRedirectUri() const;
    string getContentType() const;
    int getStatusCode() const;

    void setMethodExecutor(IMethodExecutor *mothodExecutor);
};

#endif
