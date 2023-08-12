#include "ServerAutoIndexSimulator.hpp"

ServerAutoIndexSimulator::DirectoryEntry::DirectoryEntry(const string & name, const string & date, const size_t & size)
    : name(name), date(date), size(size) {}


string ServerAutoIndexSimulator::DirectoryEntry::getName() const
{
    return name;
}

string ServerAutoIndexSimulator::DirectoryEntry::getDate() const
{
    return date;
}

size_t ServerAutoIndexSimulator::DirectoryEntry::getSize() const
{
    return size;
}

string ServerAutoIndexSimulator::findModifiedDate(const string & name) const
{
    return "2021-12-30";
}

string ServerAutoIndexSimulator::generateAutoIndexHtml(const string & dirName)
{
    std::string html = "<html><head><title>Index of " + dirName + "</title></head><body>";
        html += "<h1>Index of " + dirName + "</h1><hr>";

        html += "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\">";
        html += "<thead><tr><th>Date Modified</th><th>Size</th><th>Name</th></tr></thead>";
        html += "<tbody>";
        
        string link;
        link = "<a href=\"" + dirName + "\">" + dirName + "</a>";
        html += "<tr>";
        html += "<td>" + findModifiedDate(dirName) + "</td>";
        html += "<td>" + "-" + "</td>";
        html += "<td>" + link + "</td>";
        html += "</tr>";
        
        for (DirectoryEntry entry : directory) {
            link = "<a href=\"" + dirName + entry.getName() + "\">" + entry.getName() + "</a>";
            html += "<tr>";
            html += "<td>" + entry.getDate() + "</td>";
            html += "<td>" + Utils::itoa(entry.getSize()) + "</td>";
            html += "<td>" + link + "</td>";
            html += "</tr>";
        }

        html += "</tbody></table><hr></body></html>";
    
    return html;
}
