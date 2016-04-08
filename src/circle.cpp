#include <iostream>
#include <cmath>
#include "circle.h"

Element* fn_create_circle(void) { return new Circle; }

Circle::Circle(void) {
    name("");
    geometry[0]=0.0;
    geometry[1]=0.0;
    geometry[2]=1.0;
    geometry[3]=1.0;
    angles[0]=0;
    angles[1]=PI*2;
    scale[0]=1.0;scale[1]=1.0; // OBSOLETE?
    thick=1;
    sect=360;
    subsect=1;
    col=0xFFFFFFFF;
    txt="";
    inherit_position=false;
    inherit_scale=false;
    inherit_angle=false;
    parent=NULL;
	attrs["drawmode"]="torus"; // torus,filled,outline,radius|radial
	attrs["offset_inner"]="false"; // Offset the inner circle by half a step (more efficient draw)
}

void Circle::construct(void) {
	access.lock();
	// Clear our current point set
	points.clear();
	int w,h; w=geometry[2]/2; h=geometry[3]/2;
	double angle_per_sect=normalise_angle(angles[1]/sect);
	double angle_per_subsect=normalise_angle(angle_per_sect/subsect);
	double outer_radius = sqrt(w*w+h*h); // Ah, pythagoras....
	double inner_radius = outer_radius - thick;
	double theta;
	double outer_length = (2*outer_radius*sin(PI/sect))/subsect;
	double inner_length = (2*inner_radius*sin(PI/sect))/subsect;
	theta=0;
	Point inner_sect_point;
	Point outer_sect_point;
	Point inner_point,outer_point;
	Point tpoint;

	// Now iterate through each section
	for( unsigned int n=0; n<sect; ++n) {
		// First we need some setup for this section; we need to kno
		PointSet tpointset;
		inner_sect_point.x = inner_radius*cos( angle_per_sect*n);
		inner_sect_point.y = inner_radius*sin( angle_per_sect*n);
		outer_sect_point.x = outer_radius*cos( angle_per_sect*n);
		outer_sect_point.y = outer_radius*sin( angle_per_sect*n);
//		tpointset.points.push_back(outer_sect_point);
//		tpointset.points.push_back(inner_sect_point);
		
		
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
		tpointset.color=col;
		// Iterate through each subsection
		for(unsigned int m=0; m<=subsect; ++m) {
//			theta = normalise_angle((angle_per_sect*n) + (PI/2) + (angle_per_subsect*m));
			theta= normalise_angle(PI+(angle_per_sect*n) - ((PI-angle_per_sect)/2));
			// Now generate the points for this subsection...
			inner_point.x = round(inner_sect_point.x + (m*inner_length) * cos( theta ));	
			inner_point.y = round(inner_sect_point.y + (m*inner_length) * sin( theta ));
			outer_point.x = round(outer_sect_point.x + (m*outer_length) * cos( theta ));
			outer_point.y = round(outer_sect_point.y + (m*outer_length) * sin( theta ));
			// Push the points onto our temporary poly
		//cout << "\tCircle part: Theta: " << theta << "\t, Points: " << outer_point.x << "\t" << outer_point.y << "\t\t" << inner_point.x << "\t" << inner_point.y << endl;
			tpointset.points.push_back(outer_point);
			tpointset.points.push_back(inner_point);
		}
		// And finally push the temporary poly to our points
		points.push_back(tpointset);
	//cout << "\tset!" << endl;
	}
	// There we go, good to go!
	access.unlock();
}
