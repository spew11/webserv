#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <DerivTree.hpp>
#include <Modules.hpp>
#include <ConfigUtils.hpp>

class Config;

using namespace std;

class ConfigParser
{
private:
    list<string>    tokenList;

    void read( const string & file, string & buffer );
    void tokenize( const string & file, list<string> & list );
    bool getDeriv( Derivative & );
    void setTree( DerivTree * tree );
public:
    ConfigParser( const string & file );

    Config * get( void );
};
