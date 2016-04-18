#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <iostream>


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}


std::list<std::string> &split_to_list(const std::string &s, char delim, std::list<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::list<std::string> split_to_list(const std::string &s, char delim) {
    std::list<std::string> elems;
    split_to_list(s, delim, elems);
    return elems;
}

std::list<std::string> get_arguments(std::string s) {
	int len=s.size();
	bool stringmode=false;
	std::list<std::string> args;
	std::string thisarg="";
	for(int i=0;i<len;++i) {
		if(stringmode) {
			if( (s[i]=='\\' && (i+1<len&&s[i+1]=='"')) || (s[i]==L'\\' && (i+1<len&&s[i+1]==L'"'))) {
				thisarg+='"';
				i++;
			} else if(s[i]=='"' || s[i]==L'"') {
				stringmode=false;
				args.push_back(thisarg);
				thisarg="";
			} else {
				thisarg+=s[i];
			}
		} else {
			if(s[i]=='"' || s[i]==L'"') {
				stringmode=true;
			} else if(s[i]==' ' || s[i]==L' ' || s[i]=='\n' || s[i]==L'\n' || s[i] == '\t' || s[i] == L'\t') {
				//if( thisarg.compare("")!=0 ) {
					args.push_back(thisarg);
					thisarg="";
				//}
			} else {
				thisarg+=s[i];
			}
		}
	}
	if( thisarg.size()>0 ) args.push_back(thisarg);
	return args;
}

std::string replace_all( std::string instr, std::string find, std::string repl ) {
	int len=instr.size();
	std::string t;
	std::string rs="";
	for(int i=0;i<len;++i) {
		t="";
		for(int j=0;j<find.size();j++) {
			t+=instr[i+j];
		}
		if(t.compare(find)==0) {
			rs+=repl;
			i+=find.size()-1;
		} else {
			rs+=instr[i];
		}
	}
	return rs;
}
