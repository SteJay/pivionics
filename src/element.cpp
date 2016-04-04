/*
element.cpp - Implementation for the Element class
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
#include <cmath>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <cstdio>
#include "core_elements.h"
#include "stringsplit.h"

using namespace std;
Element* create_element(void) { return new Element; }

Element::Element(void) {
	geometry[0]=geometry[1]=0.0;
	geometry[2]=geometry[3]=1.0;
	angles[0]=0.0;
	angles[1]=PI*2;
	thick=1.0;
	sect=1;
	subsect=1;
	col=0xFFFFFFFF;
	txt="";
	parent=NULL; id_store=0; 
}

string Element::get_attr(const char* key) {
	string s=""; int n; char c[32];
	if( attrs.find(key) != attrs.end() ) {
		return attrs[key];
	} else {
		return "unknown";
	}
}

void Element::set_attr(const char* key, const char* value) {
	attrs[key]=value;
}

string Element::type(void) { return typestr; }
string Element::name(void) { return namestr; }
void Element::name(string n) { namestr=n; }
Element* Window::add(string s,Element* el) {
	if( creators.find(s)!=creators.end() ) {
		Element* (*fn)();
		fn=creators[s];
		Element* e=fn();
		e->parent=el;
		e->typestr=s;
		if(el != NULL) {
			el->contents.push_back(e);
		} else {
			this->contents.push_back(e);
		}
		return e;
	}
	return NULL;
}

void Element::construct(void) {
	Element* el;
	for(auto iter=contents.begin(); iter!=contents.end(); ++iter) {
		el=*iter;
		el->construct();
	}
}
