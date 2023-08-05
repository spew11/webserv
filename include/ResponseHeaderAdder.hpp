#ifndef RESPONSE_HEADER_ADDER_HPP
# define RESPONSE_HEADER_ADDER_HPP
# include <iostream>
# include <string>
# include "ResponseHeaderAdder.hpp"
# include "ServerConfig.hpp"
using namespace std;

class ResponseHeaderAdder
{
    protected:
        const HttpRequestMessage & requestMessage;
        HttpResponseMessage & responseMessage;
        const LocationConfig & locationConfig;
        const string & requestBody;
    public:
        ResponseHeaderAdder(const HttpRequestMessage & requestMessage, const HttpResponseMessage & responseMessage), \
            const LocationConfig & locationConfig, const string & requestBody)
        virtual void executeAll();
//        void addCacheControlHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessage);
        void addContentTypeHeader(const string & contentType);
        void addContentLengthHeader(const string & requestBody);
};

#endif