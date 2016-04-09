/*
simple_edit.cpp - Simple commandline Window editor.
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
#include <sys/stat.h>
#include <unistd.h>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include "core_elements.h"
#include "sdlrenderer.h"
#include "sdlcompositor.h"
#include "circle.h"
using namespace std;
inline bool file_exists(const string& name) { struct stat buffer; return (stat (name.c_str(), &buffer) == 0); }

// Here are our creator functions; we need one for each element we need to use. Klunky. I'm working on it.
//Element* create_container(void) { return new Container; }
//Element* create_rotation(void) { return new Rotation; }
//Element* create_offset(void) { return new Offset; }
//Element* create_offset_rotation(void) { return new OffsetRotation; }
//Element* create_static_container(void) { return new StaticContainer; }

SdlCompositor compositor;
SdlRenderer renderer;

Element* current=NULL;
string get_tree_text( Element* el, string sofar="" ) {
	if( el != NULL ) {
		string n = el->name();
		if(n.compare("\0")!=0) {
			sofar += "("+el->name() + ")";
		} else {
			sofar += el->type();
			
		}
		if( el->parent != NULL) {
			sofar=get_tree_text( el->parent ) + sofar;
		}
	}
	sofar += '/';
	return sofar;
}

void command(Window* window,string cmd) {
	vector<string> args = split(cmd,L' ');
	string c=args[0];

	if(c.compare("exit")==0) {
		if(current == NULL) {
			cout << "ERROR: Already editing Window, the top level." << endl << endl;
		} else {
			current = current->parent;
			cout << "Exited to parent element." << endl << endl;
		}
	} else if(c.compare("fps")==0 || c.compare("perf")==0) {
		unsigned int fpstot=0;
		unsigned int max=20;
		struct timespec t,t2;
		for(int  i=0;i<20;++i) {
			cout << ".";
			cout.flush();
			fpstot+=renderer.get_fps();
			t.tv_sec=0;
			t.tv_nsec=500000000;
			nanosleep(&t,&t2);
		} 
		cout << endl;
		cout << "Approximate Frame Rate: " << to_string(fpstot/20) << " fps." << endl <<endl;
	} else if(c.compare("find")==0 || c.compare("search")==0) {
		if(args.size() == 2) {
			string s=args[1];
			current=window->find_name(s);
			if(current == window) current=NULL;
			cout << "Current element changed by find/search" << endl;
		} else if(args.size() == 3) {
			string s=args[1];
			string s2=args[2];
			int i=stoi(s2);
			current=window->find_name(s,i);
			if(current == window) current=NULL;
			cout << "Current element changed by find/search" << endl;
		} else {
			cout << "ERROR: Incorrect number of arguments for find/search" << endl;
		}
		cout << endl;
	} else if(c.compare("enter")==0 || c.compare("cd")==0) {
		if(args.size() ==3) {
			if( args[1].find("/") != string::npos) {
				cout << "ERROR: Path access not supported yet. Watch this space." << endl;
			} else {
				Element *el = NULL;
				unsigned int o = 0;
				o=stoi(args[2],nullptr,0); // Attempt to guess the base from context ie prefix 0x for hexadecimal
				if( args[1].compare("sibling") ==0 || args[1].compare("sibl")==0 || args[1].compare("s")==0) {
					el = window->sibling(o,current);
				} else if( args[1].compare("child") ==0 || args[1].compare("c")==0) {
					el = window->child(o,current);
				}
				current=el;
				cout << "Current element changed to " << args[1] << ":" << args[2] << endl;
			}
		} else {
			cout << "ERROR: You need to specify what element you want to enter - see \"help enter\"." << endl << endl;
		}
		cout << endl;
	} else if(c.compare("types")==0) {
		list<string> l=window->list_creators();
		cout << "Available Element Classes (types):" << endl << endl;
		while(!l.empty()){
			cout << l.back() << " ";
			l.pop_back();
		}
		cout << endl << endl;
	} else if(c.compare("child")==0||c.compare("children")==0) {
		list<Element*> pell = window->list_elements(current);
		cout << "Children of current element in \"tuid:Type(Name)\" format:" << endl;
		int i=0;
		while(!pell.empty()) {
			Element* pel=pell.front();
			cout << i++ << ":" << pel->type() << "(" << pel->name() << ")" << endl;
			pell.pop_front();
		}
		cout << endl;
	} else if(c.compare("siblings")==0||c.compare("sibl")==0) {
		if( current != NULL) {
			list<Element*> pell = window->list_elements(window->get_parent(current));
			int i=0;
			cout << "Siblings of current element in \"tuid:Type(Name)\" format:" << endl;
			while(!pell.empty()) {
				Element* pel=pell.front();
				cout << i++ << ":" << pel->type() << "(" << pel->name() << ")" << endl;
				pell.pop_front();
			}
		} else {
			cout << "ERROR: You are at the top level (the Window) so there can't be any siblings." << endl;
		}
		cout << endl;
	} else if(c.compare("parents")==0) {
		Element* pa=window->get_parent(current);
		Element* pu=window->get_parent(pa);
		if( current != NULL) {
			list<Element*> pell = window->list_elements(pu);
			int i=0;
			cout << "Siblings of current element's parent in \"tuid:Type(Name)\" format:" << endl;
			while(!pell.empty()) {
				Element* pel=pell.front();
				cout << i++ << ":" << pel->type() << "(" << pel->name() << ")" << endl;
				pell.pop_front();
			}
		} else {
			cout << "ERROR: You are at the top level (the Window) so there can't be any siblings." << endl;
		}
		cout << endl;
	} else if(c.compare("list")==0||c.compare("ls")==0) {
		if(current!=NULL) {
			cout << "Current Element is " << current->type() << " named (" << current->name() << ")" << endl;
			cout << "cx: " << current->cx() << ", cy: " << current->cy() << ", width: " << current->width() << ", height: " << current->height() << endl;
			cout << "angle: " << current->angle() << ", arc: " << current->arc() << ", thickness: " << current->thickness() << endl;
			cout << "sections: " << current->sections() << ", subsections: " << current->subsections() << ", color: " << hex << current->color() << endl;
			vector<string> a=current->get_attrs();
			string s;
			for(auto iter=a.begin(); iter!=a.end(); ++iter) {
				s=*iter;
				cout << "Attribute: " << s << " = " << current->get_attr(s) << "" << endl;
			}
		} else {
			cout << "Current Element is a window named (" << window->name() << ")" << endl;
			cout << "DO NOT MESS WITH ANY PROPERTIES OF THE WINDOW, THEY ARE SET AUTOMATICALLY" << endl;
			cout << "Playing about with the Window is a bad idea full stop." << endl;
		}		
		cout << endl;
	} else if(c.compare("help")==0) {
		if(args.size() < 2 ) {
			cout << "Commands available:" << endl;
			cout << "Listing:       types list ls children child siblings sibl parents" << endl;
			cout << "Navigation:    enter exit find" << endl;
			cout << "Creation:      add new name rename set = " << endl;
			cout << "Files:			save load" << endl;
			cout << "Type \"help <command>\" for more information (where <command> is one of the above)." << endl;
		} else {
			c=args[1];
			if(c.compare("list")==0||c.compare("ls")==0) {
				cout << "Usage: list" << endl;
				cout << "Usage: ls" << endl;
				cout << "This lists the setup of the current element, including its geometry, angles and attributes." << endl;
				cout << "THIS IS AN EDITOR COMMAND AND CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("types")==0) {
				cout << "Usage: types" << endl;
				cout << "This lists all known (handled) element types." << endl;
				cout << "THIS IS AN EDITOR COMMAND AND CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("children")==0||c.compare("child")==0) {
				cout << "Usage: children" << endl;
				cout << "Usage: child" << endl;
				cout << "This lists all children of the current element with their names and uids." << endl;
				cout << "THIS IS AN EDITOR COMMAND AND CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("siblings")==0||c.compare("sibl")==0) {
				cout << "Usage: siblings" << endl;
				cout << "Usage: sibl" << endl;
				cout << "This lists all siblings of the current element with their names and uids." << endl;
				cout << "THIS IS AN EDITOR COMMAND AND CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("parents")==0) {
				cout << "Usage: parents" << endl;
				cout << "This lists all siblings of the current element's parent with their names and uids." << endl;
				cout << "THIS IS AN EDITOR COMMAND AND CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("enter")==0 || c.compare("cd")==0) {
				cout << "Usage: enter <child|sibling> <index>" << endl;
				cout << "Usage: cd <child|sibling> <index>" << endl;
				cout << "This is analogous to \"cd\" in CLIs like a shell. Use this in tandem with the" << endl;
				cout << "\"children\" and \"siblings\"; find the element you want to move to, and provide" << endl;
				cout << "the keyword \"child\" or \"sibling\" followed by the index number listed by children" << endl;
				cout << "or siblings; the current element will then be changed to the element you specified." <<endl;
				cout << "This is for use where you have unnamed elements you need to switch to and is currently very basic" << endl;
				cout << "A better version allowing you to specify a path is planned but not currently available." << endl;
				cout << "THIS IS AN EDITOR COMMAND AND CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("exit")==0) {
				cout << "Usage: exit" << endl;
				cout << "This moves \"out of\" the current element, making its parent the current element." << endl;
				cout << "THIS IS AN EDITOR COMMAND AND CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("find")==0) {
				cout << "Usage: find <name>" << endl;
				cout << "This changes to current element to the first element with a name matching the one given." << endl;
				cout << "If no matching element can be found, the current element will change to the Window." << endl;
				cout << "THIS IS AN EDITOR COMMAND AND CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("save")==0 || c.compare("write")==0) {
				cout << "Usage: save <filename>" << endl;
				cout << "Usage: write <filename>" << endl;
				cout << "Saves the current element (including all children) to a new file." << endl;
				cout << "THIS WILL OVERWRITE THE FILE YOU SPECIFY WITH NO CONFIRMATION!" << endl;
				cout << "You must specify a filename." << endl;
			} else if(c.compare("load")==0||c.compare("read")==0) {
				cout << "Usage: load <filename>" << endl;
				cout << "Loads the given file as a child of the current element." << endl;
				cout << "You must specify a filename." << endl;
			} else if(c.compare("set")==0||c.compare("=")==0) {
				cout << "Usage: set <perameter> <value>" << endl;
				cout << "Usage: <perameter> = <value>" << endl;
				cout << "Sets one of the element's parameters. Be careful to use a space either side of the = when using the second form." << endl;
				cout << "The perameter can be any of: cx,cy,width,height,angle,arc,thickness,sections,subsections,color" << endl;
			} else if(c.compare("name")==0||c.compare("rename")==0) {
				cout << "Usage: name <new name>" << endl;
				cout << "Usage: rename <new name>" << endl;
				cout << "Changes the human-readable name of the current element to the one specified." << endl;
				cout << "Enclose strings with spaces in \"quotes\". You can escape quotes within the string using \\\"." << endl;
			} else if(c.compare("add")==0||c.compare("new")==0) {
				cout << "Usage: add <element type>" << endl;
				cout << "Usage: new <element type>" << endl;
				cout << "Adds a new element of the given type as a child of the current element." << endl;
				cout << "Use the \"types\" command to see what elements are available for use." << endl;
			} else if(c.compare("{")==0||c.compare("}")==0) {
				cout << "Curly brackets (braces to our USA friends) are used to allow one command set (line or file)" << endl;
				cout << "to encapsulate more than one element. This is used mainly for loading files, but" << endl;
				cout << "can be used in this editor as long as they are contained within a single line." << endl;
				cout << endl << "For example the following line will create an element named \"Element1\" which contains another element named \"Element2\":" << endl << "add Element { name Element1 add Element { name Element2 } }" << endl;
			} else {
				cout << c << " is not a recognised command, so I can't help you with it." << endl;
			}
		}
		cout << endl;
	} else {
		string cmdstr;
		try {
			cmdstr=window->command(cmd,current);
		} catch(exception& e){
			cout << "ERROR: Caught exception { " << e.what() << "}, your command may not have worked as you intended!" << endl;
			cout.flush();
		}
		//cout << "Command sent to window: " << cmd << endl;
		//cout << "Response (usually blank): " << cmdstr << endl << endl;
	//	cout << "ERROR: Unrecognised Command " << c << endl << endl;
	}
}

int main (int argc, char* argv[]) {
	cout << "GPL and such" << endl;
	cout << "Pivionics window editor - type \"help\" for help" << endl;
	char chrbuf[128];
	Window window;
	string in_str;
	//window.register_creator("Element",&create_element);
	//window.register_creator("Element",&element_creator<Element>); // Some sort of class casting conflict doing this for other types... We'll come back to this later.
	window.register_creator("Container",&fn_create_container);
	window.register_creator("Rotation",&fn_create_container);
	window.register_creator("Circle",&fn_create_circle);
	//window.register_creator("Container",&create_container);
	//window.register_creator("Rotation",&create_rotation);
	//window.register_creator("Offset",&create_offset);
	//window.register_creator("OffsetRotation",&create_offset_rotation);
	//window.register_creator("StaticContainer",&create_static_container);

	if( argc>1 && file_exists(argv[1])) {
		string fn=argv[1];
		window.command("load " + fn,NULL);
	}
	renderer.render_run();
	compositor.link_window(&window);
	compositor.link_renderer(&renderer);
	while( in_str.compare("quit") != 0) {
		window.construct();
		compositor.compose();
		//cout << current->type() << "(" << current->name() << "), " << window.children(current) << " Children. " << 
		cout << get_tree_text(current) << "> ";
		cin.getline(chrbuf,sizeof(chrbuf));
		in_str=chrbuf;
		if(in_str.size()>0) {
			command(&window,in_str);
		}
	}
	renderer.render_stop();
	renderer.shutdown();
	return 0;
}
