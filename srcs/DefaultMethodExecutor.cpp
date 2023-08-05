#include "DefaultMethodExecutor.hpp"
#include <unistd.h>

int DefaultMethodExecutor::getMethod(const string &resourcePath, string &content) const
{
    int statusCode;
    ifstream file(resourcePath);
    if (access(resourcePath.c_str(), F_OK) == 0) {
        if (file.is_open()) {
            getline(file, content, '\0');
            statusCode = 200; //ok
            file.close();
        }
        else {
            statusCode = 500; // internal server error
        }
    }
    else {
        statusCode = 404; //not found
    }
    return statusCode;
}

int DefaultMethodExecutor::postMethod(const string &pathToSave, const string &content) const{
    int statusCode;
    ofstream file(pathToSave);
    if (file.is_open()) {
        file << content;
        file.close();
        statusCode = 200;
    }
    else {
        statusCode = 500;
    }
    return statusCode;
}

int DefaultMethodExecutor::deleteMethod(const string &resourcePath) const
{
    int statusCode;
    int ret = remove(resourcePath.c_str());
    if (access(resourcePath.c_str(), F_OK) == 0) {
        if (ret == 0) {
            statusCode = 200;
        }
        else {
            statusCode = 500;
        }
    }
    else {
        statusCode = 404;
    }
    return statusCode;
}
