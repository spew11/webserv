#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

class ConfigParser
{
private:
    list<string>    tokenList;

    void read( const string & file, string & buffer );
    void tokenize( const string & file, list<string> & list );
public:
    ConfigParser( const string & file );
};
