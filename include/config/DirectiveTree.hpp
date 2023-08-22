#ifndef DIRECIVETREE_HPP
#define DIRECIVETREE_HPP

#include "Modules.hpp"
#include "ConfigUtils.hpp"

class DirectiveTree
{
private:
	Directive directive;
	vector<DirectiveTree> subTree;

public:
	DirectiveTree(const Directive &directive);

	DirectiveTree *addSubTree(const DirectiveTree &tree);
	const Directive &getDirective(void) const;
	Module *createModule();

	Module *createMain();
	Module *createServer();
	Module *createServerName();
	Module *createLocation();
	Module *createRoot();
	Module *createTypes();
	Module *createIndex();
	Module *createErrorPage();
	Module *createCgi();
	Module *createCgiParams();
	Module *createAutoIndex();
	Module *createClienMaxBodySize();
	Module *createAcceptMethod();
	Module *createReturn();
};

#endif