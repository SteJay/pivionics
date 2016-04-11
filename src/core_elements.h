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
#include <thread>
#include <mutex>
#include <cmath>
#include <sys/stat.h>
// I'm going to have to take this out at some stage :(
// It won't do any harm for now.
using namespace std;

#ifndef PI
#define PI 3.141592653589793238462643383279502884L
#endif

#ifndef STRUCTURE_H
#define STRUCTURE_H

double normalise_angle(double);
long double normalise_angle(long double);
/* The constants below are used to control which sides are drawn
   given a set of points. These are used by the Compositor class
   and (if used) the Compositor. 
   
   to explain them, consider four points:

                 1    3
                  2  4  

*/
const unsigned int RENDER_SIDE_OUTLINE=1; // Draw a line from 1-3
const unsigned int RENDER_SIDE_INLINE=2;  // Draw a line from 2-4
const unsigned int RENDER_SIDE_RADIAL=4;  // Draw a line from 1-2
const unsigned int RENDER_SIDE_DIAGONAL=8;// Draw a line from 2-3
const unsigned int RENDER_SIDE_INNER=16;  // Used with the above, instead draws from 1-4
const unsigned int RENDER_FILL=32;   // Draw a filled polygon rather than lines, filling in the missing side
const unsigned int RENDER_ALPHA=64;  // Inform the compositor this is not a solid surface and so it should not clip points underneath

/* Compose order constants. Use these to control how the element is composed */

const unsigned int COMPOSE_ORDER_SRT = 57; // 11 10 01
const unsigned int COMPOSE_ORDER_STR = 54; // 11 01 10
const unsigned int COMPOSE_ORDER_TSR = 30; // 01 11 10
const unsigned int COMPOSE_ORDER_TRS = 27; // 01 10 11
const unsigned int COMPOSE_ORDER_RTS = 39; // 10 01 11
const unsigned int COMPOSE_ORDER_RST = 45; // 10 11 01



/*
   Now for the obligatory and ridiculously long list of error constants *cough*
*/

// Compositor class errors
const int ERR_DISPLAY_NO_WINDOW=-1;	// No window set for display
const int ERR_DISPLAY_NO_RENDERER=-2; // No renderer set for display
const int ERR_DISPLAY_RENDERER_NOT_READY=-3; // Renderer is not running or is not ready
const int ERR_DISPLAY_INCONGRUENT_POINTSET=-4; // Compositor came across a pointset that did not make sense
const int ERR_RENDERER_CANNOT_INIT=-100;	// Initialisation of display driver failed
const int ERR_RENDERER_CANNOT_CREATE_WINDOW=-101; // Initialisation of display driver failed
const int ERR_RENDERER_CANNOT_CREATE_SCREEN=-102; // Initialisation of display driver failed
const int ERR_RENDERER_CANNOT_CREATE_RENDER=-103; // Initialisation of display driver failed

/* X and Y points packed into one */
struct Point {
	double x;
	double y;
};

struct IntPoint {
	int x;
	int y;
};

struct Scale {
	double x;
	double y;
};

struct Origin {
	Point position;
	Scale scale;
	long double angle;
};

/* The following struct is used to pass points to the compositor */
struct PointSet {
    unsigned int render_flags;				// This is a combination of the above RENDER_ flags
    vector<Point> points; // A set of point pairs
    unsigned int color;				// The colour of this polygon
};

/* The following struct is used by the compositor to pass points to the renderer */
struct Rendergon {
	IntPoint points[4];
	unsigned int point_count;
	unsigned int color;
	bool is_surface;
	void* surface;
};







/* The Element class.*/
class Element {
	private:
		int id_store;
		string namestr;
		string typestr;
	protected:
		mutex access;
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
		bool inherit_position;
		bool inherit_angle;
		bool inherit_scale;
		unsigned int compose_order;
		vector<PointSet> points;
		vector<PointSet> composed_points;
		Element* parent;
		friend class Window; // The element store will need to set up the element with appropriate pointers
		friend class Compositor;
		Element(void);
		~Element();
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
		double scale_x(void) { return scale[0]; }
		double scale_y(void) { return scale[1]; }
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
		void scale_x(double d) { scale[0]=d; }
		void scale_y(double d) { scale[1]=d; }
		void text(string d) {txt=d;}
		

		string type(void);// There's no direct type setting function; that's because it is set by the Window containing it
		string name(void);// You can get the name in the same way...
		void name(string);// But you can also set it. No validation is done - this is just for human reference really
		
		virtual void compose(Origin);
		virtual bool pre_compose(Origin);
		virtual bool post_compose(Origin);
		virtual void construct(void); // Construct the point set according to the specified geometry.
		virtual bool pre_construct(void);
		virtual bool post_construct(void);
};


class Window: public Element {
/*
	The Window class probably has most of the functionality out of all the elements in this file.
	Windows should not be treated as elements (although they technically are), they are the root
	container for a set of elements to be visible at once. Compositor classes will contain one or 
	more Windows which the user can switch between. The renderer will only update for the currently
	visible window in the Compositor.
*/
	private:
		map<string,Element* (*)(void)> creators;
	public:
		Window(void);
//		~Window();
		friend class Compositor;
		void register_creator(string n,Element* (*c)(void)) { creators[n]=c; }
		list<string> list_creators(void);		// Used for user interface and debugging
		list<Element*> list_elements(Element*); 	// Used for user interface and debugging
		int children(Element*);
		Element* add(string,Element*);	// Add an element as the given type
		Element* del(Element*);
		Element* del_children(Element*);
		Element* move(Element*,Element*);
		Element* encap(string,Element*);
		Element* decap(Element*);
		Element* get_parent(Element*);
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

class Renderer {
	protected:
		mutex access;
		bool dirty;
		unsigned int fps_cap;
		vector<Rendergon> points;
		bool run;
		unsigned int fps;
		thread* runthread;
	public:
		int width;
		int height;
		Renderer();
		~Renderer();
		bool ready(void);	
		virtual int init(void);
		virtual int shutdown(void);
		virtual bool set_rendergons(const vector<Rendergon>*); // Called by Compositor or Compositor
		virtual bool render_frame(void); // Called by render_loop
		virtual bool render_loop(void);	 // The main function, called into its own thread.
		virtual void render_run(void);
		virtual void render_stop(void);
		virtual void draw_point(unsigned int*, const IntPoint*); // called by render_frame
		virtual void draw_line(unsigned int*,const IntPoint*, const IntPoint*); // Called by render_frame
		virtual void draw_triangle(unsigned int*, const IntPoint*, const IntPoint*, const IntPoint*); // Called by render_frame
		virtual void draw_quad(unsigned int*, const IntPoint*, const IntPoint*, const IntPoint*, const IntPoint*); // called by render_frame
		virtual void draw_surface(void*,const IntPoint*);
		virtual void flip(void);
		virtual void clear(void);
		unsigned int get_fps();

};

class Compositor {
	/* 
	The display is usually a base class for the Compositor which performs
	the donkey work of figuring out what to actually display. As such this
	class provides the basic features to interact with the Window and Element
	classes.
	*/
	private:
		mutex access;
	protected:
		Window* wind;
		Renderer* rend;
		vector<PointSet> pointsets;
		vector<Rendergon> rendergons;
		double width;
		double height;
	public:
		~Compositor();
		virtual int link_renderer(Renderer*);
		virtual int link_window(Window*);
		void p2p( IntPoint*,Point* );
		virtual int compose(void);
		virtual int flashout(void);
};

/* In addition to the main Compositor, Window, Window and Element classes, the following
		void add(Element*);
		void add(Element*);
   container elements are essential. These provide a method to group various elements into one */

class Container:public Element {
/* 
Basic container; Child elements inherit the container's position as their centre points, but their rotation remains unchanged.
All child elements are still calculated recursively. 	   
*/
	protected:
		bool calc_rel_scale;
		bool calc_rel_pos;
		bool calc_rel_rot;
	public:
		Container();
		bool pre_compose(Origin);
};

class Rotation:public Container {
/*
This is identical in concept to the Container, but child elements also inherit rotation from the container.
*/
	public:
	    Rotation();
		bool post_compose(Origin);
};

class Rescale:public Container {
	public:
	    Rescale();
		bool post_compose(Origin);

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

Element* fn_create_element(void); 
Element* fn_create_container(void); 
Element* fn_create_rotation(void);
Element* fn_create_rescale(void);

#endif
