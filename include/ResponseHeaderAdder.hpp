#ifndef RESPONSE_HEADER_ADDER_HPP
# define RESPONSE_HEADER_ADDER_HPP
# include <iostream>
# include <string>
# include "HttpRequestMessage.hpp"
# include "HttpResponseMessage.hpp"

using namespace std;

class ReponseHeaderAdder
{
    public:
        virtual void execute(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addCacheCotrolHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addContentTypeHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addContentLengthHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addETagHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addLastModifiedHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addLocationHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addAllowHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
};

#endif