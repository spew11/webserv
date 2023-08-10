#include <LocationConfig.hpp>

LocationConfig::LocationConfig( void )
{
	rootMod = NULL;
	indexMod = NULL;
	typesMod = NULL;
	cgiMod = NULL;
	cgiParamsMod = NULL;
	autoIndexMod = NULL;
	cliMaxBodyMod = NULL;
	acceptMethodMod = NULL;
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
		else if (mod->getName() == "client_max_body_size")
		{
			cliMaxBodyMod = dynamic_cast<ClientMaxBodySizeModule *>(mod);
		}	
		else if (mod->getName() == "accept_method")
		{
			acceptMethodMod = dynamic_cast<AcceptMethodModule *>(mod);
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

	if (typesMod == NULL)
		return defaultType;

	size_t dotIdx = scriptName.find_last_of('.');

	if (dotIdx != string::npos && dotIdx != scriptName.size() - 1)
	{
		string extension = scriptName.substr(dotIdx + 1);

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
	if (cgiMod == NULL)
		return false;
	
	return cgiMod->getCgi();
}

// mallllllllloc const를 붙여서 반환하는 방법은 모르겟음
char ** LocationConfig::getCgiParams( const WebservValues & env ) const 
{
	if (cgiParamsMod == NULL)
	{
		char ** paramArr = new char*[1];
		*paramArr = NULL;
		
		return paramArr;
	}
		
	const vector<pair<string, string> > & params = cgiParamsMod->getParams();

	char ** paramArr = new char*[params.size() + 1];

	for (int i = 0; i < params.size(); i++)
	{
		const string & key = params[i].first;
		string value = env.convert(params[i].second);

		size_t keySize = params[i].first.size();
		size_t valueSize = value.size();

		paramArr[i] = new char[keySize + valueSize + 2];

		strcpy(paramArr[i], (key + "=" + value).c_str());
	}

	paramArr[params.size()] = NULL;
	
	return paramArr;
}

bool LocationConfig::isAutoIndex( void ) const
{
	if (autoIndexMod == NULL)
		return false;
	
	return autoIndexMod->getAutoIndex();
}

int LocationConfig::getClientMaxBodySize( void ) const
{
	if (cliMaxBodyMod == NULL)
		return 1024;

	return cliMaxBodyMod->getClientMaxBodySize();
}

const vector<string> & LocationConfig::getAcceptMethods( void ) const
{
	const static vector<string> defaultMethod = vector<string>(1, "GET");

	if (acceptMethodMod == NULL)
		return defaultMethod;

	return acceptMethodMod->getAcceptMethods();
}
