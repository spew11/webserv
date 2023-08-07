#pragma once

#include <algorithm>
#include <Modules.hpp>
#include <LocationConfig.hpp>

class ServerConfig
{
public:
    ServerConfig(ServerModule &srvMod);
    bool operator==(const string & host) const
    {
        const vector<string> & serverNames = getServerNames();

        if (find(serverNames.begin(), serverNames.end(), host) == serverNames.end())
            return false;
        
        return true;
    }

    class LocationMap
    {
    private:
        map<string, LocationConfig> locConfMap;

    public:
        void insert(const string &, const LocationConfig &);
        const LocationConfig &getLocConf( string uri ) const;
    };

    const uint32_t &getIp(void) const { return srvMod->getIp(); }
    int getPort(void) const { return srvMod->getPort(); }
    const vector<string> &getServerNames(void) const { return srvMod->getServerNames(); }
    const LocationMap &getLocationMap(void) const { return locMap; }
private:
    ServerModule *srvMod;
    LocationMap locMap;
};

