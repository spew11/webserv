#ifndef I_METHOD_EXECUTOR_HPP
# define I_METHOD_EXECUTOR_HPP
# include <iostream>
# include <string>
# include <fstream>

using namespace std;

class IMethodExecutor
{
    public:
        virtual int getMethod(const string &resourcePath, string &content) const = 0 ;
        virtual int postMethod(const string &pathToSave, const string &content) const = 0;
        virtual int deleteMethod(const string & resourcePath) const = 0;
};
#endif