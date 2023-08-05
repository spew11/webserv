#ifndef EXTENDED_RESPONSE_HEADER_ADDER_HPP
# define EXTENDED_RESPONSE_HEADER_ADDER_HPP
# include <iostream>
# include <string>
# include "ResponseHeaderAdder.hpp"

class ExtendedResponseHeaderAdder : ReponseHeaderAdder
{
    public:
        virtual void execute(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag); // @override
        
        /* 메소드 추가는 조작자의 마음
        
        void addServerHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addDateHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addContentEncodingHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addContentLanguageHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addContentDisposition(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addAccessControlAllowOrigin(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addXPoweredByHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void addRetryAfterHeader(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        void ContentSecurityPolicy(const HttpRequestMessage & requestMessage, HttpResponseMessage & responseMessag);
        
        */
};