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
	static const int STEP_OPEN_FILE = 0x00000001;
	static const int STEP_IO_OPER = 0x00000002;
	static const int STEP_FINAL = 0x00000004;

	ServerHandler *sh;
	Client *client;

	method_step step;
	int fd;
public:
	DefaultMethodExecutor(ServerHandler *sh, Client *client);
	~DefaultMethodExecutor();
	int getMethod(const string &resourcePath, string &response);
	int postMethod(const string &resourcePath, const string &request, string &response);
	int deleteMethod(const string &resourcePath) const;
	int putMethod(const string &resourcePath, const string &request, string &response);
	int headMethod(const string &resourcePath, string &response);
};
#endif
