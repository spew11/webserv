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
        const LocationConfig &getLocConf( string uri );

        void print()
        {
            for (map<string, LocationConfig>::const_iterator it = locConfMap.begin(); it != locConfMap.end(); ++it)
            {
                const string &key = it->first;
                const LocationConfig &value = it->second;

                cout << "Key: " << key << endl;
                it->second.print();
            }
        }
    };

    const uint32_t &getIp(void) const { return srvMod->getIp(); }
    int getPort(void) const { return srvMod->getPort(); }
    const vector<string> &getServerNames(void) const { return srvMod->getServerNames(); }
    const LocationMap &getLocationMap(void) const { return locMap; }

    void print()
    {
        locMap.print();
    }
private:
    ServerModule *srvMod;
    LocationMap locMap;
};

