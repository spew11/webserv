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
		OPTIONS,
		PATCH,
		TRACE,
	};
	HttpRequestBuilderBuildStep build_step;
	HttpMethodType method_type;
	string method;
	string path;
	string http_version;
	map<string,string> headers;
	string getMethod(const HttpMethodType &methodType) const;
	int content_length;
	bool is_chunked;
	string body;
	void setBuildStep(HttpRequestBuilderBuildStep build_step);
	HttpRequestBuilderBuildStep getBuildStep(void);
	void setMethodType(HttpMethodType method_type);
	HttpMethodType getMethodType(void);
	void setPath(string path);
	string getPath(void);
	void setHttpVersion(string http_version);
	string getHttpVersion(void);
	int getContentLength(void);
	bool getIsChunked(void);
	void setBody(string body);
	void appendBody(string body);
	string getBody(void);
	bool buildFirstLine(string str, bool check_only = false);
	bool setHeader(string str, bool check_only = false);
	string getHeader(string key);
	int buildChunkedBody(string &recv_buf, string &body, vector<string> &lines, int start_idx);
	vector<string> split(const string& s, const string& delim);

public:
	HttpRequestBuilder();
	~HttpRequestBuilder();
	int isHttp(string &recv_buf);
	void print(void);
	void erase(void);
	HttpRequestMessage *createRequestMessage();
};
#endif
