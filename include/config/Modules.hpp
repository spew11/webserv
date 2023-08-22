#ifndef MODULES_HPP
#define MODULES_HPP

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <stdint.h>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include <arpa/inet.h>

#include "ConfigUtils.hpp"

using namespace std;

class Module
{
protected:
	const string name;
	const enum ModuleType type;
	vector<Module *> subMods;

	class syntax_error : public runtime_error
	{
	public:
		syntax_error(const string &directive);
	};

	static bool isBoolean(const string &str);
	static bool isNumeric(const string &str);
	static bool isNotNumeric(const string &str);

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives) = 0;

public:
	Module(Directive const &directive, enum ModuleType type);
	virtual ~Module();

	const string &getName(void) const;
	const enum ModuleType &getType(void) const;
	const vector<Module *> &getSubMods(void) const;

	void addModule(Module *m);
};

class MainModule : public Module
{
private:
	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	MainModule(const Directive &directive);
};

class ServerModule : public Module
{
private:
	uint32_t ip;
	uint16_t port;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	ServerModule(const Directive &directive, const vector<Directive> &subDirectives);

	uint32_t getIp(void) const;
	uint16_t getPort(void) const;
};

class ServerNameModule : public Module
{
private:
	vector<string> serverNames;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	ServerNameModule(const Directive &directive);

	const vector<string> &getServerNames(void) const;
};

class LocationModule : public Module
{
private:
	string uri;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	LocationModule(const Directive &directive);

	const string &getUri(void) const;
};

class RootModule : public Module
{
private:
	string root;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	RootModule(const Directive &directive);

	const string &getRoot(void) const;
};

class TypesModule : public Module
{
private:
	map<string, string> typesMap;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	TypesModule(const Directive &directive, vector<Directive> subDirectives);

	const map<string, string> &getTypesMap(void) const;
};

class IndexModule : public Module
{
private:
	vector<string> indexes;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	IndexModule(const Directive &directive);

	const vector<string> &getIndexes(void) const;
};

class ErrorPageModule : public Module
{
private:
	vector<int> errCodes;
	string uri;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	ErrorPageModule(const Directive &directive);

	bool isErrCode(int code) const;
	const string &getUri(void) const;
};

class CgiModule : public Module
{
private:
	string cgiType;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	CgiModule(const Directive &directive);

	const string &getCgi(void) const;
};

class CgiParamsModule : public Module
{
private:
	vector<pair<string, string> > params;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	CgiParamsModule(const Directive &directive, const vector<Directive> &subDirectives);

	const vector<pair<string, string> > &getParams(void) const;
};

class AutoIndexModule : public Module
{
private:
	bool isAutoIndex;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	AutoIndexModule(const Directive &directive);

	bool getAutoIndex(void) const;
};

class ClientMaxBodySizeModule : public Module
{
private:
	size_t maxSize;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	ClientMaxBodySizeModule(const Directive &directive);

	size_t getClientMaxBodySize(void) const;
};

class AcceptMethodModule : public Module
{
private:
	vector<string> methods;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	AcceptMethodModule(const Directive &directive);

	const vector<string> &getAcceptMethods(void) const;
};

class ReturnModule : public Module
{
private:
	int statusCode;
	string uri;

	virtual void checkSyntax(const Directive &directive, const vector<Directive> *subDirectives);

public:
	ReturnModule(const Directive &directive);

	int getStatusCode(void) const;
	const string &getUri(void) const;
};

#endif