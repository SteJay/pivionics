/*
window.cpp - Implementation for the Window class
Copyright (C) 2016 Stephen J Sullivan

This file is a part of Pivionics.

Pivionics is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pivionics is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pivionics.  If not, see <http://www.gnu.org/licenses/>.

*/
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <cstdio>
#include "structure.h"
#include "stringsplit.h"

using namespace std;

string Window::command(string cmd, Element* base) {
	string rs="";
	for(auto iter=cmd.begin();iter!=cmd.end();++iter){
		if( *iter == '\n' || *iter == L'\n') {
			rs+=" \n ";
		} else {
			rs+=*iter;
		}
	}
	cmd=rs;
	//cout << "Command being processed:" << endl << rs << endl << "Here we go..." << endl;
	list<string> args = split_to_list(cmd,L' ');
	args = command(args,base);
	rs="";
	if(args.size()>0) {
		for(auto iter=args.begin();iter!=args.end();++iter){
			rs += *iter + " ";
		}
	}
	return rs;
}

list<string> Window::command(list<string> args,Element* base) {
	string cmd;
	// First we split the command apart to extract the different parts
	if( base==NULL ) base=this;
	Element* last_added=NULL;
	//cout << "We're starting out the command bit with " << args.size() << " arguments..." << endl;
	while( args.size()>0 ) {
		cmd=args.front();
		args.pop_front();
		// Here's the horrible if elseif elsif times inifinity bit:
	//cout << "Command is " << cmd << " and there are " << args.size() << " arguments remaining..." << endl;
		if(cmd.compare("add")==0||cmd.compare("new")==0) {
			if( args.size()>0 ) {
				last_added=add(args.front(),base);
				args.pop_front();
			} else {
				args.push_front("ERROR: No element type given for add/new");
				return args;
			}
		} else if(cmd.compare("name")==0||cmd.compare("rename")==0) {
			if( args.size()>0 ) {
				base->name(args.front());
				args.pop_front();
			} else {
				args.push_front("ERROR: No name specified!");
				return args;
			}
		} else if(cmd.compare("remove")==0||cmd.compare("delete")==0) {
		} else if(cmd.compare("set")==0||(args.size()>0 && args.front().compare("=")==0)) {
			if(args.size()>=2) {
				string item,val;
				if(args.front().compare("=")==0) { // Yes, I could have stored the value, but performance isn't of the essence here
					item=cmd;
				} else {
					item=args.front();
				}
				args.pop_front();
				if( args.front().compare("pi")==0 || args.front().compare("rad")==0 || args.front().compare("radian")==0 || args.front().compare("radians")==0) {
					if( args.size()>1) {
						args.pop_front();
						val = to_string( stold(args.front(),nullptr) * PI );
						args.pop_front();
					}
				} else if( args.front().compare("deg") ==0 || args.front().compare("degree")==0 || args.front().compare("degrees")==0) {
					if( args.size()>1) {
						args.pop_front();
						val = to_string( stold(args.front(),nullptr) * ( PI / 180.0 ) );
						args.pop_front();
					}
				} else {
					val=args.front();
					args.pop_front();
				}
				cout << "Setting " << item << " to " << val << "..." << endl;
				if(      item.compare("cx")         ==0 ) base->cx(stod(val,nullptr));
				else if( item.compare("cy")         ==0 ) base->cy(stod(val,nullptr));
				else if( item.compare("width")      ==0 ) base->width(stod(val,nullptr));
				else if( item.compare("height")     ==0 ) base->height(stod(val,nullptr));
				else if( item.compare("angle")      ==0 ) base->angle(stold(val,nullptr));
				else if( item.compare("arc")        ==0 ) base->arc(stold(val,nullptr));
				else if( item.compare("thickness")  ==0 ) base->thickness(stoi(val,nullptr,0));
				else if( item.compare("sections")   ==0 ) base->sections(stoi(val,nullptr,0));
				else if( item.compare("subsections")==0 ) base->subsections(stoi(val,nullptr,0));
				else if( item.compare("color")      ==0 ) base->color(stoul(val,nullptr,0));
			}
		} else if(cmd.compare("attr")==0||cmd.compare("attribute")==0) {
		} else if(cmd.compare("{")==0||cmd.compare("{")==0) {
			// Go into the last added element and perform following commands on that
			if( last_added!=NULL) {
				args = command(args,last_added);
				//cout << "We returned successfully!" << endl;
				last_added=NULL;
			}
		} else if(cmd.compare("}")==0||cmd.compare("}")==0) {
		    // Go back out of our element.
			args.push_front(" ");
			break;
		//} else if(cmd.compare("")==0||cmd.compare("")==0) {
		} else {
			//args.push_front("ERROR: Unrecognised Command");
			//return args;
		}
	}
	//cout << "Reached the end - lets try and return!" << endl;
	return args;
}

Window::~Window() {
	Element* e;
	while( !contents.empty()){
		e=contents.back();
		contents.pop_back();
		delete e;
	}
}
int Window::children(Element* el) {
	if( el == NULL ) {
		return static_cast<int>(contents.size());
	} else {
		return static_cast<int>(el->contents.size());
	}
}
list<string> Window::list_creators(void) {
	list<string> l;
	for(auto iter=creators.begin();iter!=creators.end();++iter) {
		l.push_back(iter->first);
	}
	return l;
}

list<Element*> Window::list_elements(Element* pel) {
	list<Element*> c;
	if( pel != NULL) {
		c=pel->contents;
	} else {
		c=this->contents;
	}
	return c;
}

Element* Window::parent(Element* el) {
	if( el != NULL ) {
		return el->parent;
	}
	return NULL;
}

Element* Window::sibling(unsigned int i, Element* el) {
	if( el != NULL) {
		Element* p=el->parent;
		if( p!=NULL) {
			auto iter=p->contents.begin();
			advance(iter,i);
			return *iter;
		} else {
			auto iter=this->contents.begin();
			advance(iter,i);
			return *iter;
		}
	}
	return NULL;
}

Element* Window::child(unsigned int i, Element* el) {
	
	if( el != NULL) {
		auto iter=el->contents.begin();
		advance(iter,i);
		return *iter;
	} else {
		auto iter=this->contents.begin();
		advance(iter,i);
		return *iter;
	}
	return NULL;
}

Element* Window::find_name(string n,int ignore=0) {
	string t;
	Element* e;
	for(auto iter=contents.begin(); iter!=contents.end(); ++iter) {
		
		e= *iter;
		t=e->name();
		if(n.compare(t)==0) if(ignore>0) --ignore; else return *iter;
	}
	return NULL;
}







void Window::load(string fn) {
	ifstream file(fn,ios::in|ios::binary|ios::ate);
	if(file.is_open()) {
		streampos size=file.tellg();
		char buf[8];
		
	}
	file.close();
}

void Window::save(string fn) {
	//Oh boy. Here we go.
	
}

void Window::xport(Element* el, string fn) {

}

Element* Window::import(string fn) {
	Element* el;

	return el;
}
