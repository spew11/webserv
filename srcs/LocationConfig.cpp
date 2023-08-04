#include <LocationConfig.hpp>

LocationConfig::LocationConfig( void )
{
	rootMod = NULL;
	indexMod = NULL;
	typesMod = NULL;
	cgiMod = NULL;
	cgiParamsMod = NULL;
	autoIndexMod = NULL;
	errorPageMods.clear();
}

void LocationConfig::addModules( const vector<Module*> & modules )
{
	for (size_t i = 0; i < modules.size(); i++)
	{
		Module * mod = modules[i];

		if (mod->getName() == "root")
        {
            rootMod = dynamic_cast<RootModule *>(mod);
            // check casting
        }
        else if (mod->getName() == "index")
        {
            indexMod = dynamic_cast<IndexModule *>(mod);
            // check casting
        }
        else if (mod->getName() == "types")
        {
            typesMod = dynamic_cast<TypesModule *>(mod);
            // check casting
        }
        else if (mod->getName() == "error_page")
        {
            errorPageMods.push_back(dynamic_cast<ErrorPageModule *>(mod));
            // check casting
        }
		else if (mod->getName() == "cgi")
		{
			cgiMod = dynamic_cast<CgiModule *>(mod);
		}	
		else if (mod->getName() == "cgi_params")
		{
			cgiParamsMod = dynamic_cast<CgiParamsModule *>(mod);
		}
		else if (mod->getName() == "autoindex")
		{
			autoIndexMod = dynamic_cast<AutoIndexModule *>(mod);
		}	
	}
}

const string & LocationConfig::getRoot( void ) const
{
	static const string defaultRoot = "html";

	if (rootMod == NULL)
		return defaultRoot; // default값
	
	return rootMod->getRoot();
}

const vector<string> & LocationConfig::getIndexes( void ) const
{
	static const vector<string> defaultIndexes = vector<string>(1, "index");

	if (indexMod == NULL)
		return defaultIndexes;

	return indexMod->getIndexes();
}

const string & LocationConfig::getType( const string & scriptName ) const
{
	static const string defaultType = "text/html";// 나중에 defaultType모듈로 변경

	size_t dotIdx = scriptName.find_last_of('.');

	if (dotIdx != string::npos)
	{
		string extension = scriptName.substr(dotIdx);

		map<string, string>::const_iterator it = typesMod->getTypesMap().find(extension);

		if (it != typesMod->getTypesMap().end())
			return it->second;
	}

	return defaultType;
}

bool LocationConfig::isErrCode( int code ) const
{
	for (int i = 0; errorPageMods.size(); i++)
	{
		if (errorPageMods[i]->isErrCode( code ))
			return true;
	}

	return false;
}

const string & LocationConfig::getErrPage( int code ) const
{
	static const string defaultErrPage = "";

	for (int i = 0; errorPageMods.size(); i++)
	{
		if (errorPageMods[i]->isErrCode( code ))
			return errorPageMods[i]->getUri();
	}

	return defaultErrPage;
}

bool LocationConfig::isCgi( void ) const
{
	if (cgiMod != NULL)
		return true;
	
	return false;
}

const string & LocationConfig::getCgiCmd( void ) const
{
	static const string defaultCmd = "";

	if (cgiMod != NULL)
		return cgiMod->getCgiCmd();

	return defaultCmd;
}

// mallllllllloc const를 붙여서 반환하는 방법은 모르겟음
char ** LocationConfig::getCgiParams( const EnvironmentValues & env ) const 
{
	const vector<pair<string, string> > & params = cgiParamsMod->getParams();

	char ** paramArr = new char*[params.size() + 1];

	for (int i = 0; i < params.size(); i++)
	{
		const string & key = params[i].first;
		string value = env.replace(params[i].second);

		size_t keySize = params[i].first.size();
		size_t valueSize = value.size();

		paramArr[i] = new char[keySize + valueSize + 2];

		strcpy(paramArr[i], (key + "=" + value).c_str());
	}

	paramArr[params.size() + 1] = NULL;
	
	return paramArr;
}

bool LocationConfig::isAutoIndex( void ) const
{
	if (autoIndexMod == NULL)
		return false;
	
	return autoIndexMod->getAutoIndex();
}

