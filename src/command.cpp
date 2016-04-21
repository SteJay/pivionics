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

#include <iostream>
#include <unistd.h>
#include "stringsplit.h"
#include "core_elements.h"
#include "command.h"
#include "box.h"
#include "circle.h"
#include "command.h"
#include "irregular.h"
#include "sdlrenderer.h"
#include "text.h"

CommandInstance::CommandInstance() {
    current=NULL;
    out="";
    windows.clear();
    renderer=NULL;
    compositor=NULL;
    curwin=NULL;
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
    if(e==NULL) {
        e=curwin;
    }
    if(e!=NULL) {
        std::string ts="";
        output("ELEMENT: ");
        ts=e->type();
        output(ts); ops();
        output("\"");output(e->name());output("\"");ops();
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
        std::string s=e->text();
        if(s.compare("")!=0){output("\nTEXT: \""); output(s);output("\"");ops();}
        std::vector<std::string> a=e->get_attrs();
        if(a.size()>0) {
            output("\nATTRS: \n");
            for(auto iter=a.begin(); iter!=a.end(); ++iter) {
                s=*iter;
                output(s+" = \""+e->get_attr(s)+"\"");ops();
            }
        }
    }
}

void CommandInstance::command(Window* window, std::string cmd) {
    
    out.clear();
    std::vector<std::string> args = split(cmd,' ');
    std::string c=args[0];

    if(c.compare("exit")==0||c.compare("cu")==0) {
        if(current == NULL||current==window) {
            output("ERROR: Can't exit any further; current element is Window.\n");
        } else {
            current = current->parent;
        }
    } else if(c.compare("find")==0 || c.compare("search")==0) {
        if(args.size() == 2) {
            std::string s=args[1];
            current=window->find_name(s);
            if(current == window) current=NULL;
        } else if(args.size() == 3) {
            std::string s=args[1];
            std::string s2=args[2];
            int i=stoi(s2);
            current=window->find_name(s,i);
            if(current == window) current=NULL;
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
            output(l.back()); ops(); l.pop_back();
        }
    } else if(c.compare("child")==0||c.compare("children")==0) {
        std::list<Element*> pell = window->list_elements(current);
        int i=0;
        while(!pell.empty()) {
            Element* pel=pell.front();
            output(i++); output(":"); output(pel->type()); output("("); output(pel->name()); output(")"); ops();
            pell.pop_front();
        }
    } else if(c.compare("siblings")==0||c.compare("sibl")==0) {
        if( current != NULL) {
            std::list<Element*> pell = window->list_elements(window->get_parent(current));
            int i=0;
            while(!pell.empty()) {
                Element* pel=pell.front();
                output(i++); output(":"); output(pel->type()); output("("); output(pel->name()); output(")"); ops();
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
                output(i++); output(":"); output(pel->type()); output("("); output(pel->name()); output(")"); ops();
                pell.pop_front();
            }
        }
    } else if(c.compare("examine")==0||c.compare("ex")==0) {
        output(current);
    } else {
        std::string cmdstr;
        try {
            Element* returnto=current;
            if(c.compare("delete")==0 || c.compare("remove")==0 ) {
                if(current!=NULL) returnto=current->parent;
            }
            cmdstr=window->command(cmd,current);
            current=returnto;
            std::cerr << "Command: " << c << "\n";

        } catch(std::exception& e){
        }
        
    }
    
}

std::string CommandInstance::do_input(std::string s) {
    access.lock();
    std::string rs="";
    if(s.size()>0) {
        std::cerr << "Got \"" << s << "\":\n";
        command(curwin,s);
        if(out.size()>0) {
            rs=out;
        }
    }
    access.unlock();
    return rs;
}

int CommandInstance::run(void) {
    access.lock();
    curwin->construct();
    compositor->compose();
    access.unlock();
}

int CommandInstance::init(Renderer* r,Compositor* c) {
    if((renderer=r)==NULL) return ERR_COMMAND_NO_RENDERER;
    if((compositor=c)==NULL) return ERR_COMMAND_NO_COMPOSITOR;
    if(windows.size()<1) {
        // Create the default window
        Window* wnd=new Window;
        wnd->register_creator("Container",&fn_create_container);
        wnd->register_creator("Rotation",&fn_create_rotation);
        wnd->register_creator("Rescale",&fn_create_rescale);
        wnd->register_creator("Circle",&fn_create_circle);
        wnd->register_creator("Spiral",&fn_create_spiral);
        wnd->register_creator("Box",&fn_create_box);
        wnd->register_creator("Text",&fn_create_text);
        wnd->register_creator("Irregular",&fn_create_irregular);
        windows.push_back(wnd);
        curwin=wnd;
        curwin->load("piv/logo.piv");
    }
    if(windows.size()<1) return ERR_COMMAND_NO_WINDOWS;
    if(curwin==NULL) return ERR_COMMAND_NO_WINDOWS;
    compositor->link_window(curwin);
    compositor->link_renderer(renderer);
    return 0;
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
