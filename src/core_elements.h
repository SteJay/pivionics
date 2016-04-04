/*
core_elements.h - Class declarations (Base Classes) for Pivionics
Copyright (C) 2016 Stephen J Sullivan

This file is a part of Pivionics

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
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

#ifndef PI
#define PI 3.141592653589793238462643383279502884L
#endif

#ifndef STRUCTURE_H
#define STRUCTURE_H

class Element {
	private:
		int id_store;
		string namestr;
		string typestr;
	protected:
		vector<int> points;
		double geometry[4];
		long double angles[2];
		double scale[2];
		signed int thick;
		unsigned int sect;
		unsigned int subsect;
		unsigned int col;
		string txt;
		map<string,string> attrs;
		list<Element*> contents;
	public:
		Element* parent;
		friend class Window; // The element store will need to set up the element with appropriate pointers
		Element(void);
		vector<string> get_attrs(void);
		string get_attr(string);
		void set_attr(string, string);
	
		// For these simple getters and setters I won't bother prototyping and defining in seperate files
		double cx(void) {return geometry[0];}
		double cy(void) {return geometry[1];}
		double width(void) {return geometry[2];}
		double height(void) {return geometry[3];}
		long double angle(void) {return angles[0];}
		long double arc(void) {return angles[1];}
		signed int thickness(void) {return thick;}
		unsigned int sections(void) {return sect;}
		unsigned int subsections(void) {return subsect;}
		unsigned int color(void) {return col;}
		string text(void) {return txt;}
		
		void cx(double d) {geometry[0]=d;}
		void cy(double d) {geometry[1]=d;}
		void width(double d) {geometry[2]=d;}
		void height(double d) {geometry[3]=d;}
		void angle(long double d) {angles[0]=d;}
		void arc(long double d) {angles[1]=d;}
		void thickness(signed int d) {thick=d;}
		void sections(unsigned int d) {sect=d;}
		void subsections(unsigned int d) {subsect=d;}
		void color(unsigned int d) {col=d;}
		void text(string d) {txt=d;}
		

		string type(void);// There's no direct type setting function; that's because it is set by the Window containing it
		string name(void);// You can get the name in the same way...
		void name(string);// But you can also set it. No validation is done - this is just for human reference really
		
		virtual void construct(void); // Construct the point set according to the specified and parent geometry
//		virtual void add_point(int,int);
//		virtual void add_line(int,int,int,int);
//		virtual void add_triangle(int,int,int,int,int,int);
		//virtual void pre_construct(void);
		//virtual void construct_loop(void);
		//virtual void post_construct(void);
};


class Window: public Element {
/*
	The Window class probably has most of the functionality out of all the elements in this file.
	Windows should not be treated as elements (although they technically are), they are the root
	container for a set of elements to be visible at once. Display classes will contain one or 
	more Windows which the user can switch between. The renderer will only update for the currently
	visible window in the Display.
*/
	private:
		map<string,Element* (*)(void)> creators;
	public:
//		Window(void);
		~Window();
		void register_creator(string n,Element* (*c)(void)) { creators[n]=c; }
		list<string> list_creators(void);		// Used for user interface and debugging
		list<Element*> list_elements(Element*); 	// Used for user interface and debugging
		int children(Element*);
		Element* add(string,Element*);	// Add an element as the given type
		Element* parent(Element*);
		Element* sibling(unsigned int, Element*);
		Element* child(unsigned int, Element*);
		
		Element* find_name(string);
		Element* find_name(string,int);
		Element* find_name(string,int,Element*);
		void remove(int);			// Remove the nth child of the current element
		void insert(Element*);		// Insert an element of the given type between the given element and its parent


		virtual string command(string,Element*);
		virtual list<string> command(list<string>,Element*);
		void save(string);			// Save the window.
		void save(string,Element*);			// Save the window.
		void save(ofstream*,Element*);			// Save the window.
		void save(ofstream*,Element*,string);			// Save the window.
		void load(string);
		void load(string,Element*);

};


class Display {
	public:
	int placeholder;
};

/* In addition to the main Display, Window, Window and Element classes, the following
		void add(Element*);
		void add(Element*);
   container elements are essential. These provide a method to group various elements into one */

class Container:public Element {
/* 
Basic container; Child elements inherit the container's position as their centre points, but their rotation remains unchanged.
All child elements are still calculated recursively. 	   
*/

	public:
	int placeholder;

};

class Rotation:public Container {
/*
This is identical in concept to the Container, but child elements also inherit rotation from the container.
*/
	public:
	int placeholder;
};

class Offset:public Container {
/*
Offset container. This offsets the centre of rotation, allowing its contents to "orbit" the centre of the offset by the radius described by that offset.
This does not pass on rotation, so child elements will still have their original orientation.
*/
	public:
	int placeholder;
};

class OffsetRotation: public Container {
/*
Identical in conecpt to the Offset, but child elements do inherit rotation.
*/
	public:
	int placeholder;
};

class StaticContainer:public Container {
/*
This container is used where its contents will never change relative to one another.
It will actually delete its children once it has calculated the points within it, and once this is done its points will never be recalculated.
Please note that the StaticContainer itself can continue to be moved, rotated and scaled - it is not completely static on the screen - rather its internal
contents never need to be updated.

This allows for the ability to cache points without having to check recursively if they are dirty (if they have changed since the last frame), hopefully
allowing you to squeeze better perfomance out of the process. Because it only stores points, you cannot use these to contain text or other bitmap/surfaces.

It is worth re-stating here: ANY CHILD ELEMENTS OF A STATIC CONTAINER WILL CEASE TO EXIST AS SOON AS THE CONTAINER IS CALCULATED. You cannot, I repeat CAN NOT
change the contents of a StaticContainer once its "construct" function is called; if you try to do so the original contents of the container will be lost and
replaced by whatever elements you add, and at this point it will also delete its point cache! YOU HAVE BEEN WARNED.
*/
};

class OptionContainer:public Container{
/* This container can be turned on and off dynamically; useful for features that may need turning off on occasions.*/
	public:
		int placeholder;
};

template <class T> T* element_creator(void) {
	T* t;
	t=new T;
	return t;
};

Element* create_element(void); 
Element* create_container(void); 
Element* create_rotation(void);
Element* create_offset(void); 
Element* create_offset_rotation(void); 
Element* create_static_container(void); 


#endif
