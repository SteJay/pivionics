/* 

  String splitting routine, courtesy of Evan Teran from Stack Overflow here:
  http://stackoverflow.com/questions/236129/split-a-string-in-c/236803#236803

*/

#include <string>
#include <vector>
#include <list>

using namespace std;

#ifndef STRINGSPLIT_H
#define STRINGSPLIT_H

vector<string> &split(const string&, char, vector<string>&);
vector<string> split(const string&, char);
list<string> &split_to_list(const string&, char, list<string>&);
list<string> split_to_list(const string&, char);
list<string> get_arguments(const string);
string replace_all(string,string,string);
inline bool file_exists(const string& name) {
    struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}

#endif
