#pragma once

#include <Modules.hpp>
#include <EnvironmentValues.hpp>

class LocationConfig
{
private:
    RootModule *        rootMod;
    IndexModule *       indexMod;
    TypesModule *       typesMod;
    CgiModule *         cgiMod;
    CgiParamsModule *   cgiParamsMod;
    AutoIndexModule *   autoIndexMod;
    AcceptMethodModule * acceptMethodMod;
    ClientMaxBodySizeModule * cliMaxBodyMod;
    vector<ErrorPageModule*> errorPageMods;
public:
    LocationConfig( void );

    void addModules( const vector<Module*> & );

    const string & getRoot( void ) const;
    const vector<string> & getIndexes( void ) const;
    const string & getType( const string & scriptName ) const;
    bool isErrCode( int code ) const;
    const string & getErrPage( int code ) const;
    bool isCgi( void ) const;
    const string & getCgiCmd( void ) const;
    bool isAutoIndex( void ) const;
    int  getClientMaxBodySize( void ) const;
    const vector<string> & getAcceptMethods( void ) const;

    char ** getCgiParams( const EnvironmentValues & ) const;

    void print() const
    {
        if (rootMod)
            rootMod->print(2);
        if (indexMod)
            indexMod->print(2);
        if (typesMod)
            typesMod->print(2);
        if (errorPageMods.size())
            errorPageMods[0]->print(2);
        if (cgiMod)
            cgiMod->print(2);
        if (cgiParamsMod)
            cgiParamsMod->print(2);
    }
};
