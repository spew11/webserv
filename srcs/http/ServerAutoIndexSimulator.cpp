#include "ServerAutoIndexSimulator.hpp"

ServerAutoIndexSimulator::DirectoryEntry::DirectoryEntry(const string name, const string date, const string size)
	: name(name), date(date), size(size)
{
	
}

string ServerAutoIndexSimulator::DirectoryEntry::getName() const
{
	return name;
}

string ServerAutoIndexSimulator::DirectoryEntry::getDate() const
{
	return date;
}

string ServerAutoIndexSimulator::DirectoryEntry::getSize() const
{
	return size;
}

string ServerAutoIndexSimulator::DirectoryEntry::toString() const
{
	return "name is " + name + "date is " + date + "size is " + size;
}

string ServerAutoIndexSimulator::generateAutoIndexHtml(const string &root, const string &filename)
{
	string absolutePath = root + filename;
	fillDirectory(absolutePath);

	string html = "<html><head><title>Index of " + absolutePath + "</title>";
	html += "<style>"
			"table { width: 100%; table-layout: fixed; }"
			"td, th { width: 33.33%; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }"
			"</style>";
	html += "</head><body>";
	html += "<h1>Index of " + absolutePath + "</h1><hr>";
	html += "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\">";
	html += "<thead><tr><th>Path</th><th>Date</th><th>Size</th></tr></thead>";
	html += "<tbody>";

	for (size_t i = 0; i < directory.size(); i++)
	{
		DirectoryEntry &entry = directory.at(i);
		string path = entry.getName();
		string name = entry.getName();
		if (path == "..")
		{
			name = "Parent directory/";
		}
		string link = "<a href=\"" + filename + path + "\">" + name + "</a>";
		html += "<tr>";
		html += "<td>" + link + "</td>";
		html += "<td>" + entry.getDate() + "</td>";
		html += "<td>" + entry.getSize() + "</td>";
		html += "</tr>";
	}
	html += "</tbody></table><hr></body></html>";

	return html;
}

void ServerAutoIndexSimulator::fillDirectory(const string &dirName)
{
	directory.clear();
	DIR *dirp = opendir(dirName.c_str());
	struct dirent *direntp;

	string childName;
	string date;
	string size;

	while (true)
	{
		direntp = readdir(dirp);
		if (direntp == NULL)
		{
			break;
		}
		else if (string(direntp->d_name) == ".")
		{
			continue;
		}
		childName = string(direntp->d_name);
		struct stat statbuf;
		if (stat((dirName + childName).c_str(), &statbuf) == 0)
		{
			char buffer[20];
			struct tm *timeinfo = localtime(&(statbuf.st_mtime));
			strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
			if (S_ISDIR(statbuf.st_mode))
			{
				size = "-";
			}
			else
			{
				size = Utils::itoa(statbuf.st_size);
			}
			date = string(buffer);
			directory.push_back(DirectoryEntry(childName, date, size));
		}
	}
}
