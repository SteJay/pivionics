#include "core_elements.h"

// Circle Creator function
Element* fn_create_circle(void);

// Circle Class Definition
class Circle: public Element {
	public:
		Circle();
		virtual void construct(void);	
//		virtual double mod_r(unsigned int);
};

class CircleScale: public Circle {
	public:
		virtual void construct(void);
};

class Spiral: public Circle {
	public:
		virtual void construct(void);
};
