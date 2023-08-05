#ifndef I_CGI_METHOD_EXECUTOR_HPP
# define I_CGI_METHOD_EXECUTOR_HPP
# include <iostream>
# include <string>
# include <fstream>

using namespace std;

class ICgiScriptor
{
    public:
        virtual void method() = 0; //임의로 작성해봄
};

#endif