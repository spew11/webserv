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
        syntax_error( const string & directive );
    };

    bool isBoolean( const string & str );
    bool isNumeric( const string & str );

public:
    Module ( Directive const & directive, enum ModuleType type );
    virtual ~Module( void ) {}

    const string & getName( void ) const;
    const enum ModuleType & getType( void ) const;
    const vector<Module*> & getSubMods( void ) const;

    void addModule( Module * m );
    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives ) = 0;
};

class MainModule : public Module
{
public:
	MainModule( const Directive & directive );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );
};

class ServerModule : public Module
{
private:
    uint32_t    ip;
    uint16_t    port;

public:
	ServerModule( const Directive & directive, const vector<Directive> & subDirectives );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );
    
    uint32_t getIp( void ) const;
    uint16_t getPort( void ) const;
};

class ServerNameModule : public Module
{
private:
    vector<string> serverNames;
public:
    ServerNameModule( const Directive & directive );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    const vector<string> & getServerNames( void ) const;
};

class LocationModule : public Module
{
private:
    string uri;
public:
    LocationModule( const Directive & directive );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    const string & getUri( void ) const;
};

class RootModule : public Module
{
private:
    string root;
public:
    RootModule( const Directive & directive );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    const string & getRoot( void ) const;
};

class TypesModule : public Module
{
private:
    map<string, string> typesMap;
public:
    TypesModule( const Directive & directive, vector<Directive> subDirectives );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    const map<string, string> & getTypesMap( void ) const;
};

class IndexModule : public Module
{
private:
    vector<string> indexes;
public:
    IndexModule( const Directive & directive );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    const vector<string> & getIndexes( void ) const;
};

class ErrorPageModule : public Module
{
private:
    vector<int> errCodes;
    string      uri;
public:
    ErrorPageModule( const Directive & directive );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    bool isErrCode( int code ) const;
    const string & getUri( void ) const;
};

class CgiModule : public Module
{
private:
    string cgiType;
public:
    CgiModule( const Directive & directive );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    const string & getCgi( void ) const;
};

class CgiParamsModule : public Module
{
private:
    vector<pair<string, string> > params;
public:
    CgiParamsModule( const Directive & directive, const vector<Directive> & subDirectives );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    const vector<pair<string, string> > & getParams( void ) const;
};

class AutoIndexModule : public Module
{
private:
    bool isAutoIndex;
public:
    AutoIndexModule( const Directive & directive );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    bool getAutoIndex( void ) const;
};

class ClientMaxBodySizeModule : public Module
{
private:
    int maxSize;
public:
    ClientMaxBodySizeModule( const Directive & directive );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    int getClientMaxBodySize( void ) const;
};

class AcceptMethodModule : public Module
{
private:
    vector<string> methods;
public:
    AcceptMethodModule( const Directive & directive );

    virtual void checkSyntax( const Directive & directive, const vector<Directive> * subDirectives );

    const vector<string> & getAcceptMethods( void ) const;
};
