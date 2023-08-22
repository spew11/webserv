#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include "Modules.hpp"
#include "ServerConfig.hpp"
#include "DirectiveTree.hpp"
#include "ConfigUtils.hpp"

using namespace std;
class Config
{
private:
	class Parser
	{
	private:
		list<string> tokenList;
		stack<DirectiveTree *> blocks;
		DirectiveTree mainTree;

		void read(const string &file, string &buffer);
		void tokenize(const string &file, list<string> &list);
		bool createDirective(Directive &);
		void setTree(void);

	public:
		Parser(const string &file);

		Module *getMainModule(void);
	};

	Module *mainMod;
	vector<ServerConfig> srvConfs;

	void setSrvConf(void);
public:
	Config(const string &);
	~Config();

	const vector<ServerConfig> &getSrvConf(void) const { return srvConfs; }
};

#endif