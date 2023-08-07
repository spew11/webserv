#pragma once

#include <string>
#include <vector>

using namespace std;

enum ModuleType {
    NO_TYPE,
    MAIN_MOD,
    SRV_MOD,
    LOC_MOD,
};

enum DerivName
{
    ROOT,
};

struct Derivative {
    string          name;
    enum DerivName  e_name;
    vector<string>  arg;
    char            delim;

    Derivative( void ) {}
    Derivative( const string & name ) : name(name) {}
};

