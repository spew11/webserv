#ifndef CGI_METHOD_EXECUTOR_HPP
# define CGI_METHOD_EXECUTOR_HPP
# include <iostream>
# include <string>
# include <fstream>
# include "IMethodExecutor.hpp"
# include <unistd.h>
# ifdef __linux__
# include <signal.h>
# include <sys/wait.h>
# include <cstring>
# endif
using namespace std;

class CgiMethodExecutor : public IMethodExecutor
{
    private:
		const int READ;
		const int WRITE;

		int	stdin_fd;
		int stdout_fd;

		char **cgiEnv;
		string read_from_pipe();
		void write_to_pipe(string body);
	public:
        CgiMethodExecutor(char **cgiEnv);
		~CgiMethodExecutor();
        int getMethod(const string &resourcePath, string &response);
        int postMethod(const string &resourcePath, const string &request, string &response);
        int deleteMethod(const string & resourcePath) const;
        int putMethod(const string & resourcePath, const string & request, string & response);
        int headMethod(const string & resourcePath, string & response);
};

#endif