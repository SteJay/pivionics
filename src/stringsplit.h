/* 

  String splitting routine, courtesy of Evan Teran from Stack Overflow here:
  http://stackoverflow.com/questions/236129/split-a-string-in-c/236803#236803

*/

#include <string>
#include <vector>
#include <list>



#ifndef STRINGSPLIT_H
#define STRINGSPLIT_H

std::vector<std::string> &split(const std::string&, char, std::vector<std::string>&);
std::vector<std::string> split(const std::string&, char);
std::list<std::string> &split_to_list(const std::string&, char, std::list<std::string>&);
std::list<std::string> split_to_list(const std::string&, char);
std::list<std::string> get_arguments(const std::string);
std::string replace_all(std::string,std::string,std::string);
#endif
