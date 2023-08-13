#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <stdint.h>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <arpa/inet.h>

#include "ConfigUtils.hpp"

using namespace std;

class Module
{
protected:
    const string            name;
    const enum ModuleType   type;
    vector<Module*>         subMods;

    class syntax_error : public runtime_error
    {
    public:
        syntax_error( const string & directive )
         : runtime_error("Config error: The \"" + directive + "\" directive has an incorrect syntax") {}
    };

    bool isBoolean( const string & str )
    {
        if (str == "on" || str == "off")
            return true;
        
        return false;
    }

    bool isNumeric( const string & str )
    {
        if (str.empty())
            return false;

        for (string::size_type i = 0; i < str.size(); ++i)
        {
            if (!isdigit(str[i]))
                return false;
        }

        return true;
    }

public:
    Module ( Directive const & directive, enum ModuleType type )
     : name(directive.name), type(type) {}
    virtual ~Module( void ) {}

    const string & getName( void ) const { return name; }
    const enum ModuleType & getType( void ) const { return type; }
    const vector<Module*> & getSubMods( void ) const { return subMods; }

    void addModule( Module * m ) { subMods.push_back(m); }
    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives ) = 0;
};

class MainModule : public Module
{
public:
	MainModule( const Directive & directive ) : Module(directive, NO_TYPE) {}
    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives )
    {
        (void)directive;
        (void)subDirectives;
    }
};

class ServerModule : public Module
{
private:
    uint32_t    ip;
    uint16_t    port;

public:
	ServerModule( const Directive & directive, const vector<Directive> & subDirectives )
     : Module(directive, NO_TYPE)
    {
        checkSyntax(directive, &subDirectives);

        string listenArg = subDirectives[0].arg[1];
        size_t delimIdx = listenArg.find(':');

        if (delimIdx == string::npos) // ':'이 없는 경우 허용하지 않음
            throw syntax_error("server");
        
        string ipStr = listenArg.substr(0, delimIdx);
        string portStr = listenArg.substr(delimIdx + 1);

        if (!ipStr.empty() && inet_addr(ipStr.c_str()) == INADDR_NONE) // ip주소형식 맞는지 확인
            throw syntax_error("server");

        if (ipStr.empty()) // ip값이 비었다면 모든 ip 허용
            ip = INADDR_ANY;
        else
            ip = ntohl(inet_addr(ipStr.c_str()));

        if (!isNumeric(portStr.c_str())) // port값이 숫자인지 확인
            throw syntax_error("server");

        port = atoi(listenArg.substr(delimIdx + 1).c_str());
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives )
    {
        // subDirectives에는 listen지시어가 들어있다.
        if (subDirectives->size() != 1 || (*subDirectives)[0].arg.size() != 2)
            throw syntax_error("server");
    }
    
    const uint32_t & getIp( void ) const { return ip; }
    uint16_t getPort( void ) const { return port; }
};

class ServerNameModule : public Module
{
private:
    vector<string> serverNames;
public:
    ServerNameModule( const Directive & directive ) : Module(directive, SRV_MOD)
    {
        checkSyntax(directive, NULL);
        // server_name 지시어의 arg가 없다면 serverNames는 빈 벡터이다.
        for (size_t i = 1; i < directive.arg.size(); i++)
            this->serverNames.push_back(directive.arg[i]);
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives ) {}

    const vector<string> & getServerNames( void ) const { return serverNames; }
};

class LocationModule : public Module
{
private:
    string uri;
public:
    LocationModule( const Directive & directive ) : Module(directive, NO_TYPE)
    {
        checkSyntax(directive, NULL);

        uri = directive.arg[1];

        if (uri.back() == '/')
            uri.pop_back();
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives )
    {
        if (directive.arg.size() != 2)
            throw syntax_error("location");
    }

    const string & getUri( void ) const { return uri; }
};

class RootModule : public Module
{
private:
    string root;
public:
    RootModule( const Directive & directive ) : Module(directive, LOC_MOD)
    {
        checkSyntax(directive, NULL);

        root = directive.arg[1];
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives )
    {  
        if (directive.arg.size() != 2)
            throw syntax_error("root");
    }

    const string & getRoot( void ) const { return root; }
};

class TypesModule : public Module
{
private:
    map<string, string> typesMap;
public:
    TypesModule( const Directive & directive, vector<Directive> subDirectives )
     : Module(directive, LOC_MOD)
    {
        checkSyntax(directive, &subDirectives);

        for (size_t i = 0; i < subDirectives.size(); i++)
        {
            string type = subDirectives[i].arg[0];

            for (size_t j = 1; j < subDirectives[i].arg.size(); j++)
            {
                string extension = subDirectives[i].arg[j];
                typesMap[extension] = type;
            }
        }
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives )
    {
        for (size_t i = 0; i < subDirectives->size(); i++)
        {
            if ((*subDirectives)[i].arg.size() < 2)
                throw syntax_error("types");
        }
    }

    const map<string, string> & getTypesMap( void ) const { return typesMap; }
};

class IndexModule : public Module
{
private:
    vector<string> indexes;
public:
    IndexModule( const Directive & directive ) : Module(directive, LOC_MOD)
    {
        checkSyntax(directive, NULL);

        // directive.arg가 비었다면 indexes벡터도 빈 벡터이다.        
        for (size_t i = 1; i < directive.arg.size(); i++)
            indexes.push_back(directive.arg[i]);
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives ) {}

    const vector<string> & getIndexes( void ) const { return indexes; }
};

class ErrorPageModule : public Module
{
private:
    vector<int> errCodes;
    string uri;
public:
    ErrorPageModule( const Directive & directive ) : Module(directive, LOC_MOD)
    {
        checkSyntax(directive, NULL);

        for (size_t i = 1; i < directive.arg.size() - 1; i++)
            errCodes.push_back(atoi(directive.arg[i].c_str()));
        
        uri = directive.arg.back();
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives )
    {
    }

    bool isErrCode( int code ) const 
    {
        if (find(errCodes.begin(), errCodes.end(), code) != errCodes.end())
            return true;

        return false;
    }
    const string & getUri( void ) const { return uri; }
};

class CgiModule : public Module
{
private:
    bool isCgi; // string으로 변경
public:
    CgiModule( const Directive & directive ) : Module(directive, LOC_MOD)
    {
        checkSyntax(directive, NULL);

        if (directive.arg[1] == "on")
            isCgi = true;
		else
        	isCgi = false;
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives )
    {
        if (directive.arg.size() != 2 || !isBoolean(directive.arg[1]))
            throw syntax_error("cgi");
    }

    bool getCgi( void ) const { return isCgi; }
};

class CgiParamsModule : public Module
{
private:
    vector<pair<string, string> > params;
public:
    CgiParamsModule( const Directive & directive, const vector<Directive> & subDirectives ) : Module(directive, LOC_MOD)
    {
        checkSyntax(directive, &subDirectives);

        for (int i = 0; i < subDirectives.size(); i++)
        {
            params.push_back(make_pair(subDirectives[i].arg[0], subDirectives[i].arg[1]));
        }
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives )
    {
        for (int i = 0; i < subDirectives->size(); i++)
        {
            if ((*subDirectives)[i].arg.size() != 2)
                throw syntax_error("cgi_params");
        }
    }

    const vector<pair<string, string> > & getParams( void ) const { return params; }
};

class AutoIndexModule : public Module
{
private:
    bool isAutoIndex;
public:
    AutoIndexModule( const Directive & directive ) : Module(directive, LOC_MOD)
    {
        checkSyntax(directive, NULL);

        if (directive.arg[1] == "on")
            isAutoIndex = true;
        else
            isAutoIndex = false;
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives )
    {
        if (directive.arg.size() != 2 || !isBoolean(directive.arg[1]))
            throw syntax_error("autoindex");
    }
    
    bool getAutoIndex( void ) const { return isAutoIndex; }
};

class ClientMaxBodySizeModule : public Module
{
private:
    int maxSize;
public:
    ClientMaxBodySizeModule( const Directive & directive ) : Module(directive, LOC_MOD)
    {
        checkSyntax(directive, NULL);

        maxSize = atoi(directive.arg[1].c_str());
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives )
    {
        if (directive.arg.size() != 2 || !isNumeric(directive.arg[1]))
            throw syntax_error("client_max_body_size");
    }

    int getClientMaxBodySize( void ) const { return maxSize; }
};

class AcceptMethodModule : public Module
{
private:
    vector<string> methods;
public:
    AcceptMethodModule( const Directive & directive ) : Module(directive, LOC_MOD)
    {
        // accept_method 지시어 arg가 비었다면 methods벡터는 빈 벡터.
        for (int i = 1; i < directive.arg.size(); i++)
        {
            methods.push_back(directive.arg[i]);
        }
    }

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives ) {}

    const vector<string> & getAcceptMethods( void ) const { return methods; }
};
