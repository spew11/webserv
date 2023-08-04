#pragma once

#include <Modules.hpp>
#include <ConfigUtils.hpp>

class DerivTree
{
private:
    Derivative deriv;
    vector<DerivTree> subTree;

public:
    DerivTree( const Derivative & deriv ) : deriv(deriv) {}

    DerivTree * addSubTree( const DerivTree & tree );
    const Derivative & getDeriv( void ) const { return this->deriv; }
    Module * createModule();

    Module * createMain();
    Module * createServer();
    Module * createLocation();
    Module * createRoot();
    Module * createTypes();
    Module * createIndex();
    Module * createErrorPage();
    Module * createCgi();
    Module * createCgiParams();
    Module * createAutoIndex();

    void print(int indent)
    {
        for (int i = 0; i < indent ; i++)
            cout << " ";
        cout << deriv.name << endl;

        for (size_t i = 0; i < subTree.size(); i++)
            subTree[i].print(indent + 2);
    }
};