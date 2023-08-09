#include <ServerConfig.hpp>

ServerConfig::ServerConfig( ServerModule & _srvMod )
 : srvMod(&_srvMod),
   srvNameMod(NULL)
{
	LocationConfig defaultLocConf;

	vector<Module *> srvSubMods = srvMod->getSubMods();

	// server블록에 정의되서 locationConfig에 상속될 module;
	defaultLocConf.addModules(srvSubMods);
	
	// location을 만족하지 못하는 uri를 위한 defaultConf
	locMap.insert("", defaultLocConf);

	// location블록에 정의된 module
	for (size_t i = 0; i < srvSubMods.size(); i++)
	{
		if (srvSubMods[i]->getName() == "location")
		{
			LocationModule *locMod = dynamic_cast<LocationModule *>(srvSubMods[i]);
			// casting check

			LocationConfig locConf = defaultLocConf;
			locConf.addModules(locMod->getSubMods());

			locMap.insert(locMod->getUri(), locConf);
		}
		else if (srvSubMods[i]->getName() == "server_name")
		{
			srvNameMod = dynamic_cast<ServerNameModule *>(srvSubMods[i]);
			// casting check
		}
	}
}

const LocationConfig & ServerConfig::LocationMap::getLocConf( string uri ) const
{
	// 빈문자열("")은 만족하는 location이 없을때 반환되는 default설정의 key이다.
	while (uri != "")
	{
		map<string, LocationConfig>::const_iterator it = locConfMap.find(uri);

		// uri가 있다면 value반환
		if (it != locConfMap.end()) {
			return it->second;
		}
		else { // uri가 없다면 '/'기준으로 잘라서 uri를 수정해 찾는다.
			size_t slashIdx = uri.find_last_of('/');

			if (slashIdx == string::npos) {
				uri = "";
			}
			else {
				uri = uri.substr(0, slashIdx);
			}
		}
	}

	return locConfMap.find("")->second;
}

void ServerConfig::LocationMap::insert( const string & key, const LocationConfig & locConf )
{
	locConfMap[key] = locConf;
}

