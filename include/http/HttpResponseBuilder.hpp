#ifndef HTTP_RESPONSE_BUILDER_HPP
# define HTTP_RESPONSE_BUILDER_HPP
# include "HttpRequestMessage.hpp"
# include "HttpResponseMessage.hpp"
# include "DefaultMethodExecutor.hpp"
# include "CgiMethodExecutor.hpp"
# include "WebservValues.hpp"
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "ResponseHeaderAdder.hpp"
# include "ServerErrors.hpp"
# include <vector>
# include <sys/stat.h>
# include <unistd.h>

class Server;

class HttpResponseBuilder {
    private:
        HttpRequestMessage * requestMessage;
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
        
        // request info
        string resourcePath; // locations를 거쳐 찾은 진짜 경로
        string requestBody;
        
        // reponse information 
        string contentType;
        string responseBody;
        int errorCode; // statudCode

        // flag
        bool needMoreMessageFlag; 
        bool needCgiFlag;
        bool end;

        void clear();
        void parseRequestUri(const string & requestTarget);
        int checkAcceptMethod(const vector<string> & acceptMethods, const string & httpMethod);
        int validateResource(const vector<string> & indexes, const string & httpMethod);
        void initWebservValues();
        void execute(IMethodExecutor & methodExecutor);
        void parseCgiProduct();
        void createResponseMessage();
    public:
        HttpResponseBuilder(const Server *server, WebservValues & webservValues);
        void initiate(const string & request);
        void addRequestMessage(const string &request);
        void build(IMethodExecutor & methodExecutor);
        
        //getter setter
        HttpResponseMessage getResponseMessage() const;
        HttpRequestMessage getRequestMessage() const;
        LocationConfig getLocationConfig() const;
        bool getNeedMoreMessageFlag() const;
        bool getNeedCgiFlag() const;
        bool getEnd() const;
};

#endif
