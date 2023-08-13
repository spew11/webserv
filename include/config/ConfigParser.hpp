#pragma once

#include <iostream>
#include <list>
#include <stack>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include "DirectiveTree.hpp"
#include "Modules.hpp"
#include "ConfigUtils.hpp"

class Config;

using namespace std;

class ConfigParser
{
private:
    list<string>            tokenList;
    stack<DirectiveTree *>  blocks;
    DirectiveTree           mainTree;

    void    read( const string & file, string & buffer );
    void    tokenize( const string & file, list<string> & list );
    bool    createDirective( Directive & );
    void    setTree( void );
public:
    ConfigParser( const string & file );

    Config * get( void );
};
