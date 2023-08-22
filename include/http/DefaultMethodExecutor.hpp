#ifndef DEFAULT_METHOD_EXECUTOR_HPP
#define DEFAULT_METHOD_EXECUTOR_HPP
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "IMethodExecutor.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;

class DefaultMethodExecutor : public IMethodExecutor
{
public:
	~DefaultMethodExecutor();
	int getMethod(const string &resourcePath, string &response);
	int postMethod(const string &resourcePath, const string &request, string &response);
	int deleteMethod(const string &resourcePath) const;
	int putMethod(const string &resourcePath, const string &request, string &response);
	int headMethod(const string &resourcePath, string &response);
};
#endif
