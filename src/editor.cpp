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
#include "box.h"
#include "text.h"
#include "license.h"
using namespace std;
inline bool file_exists(const string& name) { struct stat buffer; return (stat (name.c_str(), &buffer) == 0); }

// Here are our creator functions; we need one for each element we need to use. Klunky. I'm working on it.
//Element* create_container(void) { return new Container; }
//Element* create_rotation(void) { return new Rotation; }
//Element* create_offset(void) { return new Offset; }
//Element* create_offset_rotation(void) { return new OffsetRotation; }
//Element* create_static_container(void) { return new StaticContainer; }

Compositor compositor;
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

	if(c.compare("exit")==0||c.compare("cu")==0) {
		if(current == NULL) {
			cout << "ERROR: Already editing Window, the top level." << endl << endl;
		} else {
			current = current->parent;
			cout << "Exited to parent element." << endl << endl;
		}
		if(current==window) { current=NULL; }
	} else if(c.compare("fps")==0 || c.compare("perf")==0) {
		unsigned int fpstot=0;
		unsigned int max=10;
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
	} else if(c.compare("examine")==0||c.compare("ex")==0) {
		if(current==NULL) current=window;
		cout << "Current Element is " << current->type() << " named (" << current->name() << ")" << endl;
		cout << "cx: " << current->cx() << ", cy: " << current->cy() << ", width: " << current->width() << ", height: " << current->height() << ", xscale: " << current->scale_x() << ", yscale: " << current->scale_y() << endl;
		cout << "angle: " << current->angle() << ", arc: " << current->arc() << ", thickness: " << current->thickness();
		cout << "sections: " << current->sections() << ", subsections: " << current->subsections() << ", color: " << hex << current->color() << dec << endl;
		cout << "Inherit Position: "; if(current->inherit_position) cout << "true"; else cout << "false";
		cout << ", Inherit Angle: "; if(current->inherit_angle) cout << "true"; else cout << "false";
		cout << ", Inherit Scale: "; if(current->inherit_scale) cout << "true"; else cout << "false";
		vector<string> a=current->get_attrs();
		string s;
		s=current->text();
		if(s.compare("")!=0) cout << "Text: [" << s << "]" << endl;
		if(a.size()>0) {
			cout << "Attributes:" << endl;
			bool comma=false;
			for(auto iter=a.begin(); iter!=a.end(); ++iter) {
				s=*iter;
				if(comma) cout << ", "; else comma=true;
				cout << "Attribute: " << s << " = \"" << current->get_attr(s) << "\"";
			}
			cout << endl;
		}
		if(current==window) current=NULL;
		cout << endl;
	} else if(c.compare("help")==0) {
		if(args.size() < 2 ) {
			cout << "Commands available:" << endl;
			cout << "Listing:       child children examine ex parents siblings sibl types" << endl;
			cout << "Navigation:    cd cu enter exit find" << endl;
			cout << "Elemental:     add delete new remove" << endl;
			cout << "Manipulation:  decap decapsulate encap encapsulate name rename set" << endl;
			cout << "Miscellaneous:	load save quit" << endl;
			cout << "Type \"help <command>\" for more information (where <command> is one of the" << endl << "keywords above)." << endl;
		} else {
			c=args[1];
			if(c.compare("examine")==0||c.compare("ex")==0) {
				cout << "Usage: examine" << endl;
				cout << "Usage: ex" << endl;
				cout << "This lists the setup of the current element, including its geometry, angles and" << endl << " attributes." << endl;
				cout << "EDITOR COMMAND; CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("types")==0) {
				cout << "Usage: types" << endl;
				cout << "This lists all known (handled) element types." << endl;
				cout << "EDITOR COMMAND; CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("children")==0||c.compare("child")==0) {
				cout << "Usage: children" << endl;
				cout << "Usage: child" << endl;
				cout << "This lists all children of the current element with their names and" << endl << "current id for use with the enter command." << endl;
				cout << "EDITOR COMMAND; CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("siblings")==0||c.compare("sibl")==0) {
				cout << "Usage: siblings" << endl;
				cout << "Usage: sibl" << endl;
				cout << "This lists all siblings of the current element with their names and" << endl << "current id for use with the enter command."  << endl;
				cout << "EDITOR COMMAND; CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("parents")==0) {
				cout << "Usage: parents" << endl;
				cout << "This lists all siblings of the current element's parent with their names and" << endl << "current id for use with the enter command."  << endl;
				cout << "EDITOR COMMAND; CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("enter")==0 || c.compare("cd")==0) {
				cout << "Usage: enter <child|sibling> <index>" << endl;
				cout << "Usage: cd <child|sibling> <index>" << endl;
				cout << "This changes the current element to the child or sibling you specify" << endl << "(depending on whether you specify the \"child\" or \"sibling\" keyword)." << endl;
				cout << "Use the \"children\" and \"siblings\" commands to find out what index to specify." << endl << "Also see the \"find\" command." << endl;
				cout << "EDITOR COMMAND; CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("exit")==0||c.compare("cu")==0) {
				cout << "Usage: exit" << endl;
				cout << "Usage: cu" << endl;
				cout << "This moves \"out of\" the current element, making its parent the current" << endl << "element. If you are looking to exit the program, use \"quit\"." << endl;
				cout << "EDITOR COMMAND; CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("find")==0) {
				cout << "Usage: find <name>" << endl;
				cout << "This changes to current element to the first element with a name matching the" << endl << "name you specify. Pivionics does not ensure names are unique." << endl;
				cout << "If no matching element can be found, the current element will change to the" << endl << "Window (top level)." << endl;
				cout << "EDITOR COMMAND; CANNOT BE COMBINED WITH OTHER COMMANDS ON A SINGLE LINE" << endl;
			} else if(c.compare("save")==0 || c.compare("write")==0) {
				cout << "Usage: save <filename>" << endl;
				cout << "Usage: write <filename>" << endl;
				cout << "Saves the current element (including all children) to a new file." << endl;
				cout << "THIS WILL OVERWRITE THE FILE YOU SPECIFY WITH NO CONFIRMATION!" << endl;
				cout << "You must specify a filename." << endl;
			} else if(c.compare("load")==0||c.compare("read")==0) {
				cout << "Usage: load <filename>" << endl;
				cout << "Loads the given file as a child/children of the current element." << endl;
				cout << "You must specify a filename." << endl;
			} else if(c.compare("set")==0||c.compare("=")==0) {
				cout << "Usage: set <perameter> <value>" << endl;
				cout << "Usage: <perameter> = <value>" << endl;
				cout << "Sets one of the element's parameters. The second form is included for " << endl << "convenience. Perameters are present for all elements, although not all" << endl << "elements will use all perameters." << endl;
				cout << "Valid perameters for use with this command are:" << endl;
				cout << "angle arc color cx cy height sections subsections thickness width xscale yscale" << endl;
				cout << "See also the \"attr\", \"name\" and \"text\" commands." << endl;
			} else if(c.compare("attr")==0) {
				cout << "Usage: attr <attribute> <value>" << endl;
				cout << "This sets an attribute (as opposed to perameter) of the current element." << endl << "Attributes are perameters for elements which are not one of the perameters" << endl << "available for every element (for which you would use the \"set\" command)." << endl << "Attributes are stored as strings, but care should be taken as some attributes" << endl << "will be treated as other types of information (such as numbers) and invalid" << endl << "values will cause undefined and undesired behaviour. A common example of an " << endl << "attribute is \"drawmode\" which specifies how an element should be drawn." << endl;
				cout << "Enclose strings with spaces in \"quotes\". You can escape quotes within the" << endl << "string using \\\"." << endl;
				cout << "See also the \"name\", \"set\" and \"text\" commands." << endl;
			} else if(c.compare("name")==0||c.compare("rename")==0) {
				cout << "Usage: name <new name>" << endl;
				cout << "Usage: rename <new name>" << endl;
				cout << "Changes the human-readable name of the current element to the one specified." << endl;
				cout << "Enclose strings with spaces in \"quotes\". You can escape quotes within the" << endl << "string using \\\". Although Pivionics makes no attempt to stop you from using" << endl << "the same name for more than one element, it is recommended that you make your" << endl << "element names unique so that you can use the \"find\" command to quickly" << endl << "jump between elements while you are editing. See the \"find\" command for more" << endl << "information." << endl;
				cout << "See also the \"attr\", \"set\" and \"text\" commands." << endl;
			} else if(c.compare("text")==0||c.compare("string")==0) {
				cout << "Usage: text <text>" << endl;
				cout << "Usage: string <text>" << endl;
				cout << "Changes the current element's text field. This field is primarily for Text" << endl << "elements to set the text that they display. For other elements this can be" << endl << "used for annotation or comments. The currently set text can be viewed using the" << endl << "\"examine\" command; see the help on this command for more information." << endl;;
				cout << "Enclose strings with spaces in \"quotes\". You can escape quotes within the" << endl << "string using \\\"." << endl;
				cout << "See also the \"attr\", \"name\" and \"set\" commands." << endl;
			} else if(c.compare("add")==0||c.compare("new")==0) {
				cout << "Usage: add <element type>" << endl;
				cout << "Usage: new <element type>" << endl;
				cout << "Adds a new element of the given type as a child of the current element." << endl;
				cout << "Use the \"types\" command to see what elements are available for use." << endl;
			} else if(c.compare("delete")==0||c.compare("remove")==0) {
				cout << "Usage: remove" << endl;
				cout << "Usage: delete" << endl;
				cout << "Deletes the current element and any children and exits to its parents." << endl;
				cout << "USE WITH CAUTION. You will not be asked for confirmation." << endl;
			} else if(c.compare("encap")==0||c.compare("encapsulate")==0) {
				cout << "Usage: encap <element type>" << endl;
				cout << "Usage: encapsulate <element type>" << endl;
				cout << "Wraps all children of the current element in a new element of the given type." << endl;
				cout << "Use the \"types\" command to see what elements are available for use." << endl;
			} else if(c.compare("decap")==0||c.compare("decapsulate")==0) {
				cout << "Usage: decap" << endl;
				cout << "Usage: decapsulate" << endl;
				cout << "The opposite of the \"encapsulate\" command. This takes all children of the" << endl << "current element and moves them to the parent as children after any existing" << endl << "elements, leaving the current element empty but otherwise unchanged." << endl;
			} else if(c.compare("{")==0||c.compare("}")==0) {
				cout << "Curly brackets (braces) are used to encapsulate commands to be executed on the" << endl << "last added element. To use them in the editor, curly brackets should all open" << endl << "and close on the same line. Files are processed whole and so new lines can" << endl << "be inserted within a file whereupon they will be treated like spaces. Eg:" << endl << "new Box { set cx -100 set cy -100 set width 100 set height 100 }" << endl << "TAKE CARE TO ENSURE THERE IS A SPACE BEFORE AND AFTER YOUR CURLY BRACKETS!" << endl;
			} else {
				cout << "No help available for " << c << "." << endl;
			}
		}
		cout << endl;
	} else {
		string cmdstr;
		try {
			Element* returnto=current;
			if(cmd.compare("delete")==0 || cmd.compare("remove")==0 ) {
				if(current!=NULL) returnto=current->parent;
			}
			cmdstr=window->command(cmd,current);
			current=returnto;
				
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
	//sleep(10);
#ifdef GPL_LICENSE
	if(argc>1 && strcmp(argv[1],"--license")==0) {
		cout << LICENSE;
		return 0;
	}
#endif
	cout << "Pivionics Window Editor  Copyright (C) 2016  Stephen J Sullivan" << endl;
	cout << "This program is licensed under the GNU General Public License version 3." << endl;
	cout << "This program comes with ABSOLUTELY NO WARRANTY; without even the implied\nwarranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE .\nThis is free software, and you are welcome to redistribute it under certain\nconditions. You should have received a full copy of the license along with\n Pivionics, in a file called \"LICENSE\" and/or \"COPYING\"";
#ifdef GPL_LICENSE
	cout << ", and running this\nprogram with --license will also produce a full copy of it to the shell." << endl;
#else
	cout << "." << endl;
#endif
	cout << "Type \"help\" for a list of available commands." <<endl << endl;
	char chrbuf[128];
	Window window;
	string in_str;
	//window.register_creator("Element",&element_creator<Element>); // Some sort of class casting conflict doing this for other types... We'll come back to this later.
	window.register_creator("Container",&fn_create_container);
	window.register_creator("Rotation",&fn_create_rotation);
	window.register_creator("Rescale",&fn_create_rescale);
	window.register_creator("Circle",&fn_create_circle);
	window.register_creator("Spiral",&fn_create_spiral);
	window.register_creator("Box",&fn_create_box);
	window.register_creator("Text",&fn_create_text);

	if( argc>1 && file_exists(argv[1])) {
		string fn=argv[1];
		window.command("load " + fn,NULL);
	}
	renderer.render_run();
	compositor.link_window(&window);
	compositor.link_renderer(&renderer);
	sleep(3); // TODO: Proper check that renderer is fully initialised
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
