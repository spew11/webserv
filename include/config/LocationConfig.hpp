#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "Modules.hpp"
#include "WebservValues.hpp"

class LocationConfig
{
private:
	RootModule *rootMod;
	IndexModule *indexMod;
	TypesModule *typesMod;
	CgiModule *cgiMod;
	CgiParamsModule *cgiParamsMod;
	AutoIndexModule *autoIndexMod;
	AcceptMethodModule *acceptMethodMod;
	ClientMaxBodySizeModule *cliMaxBodyMod;
	vector<ErrorPageModule *> errorPageMods;
	ReturnModule *returnMod;

public:
	LocationConfig(void);

	void addModules(const vector<Module *> &);

	bool isErrCode(int code) const;
	bool isCgi(void) const;
	bool isAutoIndex(void) const;
	bool isRedirect(void) const;

	size_t getClientMaxBodySize(void) const;
	int getRedirectStatusCode(void) const;

	const string &getRoot(void) const;
	const string &getType(const string &scriptName) const;
	const string &getErrPage(int code) const;
	const string &getCgi(void) const;
	const string &getRedirectUri(void) const;

	const vector<string> &getIndexes(void) const;
	const vector<string> &getAcceptMethods(void) const;

	char **getCgiParams(const WebservValues &) const;
};

#endif