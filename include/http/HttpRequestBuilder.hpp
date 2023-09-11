#ifndef HTTP_REQUEST_BUILDER_HPP
#define HTTP_REQUEST_BUILDER_HPP
#include <iostream>
#include <string>
#include <map>
#include "Utils.hpp"
#include "HttpRequestMessage.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

class HttpRequestBuilder
{
private:
	enum HttpRequestBuilderBuildStep
	{ 
		BUILD_FIRST = 0,
		BUILD_HEADER,
		BUILD_BODY,
	};
	enum HttpMethodType
	{
		METHOD_TYPE_NONE = 0,
		GET,
		POST,
		PUT,
		HEAD,
		DELETE,
	};
	map<string, HttpMethodType> httpMethods;
	HttpRequestBuilderBuildStep buildStep;
	HttpMethodType methodType;
	string method;
	string path;
	string httpVersion;
	map<string,string> headers;
	int contentLength;
	string body;
	bool isChunked;
	bool needMoreChunk;
	string getMethod(const HttpMethodType &methodType) const;
	void setBuildStep(HttpRequestBuilderBuildStep buildStep);
	HttpRequestBuilderBuildStep getBuildStep(void);
	void setMethodType(HttpMethodType methodType);
	HttpMethodType getMethodType(void);
	void setPath(string path);
	string getPath(void);
	void setHttpVersion(string httpVersion);
	string getHttpVersion(void);
	size_t getContentLength(void);
	bool getIsChunked(void);
	void setBody(string body);
	void appendBody(string body);
	string getBody(void);
	bool buildFirstLine(string str, bool checkOnly = false, bool formatOnly = false);
	bool setHeader(string str, bool checkOnly = false);
	string getHeader(string key);
	vector<string> split(const string& s, const string& delim);

public:
	HttpRequestBuilder();
	~HttpRequestBuilder();
	int isHttp(string &recvBuf);
	void print(void);
	void erase(void);
	HttpRequestMessage *createRequestMessage();
};
#endif