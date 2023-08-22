#ifndef ConfigUtils_HPP
#define ConfigUtils_HPP

#include <string>
#include <vector>

using namespace std;

enum ModuleType
{
	NO_TYPE,
	MAIN_MOD,
	SRV_MOD,
	LOC_MOD,
};

enum DirectiveName
{
	ROOT,
};

struct Directive
{
	string name;
	enum DirectiveName e_name;
	vector<string> arg;
	char delim;

	Directive(void) {}
	Directive(const string &name) : name(name) {}
};

#endif