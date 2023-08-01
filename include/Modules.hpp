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

class MainModule : public Module
{
public:
	MainModule( const Derivative & deriv ) : Module(deriv, NO_TYPE) {}
};

class ServerModule : public Module
{
private:
    friend class DerivTree;
    uint32_t  ip;
    int     port;
    vector<string> serverNames;

    void setListen( const Derivative & deriv )
    {
        // check syntax
        size_t delimIdx = deriv.arg[1].find(':');
        

        ip = ipToInt(deriv.arg[1].substr(0, delimIdx));
        port = atoi(deriv.arg[1].substr(delimIdx + 1).c_str());
    }

    void setServerName( const Derivative & deriv )
    {
        // check syntax
        for (size_t i = 1; i < deriv.arg.size(); i++)
            this->serverNames.push_back(deriv.arg[i]);
    }

    uint32_t ipToInt(const std::string& ip) {
    char ipCStr[16];
    std::strncpy(ipCStr, ip.c_str(), sizeof(ipCStr));
    ipCStr[sizeof(ipCStr) - 1] = '\0'; // 문자열 끝에 null 문자 추가

    int part1, part2, part3, part4;
    if (std::sscanf(ipCStr, "%d.%d.%d.%d", &part1, &part2, &part3, &part4) != 4) {
        // 잘못된 형식의 IP 주소를 입력받은 경우 예외 처리
        throw std::invalid_argument("Invalid IP address format.");
    }

    if (part1 < 0 || part1 > 255 || part2 < 0 || part2 > 255 ||
        part3 < 0 || part3 > 255 || part4 < 0 || part4 > 255) {
        // 유효하지 않은 IP 주소 부분이 입력된 경우 예외 처리
        throw std::invalid_argument("Invalid IP address value.");
    }

    uint32_t result = (static_cast<uint32_t>(part1) << 24) |
                      (static_cast<uint32_t>(part2) << 16) |
                      (static_cast<uint32_t>(part3) << 8) |
                      static_cast<uint32_t>(part4);

    return result;
}
public:
	ServerModule( const Derivative & deriv, vector<Derivative> subDerivs )
     : Module(deriv, NO_TYPE)
    {
        setListen(subDerivs[0]);
        setServerName(subDerivs[1]);
    }
    
    const uint32_t & getIp( void ) const { return ip; }
    const vector<string> & getServerNames( void ) const { return serverNames; }
    int getPort( void ) const { return port; }

    virtual void print(int indent)
    {
        for (int i = 0; i < indent ; i++)
            cout << " ";

        cout << name << " -> ip(" << ip << "), port(" << port << "), name(";
        for (size_t i = 0; i < serverNames.size(); i++)
            cout << serverNames[i] << ", ";
        cout << ")" << endl;

        for (size_t i = 0; i < subMods.size(); i++)
            subMods[i]->print(indent + 2);
    }
};

