#ifndef DEFAULT_METHOD_EXECUTOR_HPP
#define DEFAULT_METHOD_EXECUTOR_HPP
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "IMethodExecutor.hpp"
#include "ServerHandler.hpp"
#include "Client.hpp"
#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

class ServerHandler;
class Client;
class DefaultMethodExecutor : public IMethodExecutor
{
private:
	typedef int method_step;
	static const method_step STEP_OPEN_FILE = 0x00000001;
	static const method_step STEP_IO_OPER = 0x00000002;

	ServerHandler *sh;
	Client *client;

	method_step step;
	int fd;
	size_t write_buf_idx;
public:
	DefaultMethodExecutor(ServerHandler *sh, Client *client);
	~DefaultMethodExecutor();
	int getMethod(const string &resourcePath, string &response, const int &exitCode);
	int postMethod(const string &resourcePath, const string &request, string &response, const int &exitCode);
	int deleteMethod(const string &resourcePath, const int &exitCode) const;
	int putMethod(const string &resourcePath, const string &request, string &response, const int &exitCode);
	int headMethod(const string &resourcePath, string &response, const int &exitCode);
};
#endif
