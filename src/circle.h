#include "core_elements.h"

// Circle Creator function
Element* fn_create_circle(void);

// Circle Class Definition
class Circle: public Element {
	public:
		Circle();
		virtual void construct(void);	
};
