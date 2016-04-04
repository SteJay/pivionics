/*
structure.cpp - Implementation for structure.h
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
	parent=NULL; id_store=0; point_store=NULL; point_count=0;
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
Rotation::Rotation(void) {
	placeholder=1;
}
