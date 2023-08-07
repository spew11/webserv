#ifndef CGI_METHOD_EXECUTOR_HPP
# define CGI_METHOD_EXECUTOR_HPP
# include <iostream>
# include <string>
# include <fstream>
# include "IMethodExecutor.hpp"
# include "ICgiScriptor.hpp"

using namespace std;

class CgiMethodExecutor : public IMethodExecutor
{
    public:
        int getMethod(const string &resourcePath, string &response);
        int postMethod(const string &resourcePath, const string &request, string &response);
        int deleteMethod(const string & resourcePath) const;
};

#endif