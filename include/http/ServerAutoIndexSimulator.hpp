#ifndef SERVER_AUTO_INDEX_SIMULATOR
#define SERVER_AUTO_INDEX_SIMULATOR
#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include "Utils.hpp"

using namespace std;

class ServerAutoIndexSimulator
{
private:
	class DirectoryEntry
	{
	private:
		const string name;
		const string date;
		const string size;

	public:
		DirectoryEntry(const string name, const string date, const string size);
		string getName() const;
		string getDate() const;
		string getSize() const;
		string toString() const;
	};
	vector<ServerAutoIndexSimulator::DirectoryEntry> directory;
	void fillDirectory(const string &dirName);

public:
	string generateAutoIndexHtml(const string &root, const string &filename);
};

#endif