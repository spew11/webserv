#include "CgiMethodExecutor.hpp"

CgiMethodExecutor::CgiMethodExecutor(ICgiScriptor *cgiScriptor) {}

int CgiMethodExecutor::getMethod(const string &resourcePath, string &response)
{
    return 200;
}

int CgiMethodExecutor::postMethod(const string &resourcePath, const string &request, string &response)
{
    return 200;
}

int CgiMethodExecutor::deleteMethod(const string & resourcePath) const
{
    return 200;
}