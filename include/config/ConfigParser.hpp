#pragma once

#include <iostream>
#include <list>
#include <stack>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include <DerivTree.hpp>
#include <Modules.hpp>
#include <ConfigUtils.hpp>

class Config;

using namespace std;

class ConfigParser
{
private:
    list<string>        tokenList;
    stack<DerivTree *>  blocks;
    DerivTree           mainTree;

    void read( const string & file, string & buffer );
    void tokenize( const string & file, list<string> & list );
    size_t findDelim( const string & str );
    bool getDeriv( Derivative & );
    void setTree( void );
public:
    ConfigParser( const string & file );

    Config * get( void );
};
