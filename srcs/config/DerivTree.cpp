#include <DerivTree.hpp>

DerivTree * DerivTree::addSubTree( const DerivTree & tree )
{
	subTree.push_back(tree);
	
	return &(subTree.back());
}

Module * DerivTree::createModule()
{
	if (deriv.name == "main")
		return createMain();
	else if (deriv.name == "server")
		return createServer();
	else if (deriv.name == "location")
		return createLocation();
	else if (deriv.name == "root")
		return createRoot();
	else if (deriv.name == "types")
		return createTypes();
	else if (deriv.name == "index")
		return createIndex();
	else if (deriv.name == "error_page")
		return createErrorPage();
	else if (deriv.name == "cgi")
		return createCgi();
	else if (deriv.name == "cgi_params")
		return createCgiParams();
	else if (deriv.name == "autoindex")
		return createAutoIndex();
	else if (deriv.name == "client_max_body_size")
		return createClienMaxBodySize();
	else if (deriv.name == "accept_method")
		return createAcceptMethod();
	else
		throw runtime_error("Config error: Uknown direction name in the config file");
		
	return NULL;
}

Module * DerivTree::createMain()
{
	Module * m = new MainModule(deriv);

	for (int i = 0; i < subTree.size(); i++)
		m->addModule(subTree[i].createModule());

	return m;
}

Module * DerivTree::createServer()
{
	const Derivative * listenDeriv = NULL;
	const Derivative * serverNameDeriv = NULL;

	vector<DerivTree *> subModDerivs; 

	for (int i = 0; i < subTree.size(); i++) {
		const Derivative & deriv = subTree[i].getDeriv();

		if (deriv.name == "listen") {// listen지시어를 저장
			if (listenDeriv != NULL)
				throw runtime_error("Config error: only one 'listen' directive is allowed within a 'server' block.");
			listenDeriv = &deriv;
		}
		else if (deriv.name == "server_name") {// server_name지시어를 저장
			if (serverNameDeriv != NULL)
				; //throw
			serverNameDeriv = &deriv;
		}
		else // 이외에 지시어는 server모듈의 sub모듈지시어가 된다.
			subModDerivs.push_back(&subTree[i]);
	}

	if (listenDeriv == NULL)
		throw runtime_error("Config error: Missing 'listen' directive within the 'server' block.");

	if (serverNameDeriv == NULL)
		cout << "need serverName\n"; // throw

	// listen, server_name 지시어는 subDerivs에 담아 server모듈 생성자로 들어간다.
	vector<Derivative> subDerivs;

	subDerivs.push_back(*listenDeriv);
	subDerivs.push_back(*serverNameDeriv);

	ServerModule * m = new ServerModule(deriv, subDerivs);

	// listen, server_name 이외에 지시어로 sub모듈을 생성한다.
	for (int i = 0; i < subModDerivs.size(); i++)
		m->addModule(subModDerivs[i]->createModule());

	return m;
}

Module * DerivTree::createLocation()
{
	LocationModule * m = new LocationModule(deriv);

	for (int i = 0; i < subTree.size(); i++)
		m->addModule(subTree[i].createModule());

	return m;
}

Module * DerivTree::createRoot()
{
	return (new RootModule(deriv));
}

Module * DerivTree::createTypes()
{
	vector<Derivative> subDerivs;

	for (int i = 0; i < subTree.size(); i++) {
		subDerivs.push_back(subTree[i].getDeriv());
	}

	return new TypesModule(deriv, subDerivs);
}

Module * DerivTree::createIndex()
{
	return new IndexModule(deriv);
}

Module * DerivTree::createErrorPage()
{
	return new ErrorPageModule(deriv);
}

Module * DerivTree::createCgi()
{
	return new CgiModule(deriv);
}

Module * DerivTree::createCgiParams()
{
	vector<Derivative> subDerivs;

	for (int i = 0; i < subTree.size(); i++) {
		subDerivs.push_back(subTree[i].getDeriv());
	}

	return new CgiParamsModule(deriv, subDerivs);
}

Module * DerivTree::createAutoIndex()
{
	return new AutoIndexModule(deriv);
}

Module * DerivTree::createClienMaxBodySize()
{
	return new ClientMaxBodySizeModule(deriv);
}

Module * DerivTree::createAcceptMethod()
{
	return new AcceptMethodModule(deriv);
}
