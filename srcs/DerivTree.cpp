#include <DerivTree.hpp>

DerivTree * DerivTree::addSubTree( const DerivTree & tree )
{
	subTree.push_back(tree);
	
	return &(subTree.back());
}

Module * DerivTree::createModule()
{
	return NULL;
}
