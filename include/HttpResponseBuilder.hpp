#ifndef HTTP_RESPONSE_BUILDER_HPP
# define HTTP_RESPONSE_BUILDER_HPP
# include "HttpRequestMessage.hpp"
# include "HttpResponseMessage.hpp"
# include "IMethodExecutor.hpp"
# include "WebservValues.hpp"
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "ResponseHeaderAdder.hpp"
# include <vector>
# include <sys/stat.h>
# include <unistd.h>

class HttpResponseBuilder
{
    private:
        HttpRequestMessage *requestMessage;
        HttpResponseMessage *responseMessage;

        const LocationConfig *locationConfig;
        WebservValues *webservValues;

        string resourcePath; // locations를 거쳐 찾은 진짜 경로
        string requestBody;
        bool needMoreMessageFlag; 
        bool needCgiFlag;
        void initWebservValues();
    public:
        void build(const IMethodExecutor & methodExecutor);
        void addRequestMessage(const string &request);
        string getResourcePath() const;
        string findReasonPhrase(const int &statusCode);
        bool getNeedMoreMessageFlag() const;
        bool getNeedCgiFlag() const;
        const HttpResponseMessage getResponseMessage() const;
        const HttpRequestMessage getRequestMessage() const;
        void initiate(const string & request, WebservValues & webservValues, const ServerConfig::LocationMap & locationMap);
        void clear() const;
};
#endif
