#include "core_elements.h"

// Circle Creator function
Element* fn_create_circle(void);
Element* fn_create_spiral(void);

// Circle Class Definition
class Circle: public Element {
	public:
		Circle();
		virtual void construct(void);	
		virtual double mod_inner_radius(double,double);
		virtual double mod_outer_radius(double,double);
		virtual unsigned int mod_color(unsigned int, double);
};

class Spiral: public Circle {
	public:
		double mod_inner_radius(double,double);
		double mod_outer_radius(double,double);
};
