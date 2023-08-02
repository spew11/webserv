#pragma once

#include <iostream>
#include <vector>
#include <map>

#include <Modules.hpp>
#include <ServerConfig.hpp>

class ConfigParser;

using namespace std;

// 보류
// struct MainConfig
// {
//     WorkerConnectionsModule * workConnMod;
// };

class Config
{
private:
    friend class ConfigParser;
    Module *    mainMod;
    // MainConfig  mainConf;
    vector<ServerConfig> srvConfs;

    Config( Module * mainMod );
public:
    // serverHandler(소영)에 사용.
    void getMainConf(); 
    // server(소영), http(은지)에 사용.
    const vector<ServerConfig> & getSrvConf( void ) const {	return srvConfs; }

    void print()
    {
        for (size_t i = 0; i < srvConfs.size(); i++)
        {
            ServerConfig srvConf = srvConfs[i];

            srvConf.print();
        }
    }
};

