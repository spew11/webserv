#include "DirectiveTree.hpp"

DirectiveTree::DirectiveTree(const Directive &directive) : directive(directive) {}

const Directive &DirectiveTree::getDirective(void) const { return this->directive; }

DirectiveTree *DirectiveTree::addSubTree(const DirectiveTree &tree)
{
	subTree.push_back(tree);

	return &(subTree.back());
}

Module *DirectiveTree::createModule()
{
	if (directive.name == "main")
		return createMain();
	else if (directive.name == "server")
		return createServer();
	else if (directive.name == "server_name")
		return createServerName();
	else if (directive.name == "location")
		return createLocation();
	else if (directive.name == "root")
		return createRoot();
	else if (directive.name == "types")
		return createTypes();
	else if (directive.name == "index")
		return createIndex();
	else if (directive.name == "error_page")
		return createErrorPage();
	else if (directive.name == "cgi")
		return createCgi();
	else if (directive.name == "cgi_params")
		return createCgiParams();
	else if (directive.name == "autoindex")
		return createAutoIndex();
	else if (directive.name == "client_max_body_size")
		return createClienMaxBodySize();
	else if (directive.name == "accept_method")
		return createAcceptMethod();
	else if (directive.name == "return")
		return createReturn();
	else
		throw runtime_error("Config error: Uknown directive name in the config file: " + directive.name);

	return NULL;
}

Module *DirectiveTree::createMain()
{
	Module *m = new MainModule(directive);

	try
	{
		for (size_t i = 0; i < subTree.size(); i++)
			m->addModule(subTree[i].createModule());
	}
	catch (const exception &e)
	{
		delete m;
		m = NULL;
		throw runtime_error(e.what());
	}

	return m;
}

Module *DirectiveTree::createServer()
{
	const Directive *listenDirective = NULL;
	vector<DirectiveTree *> subModDirectives;

	for (size_t i = 0; i < subTree.size(); i++)
	{
		const Directive &directive = subTree[i].getDirective();

		if (directive.name == "listen") // listen지시어를 저장
		{
			if (listenDirective != NULL)
				throw runtime_error("Config error: only one 'listen' directive is allowed within a 'server' block.");
			listenDirective = &directive;
		}
		else // 이외에 지시어는 server모듈의 sub모듈지시어가 된다.
			subModDirectives.push_back(&subTree[i]);
	}

	if (listenDirective == NULL)
		throw runtime_error("Config error: Missing 'listen' directive within the 'server' block.");

	// listen 지시어는 subDirectives에 담아 server모듈 생성자로 들어간다.
	vector<Directive> subDirectives;
	subDirectives.push_back(*listenDirective);

	ServerModule *m = new ServerModule(directive, subDirectives);

	// listen이외에 지시어는 sub모듈을 생성한다.
	try
	{
		for (size_t i = 0; i < subModDirectives.size(); i++)
			m->addModule(subModDirectives[i]->createModule());
	}
	catch (const exception &e)
	{
		delete m;
		m = NULL;
		throw runtime_error(e.what());
	};

	return m;
}

Module *DirectiveTree::createServerName()
{
	return new ServerNameModule(directive);
}

Module *DirectiveTree::createLocation()
{
	LocationModule *m = new LocationModule(directive);

	try
	{
		for (size_t i = 0; i < subTree.size(); i++)
			m->addModule(subTree[i].createModule());
	}
	catch (const exception &e)
	{
		delete m;
		m = NULL;
		throw runtime_error(e.what());
	}

	return m;
}

Module *DirectiveTree::createRoot()
{
	return new RootModule(directive);
}

Module *DirectiveTree::createTypes()
{
	vector<Directive> subDirectives;

	for (size_t i = 0; i < subTree.size(); i++)
	{
		subDirectives.push_back(subTree[i].getDirective());
	}

	return new TypesModule(directive, subDirectives);
}

Module *DirectiveTree::createIndex()
{
	return new IndexModule(directive);
}

Module *DirectiveTree::createErrorPage()
{
	return new ErrorPageModule(directive);
}

Module *DirectiveTree::createCgi()
{
	return new CgiModule(directive);
}

Module *DirectiveTree::createCgiParams()
{
	vector<Directive> subDirectives;

	for (size_t i = 0; i < subTree.size(); i++)
	{
		subDirectives.push_back(subTree[i].getDirective());
	}

	return new CgiParamsModule(directive, subDirectives);
}

Module *DirectiveTree::createAutoIndex()
{
	return new AutoIndexModule(directive);
}

Module *DirectiveTree::createClienMaxBodySize()
{
	return new ClientMaxBodySizeModule(directive);
}

Module *DirectiveTree::createAcceptMethod()
{
	return new AcceptMethodModule(directive);
}

Module *DirectiveTree::createReturn()
{
	return new ReturnModule(directive);
}
