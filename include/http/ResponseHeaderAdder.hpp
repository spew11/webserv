#ifndef RESPONSE_HEADER_ADDER_HPP
# define RESPONSE_HEADER_ADDER_HPP
# include <iostream>
# include <string>
# include "HttpRequestMessage.hpp"
# include "HttpResponseMessage.hpp"
# include "ResponseHeaderAdder.hpp"
# include "ServerConfig.hpp"
# include "Utils.hpp"

using namespace std;

class ResponseHeaderAdder
{
    private:
        const HttpRequestMessage & requestMessage;
        HttpResponseMessage & responseMessage;
        const LocationConfig & locationConfig;
        const string & responseBody;
        const string & resourcePath;
        const string & contentType;
    public:
        ResponseHeaderAdder(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessage, \
            const LocationConfig & locationConfig, const string & responseBody, const string & resourcePath, const string & contentType);
        void executeAll();
        void addContentTypeHeader(const string & contentType);
        void addContentLengthHeader(const string & responseBody);
        void addLocationHeader(const string & location);
        void addAllowHeader(const vector<string> & acceptMethods);
        void addDateHeader();
        void parseCgiProduct(string & response, string & contentType);
        void setResponseBody(string responseBody);
        void setResourcePath(string resourcePath);
        void setContentType(string contentType);
};

#endif