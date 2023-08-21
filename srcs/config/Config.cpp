#include "Config.hpp"

Config::Config(const string &fileName) : mainMod(NULL)
{
	Config::Parser parser(fileName);

	mainMod = parser.getMainModule();

	setSrvConf();
}

void Config::setSrvConf(void)
{
	vector<Module *> mainSubMods = mainMod->getSubMods();

	for (size_t i = 0; i < mainSubMods.size(); i++)
	{
		Module *subMod = mainSubMods[i];

		if (subMod->getName() == "server")
		{
			ServerModule *srvMod = dynamic_cast<ServerModule *>(subMod);
			// check casting
			srvConfs.push_back(ServerConfig(*srvMod));
		}
	}
}

Config::~Config()
{
	delete mainMod;
	mainMod = NULL;
}

Config::Parser::Parser(const string &configFile) : mainTree(DirectiveTree(Directive("main")))
{
	tokenize(configFile, this->tokenList);
}

Module *Config::Parser::getMainModule(void)
{
	// 지시어들을 트리구조로 저장
	setTree();

	// 지시어트리를 이용해 모듈을 생성
	Module *mainMod = mainTree.createModule();

	return mainMod;
}

// 인자로 들어온 tree의 sub트리를 만든다.
void Config::Parser::setTree(void)
{
	blocks.push(&mainTree);

	Directive directive;

	while (createDirective(directive)) // tokenList에서 token을 연결해 지시어(directive)를 가져온다. 없으면 false
	{
		switch (directive.delim) // 지시어 구분자(delim)
		{
		case ';': // 단순히 sub트리를 만든다.
			if (!directive.name.empty())
				blocks.top()->addSubTree(DirectiveTree(directive));
			break;

		case '{': // sub트리를 만들고 그 sub트리를 스택에 추가해 blocks.top()을 바꾼다
			if (directive.name.empty())
				throw runtime_error("Config error: The '{' symbol should be used together with a directive.");

			DirectiveTree *subTree;
			subTree = blocks.top()->addSubTree(DirectiveTree(directive));
			blocks.push(subTree);
			break;

		case '}':
			if (blocks.size() == 1) // (size == 1)은 mainblock만 있는 상태이다.
				throw runtime_error("Config error: Unmatched '}' in the config file.");
			if (!directive.name.empty())
				throw runtime_error("Config error: The '}' symbol is used in place of ';'.");
			blocks.pop();
		}
	}
	
	if (blocks.size() != 1)
		throw runtime_error("Config error: Missing '}' in the config file.");
}

void Config::Parser::read(const string &file, string &buffer)
{
	ifstream ifs(file.c_str());

	if (!ifs.is_open())
	{
		throw runtime_error("Config error: open failed.");
	}

	string line;
	while (getline(ifs, line))
	{
		buffer += line;
	}

	ifs.close();
}

// config파일을 token단위로 쪼개서 저장
void Config::Parser::tokenize(const string &file, list<string> &list)
{
	string buffer;

	Config::Parser::read(file, buffer);

	istringstream iss(buffer);

	string token;

	// 화이트 스페이스를 기준으로 잘라준다.
	while (iss >> token)
	{
		list.push_back(token);
	}
}

// tokenList에서 지시어를 만들어 반환
bool Config::Parser::createDirective(Directive &directive)
{
	if (tokenList.size() == 0)
		return false;

	directive.name = "";
	directive.arg.clear();
	directive.delim = 0;

	list<string>::iterator it = tokenList.begin();

	// 구분자(delim)이 나올때 까지 list에 push한다.
	while (it != tokenList.end())
	{
		string token = *it;

		size_t idx;
		// token이 구분자를 가지는지 확인
		if ((idx = token.find_first_of(";{}")) != string::npos)
		{
			// 구분자 이전의 substr을 arg에 넣어준다.
			if (idx != 0)
				directive.arg.push_back(token.substr(0, idx));

			// 이때 구분자가 token의 중간에 있다면 token을 구분자 기준으로 잘라 새로 초기화한다.
			if (idx == token.size() - 1)
				it++;
			else
				*it = token.substr(idx + 1);

			if (directive.arg.size() > 0)
				directive.name = directive.arg[0];
			directive.delim = token[idx];

			break;
		}

		directive.arg.push_back(token);
		it++;
	}

	// 사용된 토큰은 삭제한다.
	if (tokenList.begin() != it)
		tokenList.erase(tokenList.begin(), it);

	if (directive.delim == 0)
		throw runtime_error("Config error: Missing ';' or '}' in the config file.");

	return true;
}
