#ifndef DEFAULT_METHOD_EXECUTOR_HPP
# define DEFAULT_METHOD_EXECUTOR_HPP
# include "HttpRequestMessage.hpp"
# include "HttpResponseMessage.hpp"
# include "IMethodExecutor.hpp"
# include <iostream>
# include <string>
# include <fstream>

using namespace std;

class DefaultMethodExecutor : IMethodExecutor
{
    private:
    public:
        int getMethod(const string &resourcePath, string &content) const;
        int postMethod(const string &pathToSave, const string &content) const;
        int deleteMethod(const string & resourcePath) const;
};
#endif
