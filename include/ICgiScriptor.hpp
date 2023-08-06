#ifndef I_CGI_METHOD_EXECUTOR_HPP
# define I_CGI_METHOD_EXECUTOR_HPP
# include <iostream>
# include <string>
# include <fstream>

using namespace std;

class ICgiScriptor
{
public:
	virtual ~ICgiScriptor() = 0;
    virtual void run(const string &scriptFile) = 0;
};

#endif