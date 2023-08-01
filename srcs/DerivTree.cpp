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
	else
		; // throw
		
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
				; //throw
			listenDeriv = &deriv;
		}
		else if (deriv.name == "server_name") {// server_name지시어를 저장
			if (serverNameDeriv != NULL)
				; //throw
			serverNameDeriv = &deriv;
		}
		else // 이외에 지시어는 sub모듈지시어가 된다.
			subModDerivs.push_back(&subTree[i]);
	}

	if (listenDeriv == NULL)
		cout << "need listen\n"; // throw

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

