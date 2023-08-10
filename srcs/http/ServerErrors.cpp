#include "ServerErrors.hpp"

string ServerErrors::findErrorMessage(const int & statusCode) const
{
    switch(statusCode) {
        case 100:
            return "The server has received the request headers and is waiting for the client to send the request body.";
        case 200:
            return "The request was successful. The resource has been located and transmitted in the response.";
        case 201:
            return "The request has been fulfilled, and a new resource has been created as a result.";
        case 204:
            return "The request was successful, but there is no additional content to send in the response.";
        case 301:
            return "The requested resource has been permanently moved to a different location.";
        case 302:
            return "The requested resource has been temporarily moved to a different location.";
        case 307:
            return "The requested resource is temporarily available at a different location.";
        case 400:
            return "The request could not be understood by the server due to malformed syntax or invalid parameters.";
        case 401:
            return "You are not authorized to access this resource. Please provide valid credentials.";
        case 403:
            return "Access to this resource is forbidden. You don't have the necessary permissions.";
        case 404:
            return "The page you're looking for could not be found. Please check the URL or try again later.";
        case 405:
            return "The requested HTTP method is not allowed for this resource.";
        case 500:
            return "An internal server error occurred. We apologize for the inconvenience. Please try again later.";
        case 503:
            return "The server is currently unavailable due to maintenance or high load. Please try again later.";
    }
    return "An internal server error occurred. We apologize for the inconvenience. Please try again later.";
}

string ServerErrors::findReasonPhrase(const int & statusCode) const
{
    switch(statusCode) {
        case 100:
            return "Continue";
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 204:
            return "No Content";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Found";
        case 307:
            return "Temporary Redirect";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 500:
            return "Internal Server Error";
        case 503:
            return "Service Unavailable";
    }
    return "Internal Server Error";
}

string ServerErrors::generateErrorHtml(const int & statusCode) const
{
    string reasonPhrase = findReasonPhrase(statusCode);
    string statusCodeStr = Utils::itoa(statusCode);
    string message = findErrorMessage(statusCode);

    string errorHtml = "<!DOCTYPE html>\
    <html>\
    <head>\
        <meta charset=\"UTF-8\">\
        <title> " + statusCodeStr + " " + statusCodeStr + " Not Found</title>\
        <style>\
            body {\
                font-family: Arial, sans-serif;\
                text-align: center;\
                padding: 100px;\
            }\
            h1 {\
                font-size: 48px;\
                margin-bottom: 10px;\
            }\
            p {\
                font-size: 18px;\
                margin-top: 0;\
            }\
        </style>\
    </head>\
    <body>\
        <h1>" + statusCodeStr + " " + reasonPhrase + " Not Found</h1>\
        <p>" + message + "</p>\
    </body>\
    </html>";

    return errorHtml;
}
