#ifndef CGI_METHOD_EXECUTOR_HPP
# define CGI_METHOD_EXECUTOR_HPP
# include <iostream>
# include <string>
# include <fstream>
# include "IMethodExecutor.hpp"
# include "ICgiScriptor.hpp"
# include <unistd.h>

using namespace std;

class CgiMethodExecutor : IMethodExecutor
{
    private:
		const int READ  = 0;
		const int WRITE = 1;

		int	stdin_fd;
		int stdout_fd;

		char **cgiEnv;
		string read_from_pipe();
		void write_to_pipe(string body);
	public:
        CgiMethodExecutor(char **cgiEnv);
        int getMethod(const string &resourcePath, string &content) const;
        int postMethod(const string &pathToSave, const string &content) const;
        int deleteMethod(const string & resourcePath) const;
};

#endif