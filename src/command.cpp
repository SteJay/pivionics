/*
command.cpp - Implementation for the CommandInstance class
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

#include "stringsplit.h"
#include "command.h"

CommandInstance::CommandInstance() {
    current=NULL;
    out="";
}

void CommandInstance::ops(void) {
    out+=" ";
    
}

void CommandInstance::output(std::string s) {
    out+=s;
}

void CommandInstance::output(double d) {
    out+=std::to_string(d);
}
void CommandInstance::output(long double d) {
    out+=std::to_string(d);
}
void CommandInstance::output(int i) {
    out+=std::to_string(i);
}
void CommandInstance::output(unsigned int i) {
    out+=std::to_string(i);
}
void CommandInstance::output(Element* e) {
        output(e->type()); ops();
        output(e->name());ops();
        output(e->cx());ops();
        output(e->cy()); ops();
        output(e->width()); ops();
        output(e->height());ops();
        output(e->scale_x());ops();
        output(e->scale_y());ops();
        output(e->angle());ops();
        output(e->arc());ops();
        output(e->thickness());ops();
        output(e->sections());ops();
        output(e->subsections());ops();
        output(e->color());ops();
        if(e->inherit_position) output("true"); else output("false");ops();
        if(e->inherit_angle) output("true"); else output("false");ops();
        if(e->inherit_scale) output("true"); else output("false");ops();
        std::vector<std::string> a=e->get_attrs();
        std::string s;
        s=e->text();
        if(s.compare("")!=0){output(s);ops();}
        if(a.size()>0) {
            output("Attrs: \n");
            for(auto iter=a.begin(); iter!=a.end(); ++iter) {
                s=*iter;
                output(s+" = \""+e->get_attr(s)+"\"");
            }
        }
}

void CommandInstance::command(Window* window, std::string cmd) {
    out.clear();
    std::vector<std::string> args = split(cmd,' ');
    std::string c=args[0];

    if(c.compare("exit")==0||c.compare("cu")==0) {
        if(current == NULL||current==window) {
            output("Can't exit any further; current element is Window.\n");
        } else {
            current = current->parent;
            output("Exited to parent element.\n");
        }
    } else if(c.compare("find")==0 || c.compare("search")==0) {
        if(args.size() == 2) {
            std::string s=args[1];
            current=window->find_name(s);
            if(current == window) current=NULL;
            output("Current element changed by find/search\n");
        } else if(args.size() == 3) {
            std::string s=args[1];
            std::string s2=args[2];
            int i=stoi(s2);
            current=window->find_name(s,i);
            if(current == window) current=NULL;
            output("Current element changed by find/search\n");
        } else {
            output("ERROR: Incorrect number of arguments for find/search\n");
        }
    } else if(c.compare("enter")==0 || c.compare("cd")==0) {
        if(args.size() ==3) {
            Element *el = NULL;
            unsigned int o = 0;
            o=std::stoi(args[2],nullptr,0); // Attempt to guess the base from context ie prefix 0x for hexadecimal
            if( args[1].compare("sibling") ==0 || args[1].compare("sibl")==0 || args[1].compare("s")==0) {
                el = window->sibling(o,current);
            } else if( args[1].compare("child") ==0 || args[1].compare("c")==0) {
                el = window->child(o,current);
            }
            current=el;
        }
    } else if(c.compare("types")==0) {
        std::list<std::string> l=window->list_creators();
        while(!l.empty()){
            output(l.back()); output("\n"); l.pop_back();
        }
    } else if(c.compare("child")==0||c.compare("children")==0) {
        std::list<Element*> pell = window->list_elements(current);
        int i=0;
        while(!pell.empty()) {
            Element* pel=pell.front();
            output(i++); output(":"); output(pel->type()); output("("); output(pel->name()); output(")\n");
            pell.pop_front();
        }
    } else if(c.compare("siblings")==0||c.compare("sibl")==0) {
        if( current != NULL) {
            std::list<Element*> pell = window->list_elements(window->get_parent(current));
            int i=0;
            while(!pell.empty()) {
                Element* pel=pell.front();
                output(i++); output(":"); output(pel->type()); output("("); output(pel->name()); output(")\n");
                pell.pop_front();
            }
        }
    } else if(c.compare("parents")==0) {
        Element* pa=window->get_parent(current);
        Element* pu=window->get_parent(pa);
        if( current != NULL) {
            std::list<Element*> pell = window->list_elements(pu);
            int i=0;
            while(!pell.empty()) {
                Element* pel=pell.front();
                output(i++); output(":"); output(pel->type()); output("("); output(pel->name()); output(")\n");
                pell.pop_front();
            }
        }
    } else if(c.compare("examine")==0||c.compare("ex")==0) {
    }

}






std::string treepos( Element* el, std::string sofar="" ) {
	if( el != NULL ) {
		std::string n = el->name();
		if(n.compare("\0")!=0) {
			sofar += "("+el->name() + ")";
		} else {
			sofar += el->type();
			
		}
		if( el->parent != NULL) {
			sofar=treepos( el->parent ) + sofar;
		}
	}
	sofar += '/';
	return sofar;
}
