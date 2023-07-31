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

    // 지시어트리를 이용해 모듈을 생성
    Module * mainMod = mainDerivTree.createModule();
    // cout << "\n[Modules]" << endl;
    // mainMod->print(0);
    
    return NULL;
}

// 인자로 들어온 tree의 sub트리를 만든다.
void ConfigParser::setTree( DerivTree * tree )
{
    struct Derivative deriv;

    while (getDeriv(deriv))// token을 연결한 지시어(deriv)를 가져온다.
    {
        if (deriv.arg.size() == 0)
            continue;

        switch (deriv.delim) // 지시어 구분자(delim)
        {
        case ';': // 단순히 sub트리를 만든다.
            tree->addSubTree(DerivTree(deriv));
            break;
        case '{': // sub트리를 만들고 그 sub트리를 기준으로 함수를 재귀적으로 실행
            DerivTree * subTree;
            subTree = tree->addSubTree(DerivTree(deriv));
            setTree(subTree);
            break;
        case '}': // 트리가 완성됬으므로 함수를 끝낸다.
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

    // 화이트 스페이스를 기준으로 잘라준다.
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

    // 구분자(delim)이 나올때 까지 vector에 push한다.
    while(it != tokenList.end())
    {
        string token = *it;

        size_t idx;
        // token이 구분자를 가지는지 확인
        if (((idx = token.find(';')) != string::npos)
         || ((idx = token.find('{')) != string::npos)
         || ((idx = token.find('}')) != string::npos))
        {
            // 구분자 이전의 substr을 arg에 넣어준다.
            deriv.arg.push_back(token.substr(0, idx));

            // 이때 구분자가 token의 중간에 있다면 token을 구분자 기준으로 잘라 새로 초기화한다.
            if (idx == token.size() - 1)
                it++;
            else
                *it = token.substr(idx + 1);
            
            // 지시어(deriv)의 변수를 초기화
            deriv.name = deriv.arg[0];
            deriv.delim = token[idx];

            break;
        }

        deriv.arg.push_back(token);
        it++;
    }

    // 사용된 토큰은 삭제한다.
    if (tokenList.begin() != it)
        tokenList.erase(tokenList.begin(), it);

    if (deriv.delim == 0)
        return false;

    return true;
}

