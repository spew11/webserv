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

