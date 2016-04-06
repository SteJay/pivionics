#include <iostream>
#include <cmath>
#include "circle.h"

Element* fn_create_circle(void) { return new Circle; }

Circle::Circle(void):Element() {
	sect=100;
	attrs["drawmode"]="torus"; // torus,filled,outline,radius|radial
	attrs["offset_inner"]="false"; // Offset the inner circle by half a step (more efficient draw)
}

void Circle::construct(void) {
	access.lock();
	// Clear our current point set
	points.clear();
	int w,h; w=geometry[2]/2; h=geometry[3]/2;
	double angle_per_sect=angles[1]/sect;
	double angle_per_subsect=angle_per_sect/subsect;
	double outer_radius = sqrt(w*w+h*h); // Ah, pythagoras....
	double inner_radius = outer_radius - thick;
	double theta = (PI/2)-(PI-angle_per_sect)/2; // Maybe this can be reduced??
	double outer_length = outer_radius - (outer_radius*cos(theta));
	double inner_length = inner_radius - (inner_radius*cos(theta));

	Point inner_sect_point;
	Point outer_sect_point;
	Point inner_point,outer_point;
	Point tpoint;

	// Now iterate through each section
	for( unsigned int n=0; n<sect; ++n) {
		inner_sect_point.x = inner_radius*cos( angle_per_sect*n);
		inner_sect_point.y = inner_radius*sin( angle_per_sect*n);
		outer_sect_point.x = outer_radius*cos( angle_per_sect*n);
		outer_sect_point.y = outer_radius*sin( angle_per_sect*n);
		
		// First we need some setup for this section; we need to kno
		PointSet tpointset;
		
		// Set the render_flags according to the "drawmode" attribute:
		if( attrs["drawmode"].compare("filled")==0 ) {
			tpointset.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_RADIAL|RENDER_FILL;
		} else if( attrs["drawmode"].compare("torus")==0 ) {
			tpointset.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INLINE|RENDER_SIDE_RADIAL|RENDER_FILL;
		} else if( attrs["drawmode"].compare("outline")==0 ) {
			tpointset.render_flags=RENDER_SIDE_OUTLINE;
		} else if( attrs["drawmode"].compare("radius")==0 ) {
			tpointset.render_flags=RENDER_SIDE_RADIAL;
		}
		// Iterate through each subsection
		for(unsigned int m=0; m<subsect; ++m) {
			// Now generate the points for this subsection...
			inner_point.x = inner_sect_point.x + inner_length*cos( (angle_per_sect*n)+(angle_per_subsect*m) + theta);
			inner_point.y = inner_sect_point.y + inner_length*sin( (angle_per_sect*n)+(angle_per_subsect*m) + theta);
			outer_point.x = outer_sect_point.x + outer_length*cos( (angle_per_sect*n)+(angle_per_subsect*m) + theta);
			outer_point.y = outer_sect_point.y + outer_length*sin( (angle_per_sect*n)+(angle_per_subsect*m) + theta);
			// Push the points onto our temporary poly
			tpointset.points.push_back(outer_point);
			tpointset.points.push_back(inner_point);
		}
		// And finally push the temporary poly to our points
		points.push_back(tpointset);
	}
	// There we go, good to go!
	access.unlock();
}
