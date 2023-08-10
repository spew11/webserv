#include "DefaultMethodExecutor.hpp"

int DefaultMethodExecutor::getMethod(const string &resourcePath, string &response)
{
    int statusCode;
    ifstream file(resourcePath);
    if (file.fail()) {
        return 500;
    }
    getline(file, response, '\0');
    file.close();
    return 200;
}

int DefaultMethodExecutor::postMethod(const string &resourcePath, const string &request, string &response)
{
    ofstream file(resourcePath);
    
    if (file.fail()) {
        return 500;
    }   
    file << request;
    file.close();
    return 201;
}

int DefaultMethodExecutor::deleteMethod(const string &resourcePath) const
{
    if (remove(resourcePath.c_str()) != 0) {
        return 500;
    }
    return 204;
}
