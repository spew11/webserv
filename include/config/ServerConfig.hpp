#pragma once

#include <algorithm>
#include <Modules.hpp>
#include <LocationConfig.hpp>

class ServerConfig
{
public:
    ServerConfig( ServerModule &srvMod );
    bool operator==( const string & host ) const;

    class LocationMap
    {
    private:
        map<string, LocationConfig> locConfMap;
    public:
        void insert( const string &, const LocationConfig &);
        const LocationConfig &getLocConf( string uri ) const;
    };

    const uint32_t & getIp( void ) const;
    int getPort( void ) const;
    const vector<string> & getServerNames( void ) const;
    const LocationMap & getLocationMap( void ) const;
private:
    ServerModule        *srvMod;
    ServerNameModule    *srvNameMod;
    LocationMap         locMap;
};

