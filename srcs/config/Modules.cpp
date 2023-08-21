#include "Modules.hpp"

// Module
Module::Module(Directive const &directive, enum ModuleType type)
	: name(directive.name), type(type) {}

Module::~Module()
{
	for (size_t i = 0; i < subMods.size(); ++i)
	{
		delete subMods[i];
		subMods[i] = NULL;
	}
}

Module::syntax_error::syntax_error(const string &directive)
	: runtime_error("Config error: The \"" + directive + "\" directive has an incorrect syntax") {}

const string &Module::getName(void) const { return name; }
const enum ModuleType &Module::getType(void) const { return type; }
const vector<Module *> &Module::getSubMods(void) const { return subMods; }

void Module::addModule(Module *m) { subMods.push_back(m); }

bool Module::isBoolean(const string &str)
{
	if (str == "on" || str == "off")
		return true;

	return false;
}

bool Module::isNumeric(const string &str)
{
	if (str.empty())
		return false;

	for (string::size_type i = 0; i < str.size(); ++i)
	{
		if (!isdigit(str[i]))
			return false;
	}

	return true;
}

bool Module::isNotNumeric(const string &str) { return !isNumeric(str); }

// MainModule
MainModule::MainModule(const Directive &directive) : Module(directive, NO_TYPE) {}

void MainModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)directive;
	(void)subDirectives;
}

// ServerModule
ServerModule::ServerModule(const Directive &directive, const vector<Directive> &subDirectives)
	: Module(directive, NO_TYPE)
{
	checkSyntax(directive, &subDirectives);

	string listenArg = subDirectives[0].arg[1];
	size_t delimIdx = listenArg.find(':');

	if (delimIdx == string::npos) // ':'이 없는 경우 허용하지 않음
		throw syntax_error("server");

	string ipStr = listenArg.substr(0, delimIdx);
	string portStr = listenArg.substr(delimIdx + 1);

	if (!ipStr.empty() && inet_addr(ipStr.c_str()) == INADDR_NONE) // ip주소형식 맞는지 확인
		throw syntax_error("server");

	if (ipStr.empty()) // ip값이 비었다면 모든 ip 허용
		ip = INADDR_ANY;
	else
		ip = ntohl(inet_addr(ipStr.c_str()));

	if (!isNumeric(portStr.c_str())) // port값이 숫자인지 확인
		throw syntax_error("server");

	port = atoi(listenArg.substr(delimIdx + 1).c_str());
}

void ServerModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)directive;
	// subDirectives에는 listen지시어가 들어있다.
	if (subDirectives->size() != 1 || (*subDirectives)[0].arg.size() != 2)
		throw syntax_error("server");
}

uint32_t ServerModule::getIp(void) const { return ip; }
uint16_t ServerModule::getPort(void) const { return port; }

// ServerNameModule
ServerNameModule::ServerNameModule(const Directive &directive) : Module(directive, SRV_MOD)
{
	checkSyntax(directive, NULL);
	// server_name 지시어의 arg가 없다면 serverNames는 빈 벡터이다.
	for (size_t i = 1; i < directive.arg.size(); i++)
		this->serverNames.push_back(directive.arg[i]);
}

void ServerNameModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)directive;
	(void)subDirectives;
}

const vector<string> &ServerNameModule::getServerNames(void) const { return serverNames; }

// LocationModule
LocationModule::LocationModule(const Directive &directive) : Module(directive, NO_TYPE)
{
	checkSyntax(directive, NULL);

	uri = directive.arg[1];

	if (uri.size() > 0 && uri[uri.size() - 1] == '/')
		uri.erase(uri.end() - 1);
}

void LocationModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)subDirectives;

	if (directive.arg.size() != 2)
		throw syntax_error("location");
}

const string &LocationModule::getUri(void) const { return uri; }

// RootModule
RootModule::RootModule(const Directive &directive) : Module(directive, LOC_MOD)
{
	checkSyntax(directive, NULL);

	root = directive.arg[1];
}

void RootModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)subDirectives;

	if (directive.arg.size() != 2)
		throw syntax_error("root");
}

const string &RootModule::getRoot(void) const { return root; }

// TypesModule
TypesModule::TypesModule(const Directive &directive, vector<Directive> subDirectives)
	: Module(directive, LOC_MOD)
{
	checkSyntax(directive, &subDirectives);

	for (size_t i = 0; i < subDirectives.size(); i++)
	{
		string type = subDirectives[i].arg[0];

		for (size_t j = 1; j < subDirectives[i].arg.size(); j++)
		{
			string extension = subDirectives[i].arg[j];
			typesMap[extension] = type;
		}
	}
}

void TypesModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)directive;

	for (size_t i = 0; i < subDirectives->size(); i++)
	{
		if ((*subDirectives)[i].arg.size() < 2)
			throw syntax_error("types");
	}
}

const map<string, string> &TypesModule::getTypesMap(void) const { return typesMap; }

// IndexModule
IndexModule::IndexModule(const Directive &directive) : Module(directive, LOC_MOD)
{
	checkSyntax(directive, NULL);

	// directive.arg가 비었다면 indexes벡터도 빈 벡터이다.
	for (size_t i = 1; i < directive.arg.size(); i++)
		indexes.push_back(directive.arg[i]);
}

void IndexModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)directive;
	(void)subDirectives;
}

const vector<string> &IndexModule::getIndexes(void) const { return indexes; }

// ErrorPageModule
ErrorPageModule::ErrorPageModule(const Directive &directive) : Module(directive, LOC_MOD)
{
	checkSyntax(directive, NULL);

	for (size_t i = 1; i < directive.arg.size() - 1; i++)
		errCodes.push_back(atoi(directive.arg[i].c_str()));

	uri = directive.arg.back();
}

void ErrorPageModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)subDirectives;

	if (directive.arg.size() < 3)
		throw syntax_error("error_page");

	// arg의 마지막을 제외한 인자에 숫자가 아닌 string이 있다면 에러.
	if (find_if(directive.arg.begin() + 1, directive.arg.end() - 1, isNotNumeric) != directive.arg.end() - 1)
		throw syntax_error("error_page");
}

bool ErrorPageModule::isErrCode(int code) const
{
	if (find(errCodes.begin(), errCodes.end(), code) != errCodes.end())
		return true;

	return false;
}
const string &ErrorPageModule::getUri(void) const { return uri; }

// CgiModule
CgiModule::CgiModule(const Directive &directive) : Module(directive, LOC_MOD)
{
	checkSyntax(directive, NULL);

	cgiType = directive.arg[1];
}

void CgiModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)subDirectives;

	if (directive.arg.size() != 2)
		throw syntax_error("cgi");
}

const string &CgiModule::getCgi(void) const { return cgiType; }

// CgiParamsModule
CgiParamsModule::CgiParamsModule(const Directive &directive, const vector<Directive> &subDirectives) : Module(directive, LOC_MOD)
{
	checkSyntax(directive, &subDirectives);

	for (size_t i = 0; i < subDirectives.size(); i++)
	{
		params.push_back(make_pair(subDirectives[i].arg[0], subDirectives[i].arg[1]));
	}
}

void CgiParamsModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)directive;

	for (size_t i = 0; i < subDirectives->size(); i++)
	{
		if ((*subDirectives)[i].arg.size() != 2)
			throw syntax_error("cgi_params");
	}
}

const vector<pair<string, string> > &CgiParamsModule::getParams(void) const { return params; }

// AutoIndexModule
AutoIndexModule::AutoIndexModule(const Directive &directive) : Module(directive, LOC_MOD)
{
	checkSyntax(directive, NULL);

	if (directive.arg[1] == "on")
		isAutoIndex = true;
	else
		isAutoIndex = false;
}

void AutoIndexModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)subDirectives;

	if (directive.arg.size() != 2 || !isBoolean(directive.arg[1]))
		throw syntax_error("autoindex");
}

bool AutoIndexModule::getAutoIndex(void) const { return isAutoIndex; }

// ClientMaxBodySizeModule
ClientMaxBodySizeModule::ClientMaxBodySizeModule(const Directive &directive) : Module(directive, LOC_MOD)
{
	checkSyntax(directive, NULL);

	maxSize = static_cast<size_t>(atoi(directive.arg[1].c_str()));
}

void ClientMaxBodySizeModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)subDirectives;

	if (directive.arg.size() != 2 || !isNumeric(directive.arg[1]))
		throw syntax_error("client_max_body_size");
}

size_t ClientMaxBodySizeModule::getClientMaxBodySize(void) const { return maxSize; }

// AcceptMethodModule
AcceptMethodModule::AcceptMethodModule(const Directive &directive) : Module(directive, LOC_MOD)
{
	// accept_method 지시어 arg가 비었다면 methods벡터는 빈 벡터.
	for (size_t i = 1; i < directive.arg.size(); i++)
	{
		methods.push_back(directive.arg[i]);
	}
}

void AcceptMethodModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)directive;
	(void)subDirectives;
}

const vector<string> &AcceptMethodModule::getAcceptMethods(void) const { return methods; }

// ReturnModule
ReturnModule::ReturnModule(const Directive &directive) : Module(directive, LOC_MOD)
{
	checkSyntax(directive, NULL);

	statusCode = atoi(directive.arg[1].c_str());
	uri = directive.arg[2];
}

void ReturnModule::checkSyntax(const Directive &directive, const vector<Directive> *subDirectives)
{
	(void)subDirectives;

	if (directive.arg.size() != 3 || !isNumeric(directive.arg[1]))
		throw syntax_error("return");
}

int ReturnModule::getStatusCode(void) const { return statusCode; }
const string &ReturnModule::getUri(void) const { return uri; }
