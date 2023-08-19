#include "ServerConfig.hpp"

ServerConfig::ServerConfig(const ServerModule &_srvMod)
	: srvMod(&_srvMod), srvNameMod(NULL)
{
	const vector<Module *> &srvSubMods = srvMod->getSubMods();

	// server블록에 정의된 location설정. (location블록에 상속된다.)
	// uri에 맞는 location 블록이 없을때 사용되는 default설정으로도 쓰인다.
	locMap.defaultLocConf.addModules(srvSubMods);

	// location블록에 정의된 module
	for (size_t i = 0; i < srvSubMods.size(); i++)
	{
		if (srvSubMods[i]->getName() == "location")
		{
			LocationModule *locMod = dynamic_cast<LocationModule *>(srvSubMods[i]);
			// casting check

			LocationConfig locConf = locMap.defaultLocConf;
			locConf.addModules(locMod->getSubMods());

			locMap.uriMap[locMod->getUri()] = locConf;
		}
		else if (srvSubMods[i]->getName() == "server_name")
		{
			srvNameMod = dynamic_cast<ServerNameModule *>(srvSubMods[i]);
			// casting check
		}
	}
}

bool ServerConfig::operator==(const string &host) const
{
	const vector<string> &serverNames = getServerNames();

	if (find(serverNames.begin(), serverNames.end(), host) == serverNames.end())
		return false;

	return true;
}

const LocationConfig &ServerConfig::LocationMap::getLocConf(string uri) const
{
	while (true)
	{
		map<string, LocationConfig>::const_iterator it = uriMap.find(uri);

		// uri가 있다면 value반환
		if (it != uriMap.end())
			return it->second;

		// uri가 없다면 '/'기준으로 잘라서 uri를 수정한다.
		size_t slashIdx = uri.find_last_of('/');

		if (slashIdx == string::npos)
			break;

		uri = uri.substr(0, slashIdx);
	}

	return defaultLocConf;
}

uint32_t ServerConfig::getIp(void) const
{
	if (srvMod == NULL) // 생성자에서 반드시 srvMod 초기화 해줘야한다.
		return INADDR_NONE;

	return srvMod->getIp();
}

uint16_t ServerConfig::getPort(void) const
{
	if (srvMod == NULL) // 생성자에서 반드시 srvMod 초기화 해줘야한다.
		return -1;

	return srvMod->getPort();
}

const vector<string> &ServerConfig::getServerNames(void) const
{
	static const vector<string> defaultSrvName; // = 빈 벡터.

	if (srvNameMod == NULL)
		return defaultSrvName;

	return srvNameMod->getServerNames();
}

const ServerConfig::LocationMap &ServerConfig::getLocationMap(void) const
{
	return locMap;
}
