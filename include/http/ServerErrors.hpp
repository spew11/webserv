#ifndef SERVER_ERROR_HPP
# define SERVER_ERROR_HPP
# include <iostream>
# include <string>
# include "Utils.hpp"

using namespace std;

class ServerErrors
{
    public:
        string findReasonPhrase(const int &statusCode) const;
        string findErrorMessage(const int &statusCode) const;
        string generateErrorHtml(const int & statusCode) const;
};

#endif