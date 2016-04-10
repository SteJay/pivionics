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
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>
#include "core_elements.h"
#include "stringsplit.h"

using namespace std;
double normalise_angle( double d ) { d=fmod(d,PI*2); if(d<0.0) d+=PI*2; return d; }

Element::Element() {
	id_store=0;
	namestr="Base Element";
	typestr="Element";
	geometry[0]=0.0;
	geometry[1]=0.0;
	geometry[2]=1.0;
	geometry[3]=1.0;
	angles[0]=0;
	angles[1]=PI*2;
	scale[0]=1.0;scale[1]=1.0; // OBSOLETE
	thick=1;
	sect=4;
	subsect=1;
	col=0xFFFFFFFF;
	txt="";
	inherit_position=true;
	inherit_scale=true;
	inherit_angle=true;
	compose_order=COMPOSE_ORDER_SRT; // Scale, Rotate, Translate
	parent=NULL;
	cout << this << " : Element Constructor" << endl;
}
Container::Container() {
	name("");
	geometry[0]=0.0;
	geometry[1]=0.0;
	geometry[2]=1.0;
	geometry[3]=1.0;
	angles[0]=0;
	angles[1]=PI*2;
	scale[0]=1.0;scale[1]=1.0; // OBSOLETE
	thick=1;
	sect=4;
	subsect=1;
	col=0xFFFFFFFF;
	txt="";
	inherit_position=true;
	inherit_scale=true;
	inherit_angle=true;
	compose_order=COMPOSE_ORDER_SRT; // Scale, Rotate, Translate
	parent=NULL;
	cout << this << " : Container Constructor." << endl;
}
Element* fn_create_element(void) { return new Element; }
Element* fn_create_container(void) { return new Container; }
Element::~Element() {
	access.lock();
    Element* e;
	while( !contents.empty()){
		e=contents.back();
		contents.pop_back();
		delete e;
	}
	access.unlock();
}
bool Element::pre_compose(Origin o) { return true; }
bool Element::post_compose(Origin o) { return true; }
bool Element::pre_construct(void) { return true; }
bool Element::post_construct(void) { return true; }
vector<string> Element::get_attrs(void) {
	access.lock();
	vector<string> v;
	for(auto iter=attrs.begin(); iter!=attrs.end(); ++iter) {
		v.push_back(iter->first);
	}
	access.unlock();
	return v;
}

void Element::compose(Origin origin) {
  access.lock();
  if(this->pre_compose(origin)) {
	composed_points.clear();
	if(!inherit_position) { origin.position.x=0.0; origin.position.y=0.0; }
	if(!inherit_scale) { origin.scale.x=1.0; origin.scale.y=1.0; }
	if(!inherit_angle) { origin.angle=0.0; }
	origin.position.x+=geometry[0];
	origin.position.y+=geometry[1];
	origin.scale.x *= scale[0];
	origin.scale.y *= scale[1];
	origin.angle += angles[0];
	Element* el;
	for(auto iter=contents.begin(); iter!=contents.end();++iter) {
		// Now we compose each child element...
		auto el2=*iter;
		el2->compose(origin);
	}
	// Now we compose this element as a whole
	PointSet tps,tps2;
	Point tp,tp2;
	vector<PointSet> tpsv;
	for(auto iter=points.begin(); iter!=points.end(); ++iter) {
		// Iterate through our pointsets
		tps=*iter;
		tps2=tps;
		tps2.points.clear();
		for(auto piter=tps.points.begin(); piter!=tps.points.end(); ++piter) {
			tp=*piter;
			for(int i=0;i<3;++i) {
				switch( (compose_order >> (i*2)) & 3){
				case 1:
					// Scale...
					tp.x=tp.x*origin.scale.x;
					tp.y=tp.y*origin.scale.y;
					break;
				case 3:
					// Translate...
					tp.x+=origin.position.x;
					tp.y+=origin.position.y;
					break;
				case 2:
					// Rotate...
					tp2.x=tp.x*cos(origin.angle)-tp.y*sin(origin.angle);
					tp2.y=tp.x*sin(origin.angle)+tp.y*cos(origin.angle);
					tp=tp2;
					break;
				}
			}
			tps2.points.push_back(tp);
		}
		tpsv.push_back(tps2);
	}
	// Include these points in our final composed_points
	composed_points=tpsv;
	for(auto iter=contents.begin(); iter!=contents.end();++iter) {
		// And add the composed_points of our children that we worked out earlier
		auto el = *iter;
		composed_points.insert(composed_points.end(),el->composed_points.cbegin(),el->composed_points.cend());
		el->composed_points.clear();
	}
  }
  if(this->post_compose(origin)) {
  }
  access.unlock();
  // Composition is good to go!
}

string Element::get_attr(string key) {
	access.lock();
	string s=""; int n; char c[32];
	if( attrs.find(key) != attrs.end() ) {
		access.unlock();
		return attrs[key];
	} else {
		access.unlock();
		return "unknown";
	}
}

void Element::set_attr(string key, string value) {
	access.lock();
	attrs[key]=value;
	access.unlock();
}

string Element::type(void) { return typestr; }
string Element::name(void) { return namestr; }

void Element::name(string n) { namestr=n; }

Element* Window::add(string s,Element* el) {
	access.lock();
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
		access.unlock();
		return e;
	}
	access.unlock();
	return NULL;
}

void Element::construct(void) {
	access.lock();
	Element* el;
	for(auto iter=contents.begin(); iter!=contents.end(); ++iter) {
		el=*iter;
		el->construct();
	}
	access.unlock();
}


