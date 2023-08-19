#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include "Utils.hpp"
#include <sstream>

using namespace std;

class Utils 
{
private:
  Utils();
  ~Utils();
public:
  static vector<string> split(const string &s, const string &delim);
  static string ltrim(string s);
  static string rtrim(string s);
  static string trim(string s);
  static string itoa(const int &num);
  static string str_join(vector<string>lines, string delim, int start_idx);
  static bool is_digit_string(string s);
  static string toLowerCase(const string &input);
};
#endif