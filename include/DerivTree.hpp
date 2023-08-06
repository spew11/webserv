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
    Module * createClienMaxBodySize();
    Module * createAcceptMethod();
};