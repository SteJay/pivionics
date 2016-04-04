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
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include "core_elements.h"
#include "stringsplit.h"

using namespace std;

// Here are our creator functions; we need one for each element we need to use. Klunky. I'm working on it.

Element* create_element(void) { return new Element; } 
Element* create_container(void) { return new Container; } 
Element* create_rotation(void) { return new Rotation; } 
Element* create_offset(void) { return new Offset; } 
Element* create_offset_rotation(void) { return new OffsetRotation; } 
Element* create_static_container(void) { return new StaticContainer; } 


Element* current=NULL;
string get_tree_text( Element* el, string sofar="" ) {
	sofar += '/';
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
	} else if(c.compare("enter")==0 || c.compare("cd")==0) {
		if(args.size() ==3) {
			if( args[1].find("/") != string::npos) {
				cout << "ERROR: Path access not supported yet. Watch this space." << endl;
			} else {
				Element *el;
				unsigned int o = stoi(args[2],nullptr,0); // Attempt to guess the base from context ie prefix 0x for hexadecimal
				if( args[1].compare("sibling") ==0 || args[1].compare("sibl")==0) {
					el = window->sibling(o,current);
				} else if( args[1].compare("child") ==0 || args[1].compare("chld")==0) {
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
			list<Element*> pell = window->list_elements(window->parent(current));
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
		Element* pa=window->parent(current);
		Element* pu=window->parent(pa);
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
			cout << "Creation:      add name remove rem attribute attr geometry g angles a thickness t sections s color c" << endl;
			cout << "Files:			save load export import" << endl;
			cout << "Type \"help <command>\" for more information (where <command> is one of the above)." << endl;
		} else {
			c=args[1];
			if(c.compare("list")==0||c.compare("ls")==0) {
				cout << "Usage: list" << endl;
				cout << "Usage: ls" << endl;
				cout << "This lists the setup of the current element, including its geometry, angles and attributes." << endl;
			} else if(c.compare("types")==0) {
				cout << "Usage: types" << endl;
				cout << "This lists all known (handled) element types." << endl;
			} else if(c.compare("children")==0||c.compare("child")==0) {
				cout << "Usage: children" << endl;
				cout << "Usage: child" << endl;
				cout << "This lists all children of the current element with their names and uids." << endl;
			} else if(c.compare("siblings")==0||c.compare("sibl")==0) {
				cout << "Usage: siblings" << endl;
				cout << "Usage: sibl" << endl;
				cout << "This lists all siblings of the current element with their names and uids." << endl;
			} else if(c.compare("parents")==0) {
				cout << "Usage: parents" << endl;
				cout << "This lists all siblings of the current element's parent with their names and uids." << endl;
			} else if(c.compare("enter")==0) {
				cout << "Usage: enter" << endl;
				cout << "This moves \"into\" another element, making its first child the current element." << endl;
				cout << "This is analogous to \"cd\" in CLIs like a shell. Use this in tandem with the" << endl;
				cout << "\"children\" and \"siblings\"; find the element you want to move to, and provide" << endl;
				cout << "the keyword \"child\" or \"sibling\" followed by the index number listed by children" << endl;
				cout << "or siblings." <<endl;
				cout << "**You will also be able to specify a direct pathname but this isn't supported yet!" << endl;
			} else if(c.compare("exit")==0) {
				cout << "Usage: exit" << endl;
				cout << "This moves \"out of\" the current element, making its parent the current element." << endl;
			} else if(c.compare("find")==0) {
				cout << "Usage: find <uid|name>" << endl;
				cout << "This finds and moves to the element with the given uid or name. UIDs are always \nintegers whereas names must always contain alphabet characters" << endl;
			} else if(c.compare("save")==0) {
				cout << "Usage: save <filename>" << endl;
				cout << "This will save the entire Window being edited as a .pivionics.window file." << endl;
				cout << "You must specify a filename." << endl;
			} else if(c.compare("load")==0) {
				cout << "Usage: load <filename>" << endl;
				cout << "This will a .pivionics.window file into the editor. THIS WILL WIPE OUT ANY EXISTING WINDOW IN THE EDITOR!" << endl;
				cout << "You must specify a filename." << endl;
			} else if(c.compare("save")==0) {
				cout << "Usage: save <filename>" << endl;
				cout << "This will save the entire Window being edited as a .pivionics.window file." << endl;
			} else if(c.compare("export")==0) {
				cout << "Usage: export <filename>" << endl;
				cout << "This will save the current element and its children being edited as a .pivionics.element file." << endl;
				cout << "You must specify a filename." << endl;
			} else if(c.compare("import")==0) {
				cout << "Usage: import <filename>" << endl;
				cout << "This will load the specified .pivionics.element file as a child of the current element." << endl;
				cout << "You must specify a filename." << endl;
			} else if(c.compare("export")==0) {
				cout << "Usage: quit" << endl;
				cout << "This will quit the editor. If the current window is dirty (it hasn't been saved since changed)" << endl;
				cout << "you will be asked to type \"Yes\" (note the capital Y) to confirm." << endl;
			} else {
				//cout << c << " is not a recognised command, so I can't help you with it." << endl;
			}
		}
		cout << endl;
	} else {
		string cmdstr;
		try {
			cmdstr=window->command(cmd,current);
		} catch (int e) {
			cout << "ERROR: Caught exception " << e << ", your command may not have worked as you intended!" << endl;
		}
		cout << "Command sent to window: " << cmd << endl;
		cout << "Response (usually blank): " << cmdstr << endl << endl;
	//	cout << "ERROR: Unrecognised Command " << c << endl << endl;
	}
}

int main (int argc, char* argv[]) {
	cout  << "PI is " << setprecision(16) << PI << " and 2*PI is " << PI*2.0 << endl;
	char chrbuf[128];
	Window window;
	string in_str;
	//window.register_creator("Element",&create_element);
	window.register_creator("Element",&element_creator<Element>); // Some sort of class casting conflict doing this for other types... We'll come back to this later.
	window.register_creator("Container",&create_container);
	window.register_creator("Rotation",&create_rotation);
	window.register_creator("Offset",&create_offset);
	window.register_creator("OffsetRotation",&create_offset_rotation);
	window.register_creator("StaticContainer",&create_static_container);
	while( in_str.compare("quit") != 0) {
		cout << window.children(current) << " Children";
		cout << ":[" << get_tree_text(current) << "]> ";
		cin.getline(chrbuf,sizeof(chrbuf));
		in_str=chrbuf;
		if(in_str.size()>0) {
			command(&window,in_str);
		}
	}

	return 0;
}
