#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "Modules.hpp"
#include "ServerConfig.hpp"

class ConfigParser;

using namespace std;
class Config
{
private:
    friend class ConfigParser;
    Module *    mainMod;
    vector<ServerConfig> srvConfs;

    Config( Module * mainMod );
public:
    ~Config();
    // void getMainConf();
    const vector<ServerConfig> & getSrvConf( void ) const {	return srvConfs; }
};

