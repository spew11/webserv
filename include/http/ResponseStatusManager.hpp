#ifndef SERVER_ERROR_HPP
#define SERVER_ERROR_HPP
#include <iostream>
#include <string>
#include "Utils.hpp"

using namespace std;

class ResponseStatusManager
{
public:
	ResponseStatusManager();
	string findReasonPhrase(const int &statusCode) const;
	string findStatusMessage(const int &statusCode) const;
	string generateResponseHtml(const int &statusCode) const;
};

#endif