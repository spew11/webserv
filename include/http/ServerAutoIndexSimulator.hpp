#ifndef SERVER_AUTO_INDEX_SIMULATOR
# define SERVER_AUTO_INDEX_SIMULATOR
# include <iostream>
# include <string>
# include <vector>
# include "Utils.hpp"
using namespace std;

class ServerAutoIndexSimulator
{
    private:
        class DirectoryEntry
        {
            private:
                const string & name;
                const string & date;
                const size_t & size;
            public:
                DirectoryEntry(const string & name, const string & date, const size_t & size);
                string getName() const;
                string getDate() const;
                size_t getSize() const;
        };

        vector<ServerAutoIndexSimulator::DirectoryEntry> directory;
        string findModifiedDate(const string & name) const;
        string CreateDirectoryEntries();
    public:
        string generateAutoIndexHtml(const string & path);
};

# endif