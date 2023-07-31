#include <ConfigParser.hpp>

ConfigParser::ConfigParser( const string & configFile )
{
    tokenize(configFile, this->tokenList);
}

Config * ConfigParser::get( void )
{
    DerivTree mainDerivTree(Derivative("main"));
    
    // 지시어들을 트리구조로 저장
    setTree(&mainDerivTree);
    // cout << "[Tree]" << endl;
    // mainDerivTree.print(0);

    return NULL;
}

void ConfigParser::setTree( DerivTree * tree )
{
    struct Derivative deriv;

    while (getDeriv(deriv))
    {
        if (deriv.arg.size() == 0)
            continue;

        switch (deriv.delim)
        {
        case ';':
            tree->addSubTree(DerivTree(deriv));
            break;
        case '{':
            DerivTree * subTree;
            subTree = tree->addSubTree(DerivTree(deriv));
            setTree(subTree);
            break;
        case '}':
            return;
        }
    }
}

void ConfigParser::read( const string & file, string & buffer )
{
    ifstream ifs(file.c_str());

    if (!ifs.is_open())
    {
        throw runtime_error("file open fail!\n");
    }

    string line;
    while (std::getline(ifs, line))
    {
        buffer += line;
    }

    ifs.close();
}

// config파일을 token단위로 쪼개서 저장
void ConfigParser::tokenize( const string & file, list<string> & list )
{
    string buffer;

    ConfigParser::read(file, buffer);

    std::istringstream iss(buffer);

    string token;

    while (iss >> token)
    {
        list.push_back(token);
    }
}

// tokenList에서 지시어를 만들어 반환
bool ConfigParser::getDeriv( Derivative & deriv )
{
    if (tokenList.size() == 0)
        return false;
    
    deriv.name = "";
    deriv.arg.clear();
    deriv.delim = 0;

    list<string>::iterator it = tokenList.begin();

    while(it != tokenList.end())
    {
        string token = *it;

        size_t idx;
        if (((idx = token.find(';')) != string::npos)
         || ((idx = token.find('{')) != string::npos)
         || ((idx = token.find('}')) != string::npos))
        {
            deriv.arg.push_back(token.substr(0, idx));

            if (idx == token.size() - 1)
                it++;
            else
                *it = token.substr(idx + 1);
            
            deriv.name = deriv.arg[0];
            deriv.delim = token[idx];

            break;
        }
        deriv.arg.push_back(token);
        it++;
    }

    if (tokenList.begin() != it)
        tokenList.erase(tokenList.begin(), it);

    if (deriv.delim == 0)
        return false;

    return true;
}

