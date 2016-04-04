#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <iostream>
using namespace std;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}


list<string> &split_to_list(const string &s, char delim, list<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


list<string> split_to_list(const string &s, char delim) {
    list<string> elems;
    split_to_list(s, delim, elems);
    return elems;
}

list<string> get_arguments(string s) {
	int len=s.size();
	bool stringmode=false;
	list<string> args;
	string thisarg="";
	for(int i=0;i<len;++i) {
		if(stringmode) {
			if( (s[i]=='\\' && (i+1<len&&s[i+1]=='"')) || (s[i]==L'\\' && (i+1<len&&s[i+1]==L'"'))) {
				thisarg+='"';
				i++;
			} else if(s[i]=='"' || s[i]==L'"') {
				stringmode=false;
			} else {
				thisarg+=s[i];
			}
		} else {
			if(s[i]=='"' || s[i]==L'"') {
				stringmode=true;
			} else if(s[i]==' ' || s[i]==L' ' || s[i]=='\n' || s[i]==L'\n' || s[i] == '\t' || s[i] == L'\t') {
				if( thisarg.compare("")!=0 ) {
					args.push_back(thisarg);
					thisarg="";
				}
			} else {
				thisarg+=s[i];
			}
		}
	}
	if( thisarg.size()>0 ) args.push_back(thisarg);
	return args;
}

string replace_all( string instr, string find, string repl ) {
	int len=instr.size();
	string t;
	string rs="";
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
