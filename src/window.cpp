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
#include "core_elements.h"
#include "stringsplit.h"

using namespace std;

Window::Window(void) {
    id_store=0;
    namestr="Default Window";
    typestr="Window";
    inherit_position=false;
    inherit_scale=false;
    inherit_angle=false;
    this->parent=NULL;
}




string Window::command(string cmd, Element* base) {
	string rs="";
	list<string> args = get_arguments(cmd);
	/////cout << "Got " << args.size() << " arguments, looking at " << args.front() << endl;
	args = command(args,base);
	rs="";
	if(args.size()>0) {
		for(auto iter=args.begin();iter!=args.end();++iter){
			rs += *iter + " ";
		}
	}
	return rs;
}

list<string> Window::command(list<string> args,Element* base){
	string cmd;
	// First we split the command apart to extract the different parts
	if( base==NULL ) base=this;
	Element* last_added=NULL;
	/////cout << "We're starting out the command bit with " << args.size() << " arguments..." << endl;
	while( args.size()>0 ) {
		cmd=args.front();
		args.pop_front();
		// Here's the horrible if elseif elsif times inifinity bit:
	/////cout << "Command is " << cmd << " and there are " << args.size() << " arguments remaining..." << endl;
		if(cmd.compare("add")==0||cmd.compare("new")==0) {
			if( args.size()>0 ) {
				if(args.front().compare("Window")!=0){
					last_added=add(args.front(),base);
				}
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
				base->name("");
			}
		} else if(cmd.compare("text")==0||cmd.compare("string")==0) {
			if( args.size()>0 ) {
				base->text(args.front());
				args.pop_front();
			} else {
				base->text("");
				return args;
			}
		} else if(cmd.compare("remove")==0||cmd.compare("delete")==0) {
			Element* dummy=del(base);
		} else if(cmd.compare("encap")==0||cmd.compare("encapsulate")==0) {
			if( args.size()>0 ) {
				last_added=encap(args.front(),base);
			}
			args.pop_front();
		} else if(cmd.compare("decap")==0||cmd.compare("decapsulate")==0) {
				last_added=decap(base);
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
				/////cout << "Setting " << item << " to " << val << "..." << endl;
				if(      item.compare("cx")         ==0 ) base->cx(stod(val,nullptr));
				else if( item.compare("cy")         ==0 ) base->cy(stod(val,nullptr));
				else if( item.compare("width")      ==0 ) base->width(stod(val,nullptr));
				else if( item.compare("height")     ==0 ) base->height(stod(val,nullptr));
				else if( item.compare("xscale")     ==0 ) base->scale_x(stod(val,nullptr));
				else if( item.compare("yscale")     ==0 ) base->scale_y(stod(val,nullptr));
				else if( item.compare("angle")      ==0 ) base->angle(stold(val,nullptr));
				else if( item.compare("arc")        ==0 ) base->arc(stold(val,nullptr));
				else if( item.compare("thickness")  ==0 ) base->thickness(stoi(val,nullptr,0));
				else if( item.compare("sections")   ==0 ) base->sections(stoi(val,nullptr,0));
				else if( item.compare("subsections")==0 ) base->subsections(stoi(val,nullptr,0));
				else if( item.compare("color")      ==0 ) base->color(stoul(val,nullptr,0));
				else if( item.compare("inherit")   ==0 ) {
					if(args.size()>0) {
						item=val;
						val=args.front();
						args.pop_front();
						if(item.compare("angle") ==0 || item.compare("r") ==0 ) {
							if(val.compare("true")==0) base->inherit_angle=true; else base->inherit_angle=false;
						} else if(item.compare("position") ==0 || item.compare("p") ==0 ) {
							if(val.compare("true")==0) base->inherit_position=true; else base->inherit_position=false;
						} else if(item.compare("scale") ==0 || item.compare("s") ==0 ) {
							if(val.compare("true")==0) base->inherit_scale=true; else base->inherit_scale=false;
						}
					}
				}
			}
		} else if(cmd.compare("attr")==0||cmd.compare("attribute")==0) {
			if(args.size()>1) {
				string s,t;
				s=args.front();
				args.pop_front();
				t=args.front();
				args.pop_front();
				base->attrs[s]=t;
				//cout << "Attribute set " << s << " to " << t << endl;
			}
		} else if(cmd.compare("{")==0||cmd.compare("{")==0) {
			// Go into the last added element and perform following commands on that
			if( last_added!=NULL) {
				args = command(args,last_added);
				/////cout << "We returned successfully!" << endl;
				last_added=NULL;
			}
		} else if(cmd.compare("}")==0||cmd.compare("}")==0) {
			// Go back out of our element.
			args.push_front(" ");
			break;
			
		//} else if(cmd.compare("")==0||cmd.compare("")==0) {
		} else if(cmd.compare("load")==0||cmd.compare("read")==0) {
			if(args.size()>0) {
				load(args.front() );
				args.pop_front();
			}
		} else if(cmd.compare("save")==0||cmd.compare("write")==0) {
			if(args.size()>0) {
				save(args.front(),base);
				args.pop_front();
			}
		} else {
			//args.push_front("ERROR: Unrecognised Command");
			//return args;
		}
	}
	/////cout << "Reached the end - lets try and return!" << endl;
	return args;
}
/*
Window::~Window() {
	access.lock();
	Element* e;
	while( !contents.empty()){
		e=contents.back();
		contents.pop_back();
		delete e;
	}
	access.unlock();
}*/
int Window::children(Element* el) {
	access.lock();
	if( el == NULL ) {
		access.unlock();
		return static_cast<int>(contents.size());
	} else {
		access.unlock();
		return static_cast<int>(el->contents.size());
	}
}
list<string> Window::list_creators(void) {
	access.lock();
	list<string> l;
	for(auto iter=creators.begin();iter!=creators.end();++iter) {
		l.push_back(iter->first);
	}
	access.unlock();
	return l;
}

list<Element*> Window::list_elements(Element* pel) {
	access.lock();
	list<Element*> c;
	if( pel != NULL) {
		c=pel->contents;
	} else {
		c=this->contents;
	}
	access.unlock();
	return c;
}

Element* Window::get_parent(Element* el) {
	access.lock();
	if( el != NULL ) {
		access.unlock();
		return el->parent;
	}
	access.unlock();
	return NULL;
}

Element* Window::sibling(unsigned int i, Element* el) {
	access.lock();
	if( el != NULL) {
		Element* p=el->parent;
		if( p!=NULL && p->contents.size()>i) {
			auto iter=p->contents.begin();
			advance(iter,i);
			access.unlock();
			return *iter;
		} else if( this->contents.size()>i){
			auto iter=this->contents.begin();
			advance(iter,i);
			access.unlock();
			return *iter;
		}
	}
	access.unlock();
	return NULL;
}

Element* Window::child(unsigned int i, Element* el) {
	access.lock();	
	if( el != NULL && el->contents.size()>i) {
		auto iter=el->contents.begin();
		advance(iter,i);
		access.unlock();
		return *iter;
	} else if(this->contents.size()>i){
		auto iter=this->contents.begin();
		advance(iter,i);
		access.unlock();
		return *iter;
	}
	access.unlock();
	return NULL;
}

Element* Window::find_name(string n) { return find_name(n,0); }
Element* Window::find_name(string n,int ignore) { return find_name(n,ignore,this); }

Element* Window::find_name(string n,int ignore, Element* base) {
	string t;
	Element* e;
	for(auto iter=base->contents.begin(); iter!=base->contents.end(); ++iter) {
		e= *iter;
		t=e->name();
		/////cout << "Comparing " << n << " with " << t << "..." << endl;
		if(n.compare(t)==0) {
		  if(ignore>0) {
		    ignore--; 
		  } else {
		    return e;
		  }
		}
		e = find_name(n,ignore,e);
		if( e!=NULL) {
			t=e->name();
			if(n.compare(t)==0) {
				if(ignore>0) {
					ignore--; 
				} else {
			    	return e;
				}
			}
		}
		
	}
	e=NULL;
	return e;
}







void Window::load(string fn,Element *el) {
	ifstream file(fn,ios::in|ios::binary|ios::ate);
	if(file.is_open()) {
		streampos size=file.tellg(); // because we used ios::ate we start at the end of the file, thus we know the length
		if(size>0) {
			char* buf=new char[size];
			file.seekg(0,ios::beg); // seek back to the beginning of the file
			file.read(buf,size);
			command(buf,el);
			delete buf;
		}
		file.close();
	}
}
void Window::load(string fn) { load(fn,this); }

void Window::save(string fn,Element* el) {
	ofstream file(fn,ios::out);
	save(&file,el);	
	file.close();	
}

void Window::save(ofstream* file,Element* el) {
	save(file,el,"");
}
void Window::save(ofstream* file,Element* el,string lvl) {
	string t = el->type();
	bool isntwindow=true;
	if(t.compare("Window") != 0) {
		*file << lvl << "add " + t;
		*file << " " << "{" << endl;
		lvl+="  ";
		t=el->name();
		if(t.compare("") != 0) { *file << lvl << "name \"" + replace_all(t,"\"","\\\"") + "\"" << endl; }
		t=el->text();
		if(t.compare("") != 0) { *file << lvl << "text \"" + replace_all(t,"\"","\\\"") + "\"" << endl; }
		*file << lvl << "set cx " << el->cx() << endl;
		*file << lvl << "set cy " << el->cy() << endl;
		*file << lvl << "set width " << el->width() << endl;
		*file << lvl << "set height " << el->height() << endl;
		*file << lvl << "set xscale " << el->scale_x() << endl;
		*file << lvl << "set yscale " << el->scale_y() << endl;
		*file << lvl << "set angle " << setprecision(16) << el->angle() << endl;
		*file << lvl << "set arc " << setprecision(16) << el->arc() << endl;
		*file << lvl << "set thickness " << el->thickness() << endl;
		*file << lvl << "set sections " << el->sections() << endl;
		*file << lvl << "set subsections " << el->subsections() << endl;
		*file << lvl << "set color " << el->color() << endl;
		
		*file << lvl << "set inherit position "; if(el->inherit_position) *file << "true"; else *file << "false"; *file << endl;
		*file << lvl << "set inherit angle "; if(el->inherit_angle) *file << "true"; else *file << "false"; *file << endl;
		*file << lvl << "set inherit scale "; if(el->inherit_scale) *file << "true"; else *file << "false"; *file << endl;

		for(auto iter=el->attrs.begin(); iter != el->attrs.end();++iter) {
			*file << lvl << "attr " << "" << iter->first << " \"" << replace_all(iter->second,"\"","\\\"") << "\"" << endl;
		}
	} else {
	  isntwindow=false;
	}
	for(auto iter=el->contents.begin();iter!=el->contents.end();++iter) {
		save(file,*iter,lvl);
	}
	lvl.erase(0,2);
	if(isntwindow) *file << lvl << "}" << endl;
}

void Window::save(string fn) { save(fn,this); }
