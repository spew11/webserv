#pragma once

#include <Modules.hpp>
#include <LocationConfig.hpp>

class ServerConfig
{
public:
    ServerConfig(ServerModule &srvMod);

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

