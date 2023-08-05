#ifndef HTTP_RESPONSE_BUILDER_HPP
# define HTTP_RESPONSE_BUILDER_HPP
# include "HttpRequestMessage.hpp"
# include "HttpResponseMessage.hpp"
# include "IMethodExecutor.hpp"
# include "WebservValues.hpp"
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include <vector>
# include <sys/stat.h>
# include <unistd.h>

class HttpResponseBuilder
{
    private:
        HttpResponseMessage responseMessage;
        HttpRequestMessage & requestMessage;
        WebservValues & webservValues;
        string resourcePath; // uri에서 locations를 거쳐 찾은 진짜 경로
        string requestBody;
        bool needMoreMessageFlag; 
        bool needCgiFlag;
        void initWebservValues(const ServerConfig::LocationMap &locationMap);
        void addResponseHeaders();
    public:
        void build(const IMethodExecutor & methodExecutor);
        HttpResponseBuilder(HttpRequestMessage & requestMessage, WebservValues &webservValues, const ServerConfig::LocationMap &locationMap);
        void addRequestMessage(const string &request);
        string getResourcePath() const;
        string findReasonPhrase(const int &statusCode);
        bool getNeedMoreMessageFlag() const;
        bool getNeedCgiFlag() const;
};
#endif
