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


double normalise_angle( double d ) { d=fmod(d,PI*2); if(d<0.0) d+=PI*2; return d; }
long double normalise_angle( long double d ) { d=fmod(d,PI*2.0L); if(d<0.0) d+=PI*2; return d; }

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
	sect=1;
	subsect=1;
	col=0xFFFFFFFF;
	txt="";
	inherit_position=true;
	inherit_scale=true;
	inherit_angle=true;
	compose_order=COMPOSE_ORDER_SRT; // Scale, Rotate, Translate
	parent=NULL;
	has_surface=false;
	vpsurface=NULL;
	composed_surface=-1;
	dirty=true;
}
Element* fn_create_element(void) { return new Element; }
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
std::vector<std::string> Element::get_attrs(void) {
	access.lock();
	std::vector<std::string> v;
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
	std::vector<PointSet> tpsv;
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

std::string Element::get_attr(std::string key) {
	access.lock();
	std::string s=""; int n; char c[32];
	if( attrs.find(key) != attrs.end() ) {
		access.unlock();
		return attrs[key];
	} else {
		access.unlock();
		return "unknown";
	}
}

void Element::set_attr(std::string key, std::string value) {
	access.lock();
	attrs[key]=value;
	dirty=true;
	access.unlock();
}

std::string Element::type(void) { return typestr; }
std::string Element::name(void) { return namestr; }

void Element::name(std::string n) { namestr=n; }

Element* Window::add(std::string s,Element* el) {
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

Element* Window::del(Element* el) {
	el=del_children(el);
	Element* par=this;
	if(el->parent!=NULL) par=el->parent;
	par->contents.remove(el);
	delete el;
}
Element* Window::del_children(Element* el) {
	if(el==NULL) el=this;
	// First we delete all children
	for(auto iter=el->contents.begin(); iter!=el->contents.end(); ++iter) {
		del_children(*iter);
		//cout << "Deleting " << *iter << endl;
		delete *iter;
	}
	// Now we remove them from the contents
	while(!el->contents.empty()) el->contents.pop_front();
	return el;
}


Element* Window::move(Element* subject,Element* target) {
	if( subject->parent!=NULL ) {
		if( target==NULL ) target=this;
		subject->access.lock();
		target->access.lock();
		subject->parent->contents.remove(subject);
		target->contents.push_back(subject);
		subject->parent=target;
		target->access.unlock();
		subject->access.unlock();
	}
	return subject->parent;
}

Element* Window::encap(std::string s,Element* el) {
	if(el==NULL) el=this;
	Element* e=add(s,el);
	std::list<Element*> tc = el->contents;
	for(auto iter=tc.begin(); iter!=tc.end(); ++iter) {
		if(*iter!=e) {
			move( *iter,e );	
		}
	}
	return e;
}

Element* Window::decap(Element* el) {
	if(el!=NULL) {
		if(el->parent!=NULL) {
			std::list<Element*> tc = el->contents;
			for(auto iter=tc.begin(); iter!=tc.end(); ++iter) {
				move( *iter, el->parent );
			}
		}
	}
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

Element* Element::copy(Element* wnd) {
		
		Window* w=static_cast<Window*>(wnd);
		Element* e=w->add(this->typestr,this->parent);
		
		for(int i=0;i<4;++i) e->geometry[i]=this->geometry[i];
		for(int i=0;i<2;++i) {
			e->scale[i]=this->scale[i];
			e->angles[i]=this->angles[i];
		}
		e->thick=this->thick;
		e->sect=this->sect;
		e->subsect=this->subsect;
		e->col=this->col;
		e->txt=this->txt;
		e->inherit_angle=this->inherit_angle;
		e->inherit_scale=this->inherit_scale;
		e->compose_order=this->compose_order;

		std::vector<std::string> a=get_attrs();
		for(auto it=a.begin();it!=a.end();++it) {
			e->set_attr(*it,this->get_attr(*it));
		}
		e->parent=this->parent;
		e->typestr=this->typestr;
		for(auto it=this->contents.begin();it!=this->contents.end();++it) {
			// Now copy all sub-elements...
			Element* cel = *it;
			Element* celd = cel->copy(wnd);
			w->move(celd,e);
		}
		return e;
}
Element* Element::copy_all(Element* w) {
	return this->copy(w);
}

