#include <ConfigParser.hpp>

ConfigParser::ConfigParser( const string & configFile )
{
    tokenize(configFile, this->tokenList);
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
