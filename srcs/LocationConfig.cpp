#include <LocationConfig.hpp>

LocationConfig::LocationConfig( void )
{
	rootMod = NULL;
	indexMod = NULL;
	typesMod = NULL;
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

