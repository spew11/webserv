#ifndef HTTP_RESPONSE_BUILDER_HPP
# define HTTP_RESPONSE_BUILDER_HPP
# include "HttpRequestMessage.hpp"
# include "HttpResponseMessage.hpp"
# include "MethodExecutor.hpp"
# include "EnvironmentValues.hpp"
# include <vector>

class HttpResponseBuilder
{
    private:
        vector<HttpRequestMessage> requestMessageList;
        HttpResponseMessage responseMessage;
        // 환경변수 리스트
        string method; // GET $method
        string host; // www.goole.com $host
        string uri; // /search/index.html $uri
        string args; //page=1 $args
        string queryString; // q=hello&hl=ko $query_string
        string requestUri; // /search/index.html;page=1?q=hello&hl=ko#fragment $request_uri
        string requestFilename; // index.html $request_filename
        
        string resourcePath; // uri에서 locations를 거쳐 찾은 진짜 경로...
        string requestBody;
        bool needMoreMessageFlag; 
        bool needCgiFlag;
        void setEnvironmentValue();
    public:
        void execute(MethodExecutor &methodExecutor);
        HttpResponseBuilder(const string &request, EnvironmentValues &envRepo);
        void addRequestMessage(const string &request);
        string getMethod() const;
        string getHost() const;
        string getUri() const;
        string getArgs() const;
        string getQueryString() const;
        string getRequestUri() const;
        string getRequestFilename() const;
        string getResourcePath() const;
        string findReasonPhrase(const int &statusCode);
        bool getNeedMoreMessageFlag() const;
        bool getNeedCgiFlag() const;
};

#endif
