#ifndef I_METHOD_EXECUTOR_HPP
#define I_METHOD_EXECUTOR_HPP
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class IMethodExecutor
{
public:
	virtual ~IMethodExecutor(){};
	virtual int getMethod(const string &resourcePath, string &response, const int &exitCode) = 0;
	virtual int postMethod(const string &resourcePath, const string &request, string &response, const int &exitCode) = 0;
	virtual int deleteMethod(const string &resourcePath, const int &exitCode) const = 0;
	virtual int putMethod(const string &resourcePath, const string &request, string &response, const int &exitCode) = 0;
	virtual int headMethod(const string &resourcePath, string &response, const int &exitCode) = 0;
};
#endif