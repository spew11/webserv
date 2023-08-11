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

#include <ConfigUtils.hpp>

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
        syntax_error( const string & deriv )
         : runtime_error("Config error: The \"" + deriv + "\" directive has an incorrect syntax") {}
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
    Module ( Derivative const & deriv, enum ModuleType type )
     : name(deriv.name), type(type) {}
    virtual ~Module( void ) {}

    const string & getName( void ) const { return name; }
    const enum ModuleType & getType( void ) const { return type; }
    const vector<Module*> & getSubMods( void ) const { return subMods; }

    void addModule( Module * m ) { subMods.push_back(m); }
    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs ) = 0;
};

class MainModule : public Module
{
public:
	MainModule( const Derivative & deriv ) : Module(deriv, NO_TYPE) {}
    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs )
    {
        (void)deriv;
        (void)subDerivs;
    }
};

class ServerModule : public Module
{
private:
    uint32_t    ip;
    int         port;

public:
	ServerModule( const Derivative & deriv, const vector<Derivative> & subDerivs )
     : Module(deriv, NO_TYPE)
    {
        checkSyntax(deriv, &subDerivs);

        string listenArg = subDerivs[0].arg[1];
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

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs )
    {
        // subDerivs에는 listen지시어가 들어있다.
        if (subDerivs->size() != 1 || (*subDerivs)[0].arg.size() != 2)
            throw syntax_error("server");
    }
    
    const uint32_t & getIp( void ) const { return ip; }
    int getPort( void ) const { return port; }
};

class ServerNameModule : public Module
{
private:
    vector<string> serverNames;
public:
    ServerNameModule( const Derivative & deriv ) : Module(deriv, SRV_MOD)
    {
        checkSyntax(deriv, NULL);
        // server_name 지시어의 arg가 없다면 serverNames는 빈 벡터이다.
        for (size_t i = 1; i < deriv.arg.size(); i++)
            this->serverNames.push_back(deriv.arg[i]);
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs ) {}

    const vector<string> & getServerNames( void ) const { return serverNames; }
};

class LocationModule : public Module
{
private:
    string uri;
public:
    LocationModule( const Derivative & deriv ) : Module(deriv, NO_TYPE)
    {
        checkSyntax(deriv, NULL);

        uri = deriv.arg[1];

        if (uri.back() == '/')
            uri.pop_back();
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs )
    {
        if (deriv.arg.size() != 2)
            throw syntax_error("location");
    }

    const string & getUri( void ) const { return uri; }
};

class RootModule : public Module
{
private:
    string root;
public:
    RootModule( const Derivative & deriv ) : Module(deriv, LOC_MOD)
    {
        checkSyntax(deriv, NULL);

        root = deriv.arg[1];
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs )
    {  
        if (deriv.arg.size() != 2)
            throw syntax_error("root");
    }

    const string & getRoot( void ) const { return root; }
};

class TypesModule : public Module
{
private:
    map<string, string> typesMap;
public:
    TypesModule( const Derivative & deriv, vector<Derivative> subDerivs )
     : Module(deriv, LOC_MOD)
    {
        checkSyntax(deriv, &subDerivs);

        for (size_t i = 0; i < subDerivs.size(); i++)
        {
            string type = subDerivs[i].arg[0];

            for (size_t j = 1; j < subDerivs[i].arg.size(); j++)
            {
                string extension = subDerivs[i].arg[j];
                typesMap[extension] = type;
            }
        }
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs )
    {
        for (size_t i = 0; i < subDerivs->size(); i++)
        {
            if ((*subDerivs)[i].arg.size() < 2)
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
    IndexModule( const Derivative & deriv ) : Module(deriv, LOC_MOD)
    {
        checkSyntax(deriv, NULL);

        // deriv.arg가 비었다면 indexes벡터도 빈 벡터이다.        
        for (size_t i = 1; i < deriv.arg.size(); i++)
            indexes.push_back(deriv.arg[i]);
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs ) {}

    const vector<string> & getIndexes( void ) const { return indexes; }
};

class ErrorPageModule : public Module
{
private:
    vector<int> errCodes;
    string uri;
public:
    ErrorPageModule( const Derivative & deriv ) : Module(deriv, LOC_MOD)
    {
        checkSyntax(deriv, NULL);

        for (size_t i = 1; i < deriv.arg.size() - 1; i++)
            errCodes.push_back(atoi(deriv.arg[i].c_str()));
        
        uri = deriv.arg.back();
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs )
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
    bool isCgi;
public:
    CgiModule( const Derivative & deriv ) : Module(deriv, LOC_MOD)
    {
        checkSyntax(deriv, NULL);

        if (deriv.arg[1] == "on")
            isCgi = true;
		else
        	isCgi = false;
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs )
    {
        if (deriv.arg.size() != 2 || !isBoolean(deriv.arg[1]))
            throw syntax_error("cgi");
    }

    bool getCgi( void ) const { return isCgi; }
};

class CgiParamsModule : public Module
{
private:
    vector<pair<string, string> > params;
public:
    CgiParamsModule( const Derivative & deriv, const vector<Derivative> & subDerivs ) : Module(deriv, LOC_MOD)
    {
        checkSyntax(deriv, &subDerivs);

        for (int i = 0; i < subDerivs.size(); i++)
        {
            params.push_back(make_pair(subDerivs[i].arg[0], subDerivs[i].arg[1]));
        }
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs )
    {
        for (int i = 0; i < subDerivs->size(); i++)
        {
            if ((*subDerivs)[i].arg.size() != 2)
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
    AutoIndexModule( const Derivative & deriv ) : Module(deriv, LOC_MOD)
    {
        checkSyntax(deriv, NULL);

        if (deriv.arg[1] == "on")
            isAutoIndex = true;
        else
            isAutoIndex = false;
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs )
    {
        if (deriv.arg.size() != 2 || !isBoolean(deriv.arg[1]))
            throw syntax_error("autoindex");
    }
    
    bool getAutoIndex( void ) const { return isAutoIndex; }
};

class ClientMaxBodySizeModule : public Module
{
private:
    int maxSize;
public:
    ClientMaxBodySizeModule( const Derivative & deriv ) : Module(deriv, LOC_MOD)
    {
        checkSyntax(deriv, NULL);

        maxSize = atoi(deriv.arg[1].c_str());
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs )
    {
        if (deriv.arg.size() != 2 || !isNumeric(deriv.arg[1]))
            throw syntax_error("client_max_body_size");
    }

    int getClientMaxBodySize( void ) const { return maxSize; }
};

class AcceptMethodModule : public Module
{
private:
    vector<string> methods;
public:
    AcceptMethodModule( const Derivative & deriv ) : Module(deriv, LOC_MOD)
    {
        // accept_method 지시어 arg가 비었다면 methods벡터는 빈 벡터.
        for (int i = 1; i < deriv.arg.size(); i++)
        {
            methods.push_back(deriv.arg[i]);
        }
    }

    virtual void checkSyntax( const Derivative & deriv, const vector<Derivative> * subDerivs ) {}

    const vector<string> & getAcceptMethods( void ) const { return methods; }
};
