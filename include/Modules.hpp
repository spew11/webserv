#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <cstdlib>

#include <ConfigUtils.hpp>

using namespace std;

class Module
{
protected:
    const string            name;
    const enum ModuleType   type;
    vector<Module*>         subMods;
    
public:
    Module ( Derivative const & deriv, enum ModuleType type )
     : name(deriv.name), type(type) {}

    const string & getName( void ) const { return name; }
    const enum ModuleType & getType( void ) const { return type; }
    const vector<Module*> & getSubMods( void ) const { return subMods; }

    void addModule( Module * m ) { subMods.push_back(m); }

    virtual void print(int indent)
    {
        for (int i = 0; i < indent ; i++)
            cout << " ";
        cout << name << endl;

        for (size_t i = 0; i < subMods.size(); i++)
            subMods[i]->print(indent + 2);
    }
};

