#ifndef CGI_METHOD_EXECUTOR_HPP
#define CGI_METHOD_EXECUTOR_HPP
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <cstdlib>

#include "IMethodExecutor.hpp"
#include "ServerHandler.hpp"
#include "Client.hpp"

#ifdef __linux__
#include <signal.h>
#include <sys/wait.h>
#include <cstring>
#endif

using namespace std;

class ServerHandler;
class Client;

class CgiMethodExecutor : public IMethodExecutor
{
private:
	static const int READ;
	static const int WRITE;

	typedef int method_step;
	static const method_step STEP_FORK_PROC;
	static const method_step STEP_PROC_DIE;
	static const method_step STEP_PARENT_WRITE;
	static const method_step STEP_PARENT_READ;
	static const method_step STEP_CHILD;
	method_step step;

	ServerHandler *sh;
	void *client;
	char **cgiEnv;

	int stdin_fd;
	int stdout_fd;
	int parent_to_child_pipe[2];
	int child_to_parent_pipe[2];

	int pid;

	size_t write_buf_idx;
	
	int read_from_pipe(int &fd, string &body);
	int write_to_pipe(int &fd,  const string &body);

public:
	CgiMethodExecutor(ServerHandler *sh, Client *client, char **cgiEnv);
	~CgiMethodExecutor();
	int getMethod(const string &resourcePath, string &response, const int &exitCode);
	int postMethod(const string &resourcePath, const string &request, string &response, const int &exitCode);
	int deleteMethod(const string &resourcePath, const int &exitCode) const;
	int putMethod(const string &resourcePath, const string &request, string &response, const int &exitCode);
	int headMethod(const string &resourcePath, string &response, const int &exitCode);
};

#endif