/*
circle.cpp - 
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
#include <cmath>
#include <string>
#include "circle.h"

Element* fn_create_circle(void) { return new Circle; }
Element* fn_create_spiral(void) { return new Spiral; }

Circle::Circle(void) {
	geometry[2]=1.0;
	geometry[3]=0.0;
    name("");
    sect=360;
	attrs["drawmode"]="outline"; // torus,filled,outline,radius|radial
}

double Circle::mod_inner_radius(double r,double percent) { return r; }
double Circle::mod_outer_radius(double r,double percent) { return r; }


double Spiral::mod_inner_radius(double r,double percent) { return r-(r*percent); }
double Spiral::mod_outer_radius(double r,double percent) { return r-(r*percent); }


unsigned int Circle::mod_color(unsigned int c, double percent) { return c; }

void Circle::construct(void) {
	access.lock();
	// Clear our current point set
	points.clear();
	// Intitialise our perameters:
	Point inner_point,outer_point;// Used and set within loop
	Point tpoint;				  // Used and set within loop
	int w,h; w=geometry[2]/2; h=geometry[3]/2; // Used to work out radius
	double angle_per_sect=angles[1]/sect; 
	double outer_radius = sqrt(w*w+h*h); // Ah, pythagoras....
	double inner_radius = outer_radius - thick;
	long double theta=0.0L; // Initialised within every loop iteration
	if(attrs["drawmode"].compare("filled")==0) { inner_radius=0.0; } // zero inner radius should cause the compositor to clip to triangles
	Point inner_sect_point={inner_radius,0};
	Point outer_sect_point={outer_radius,0};
	Point next_inner_sect_point = { inner_radius*cos( angle_per_sect*1), inner_radius*sin( angle_per_sect*1) };
	Point next_outer_sect_point = { outer_radius*cos( angle_per_sect*1), outer_radius*sin( angle_per_sect*1) };
	long double outer_length = sqrt( pow(next_outer_sect_point.x-outer_sect_point.x,2)+pow(next_outer_sect_point.y-outer_sect_point.y,2));
	long double inner_length = sqrt( pow(next_inner_sect_point.x-inner_sect_point.x,2)+pow(next_inner_sect_point.y-inner_sect_point.y,2));
	outer_length = outer_length/subsect;
	inner_length = inner_length/subsect;
	double percent=0.0;
	double nextpercent=0.0;
	// In order to keep track of the radius, we're going to need to store its initial value:
	double defined_outer_radius=outer_radius;
	double defined_inner_radius=inner_radius;
	
	// Now iterate through each section
	for( unsigned int n=0; n<sect; ++n) {
		// First we need some setup for this section; we need to kno
		PointSet tpointset;
		tpointset.owner=this;
		percent = static_cast<double>(n)/static_cast<double>(sect);// 0.0-1.0 on how far we've drawn...
		nextpercent = static_cast<double>(n+1)/static_cast<double>(sect);// 0.0-1.0 on how far we've drawn...
		// Work out the next point first, hopefully stop the annoying jitter
		inner_radius=this->mod_inner_radius(defined_inner_radius, nextpercent);
		outer_radius=this->mod_outer_radius(defined_outer_radius, nextpercent);
		next_inner_sect_point = { inner_radius*cos( angle_per_sect*(n+1)), inner_radius*sin( angle_per_sect*(n+1)) };
		next_outer_sect_point = { outer_radius*cos( angle_per_sect*(n+1)), outer_radius*sin( angle_per_sect*(n+1)) };
		// Now work out the point we're currently on
		inner_radius=this->mod_inner_radius(defined_inner_radius, percent);
		outer_radius=this->mod_outer_radius(defined_outer_radius, percent);
		inner_sect_point.x = inner_radius*cos( angle_per_sect*n);
		inner_sect_point.y = inner_radius*sin( angle_per_sect*n);
		outer_sect_point.x = outer_radius*cos( angle_per_sect*n);
		outer_sect_point.y = outer_radius*sin( angle_per_sect*n);
		// The lengths may have changed, so lets recalculate those next	
		outer_length = sqrt( pow(next_outer_sect_point.x-outer_sect_point.x,2)+pow(next_outer_sect_point.y-outer_sect_point.y,2));
		inner_length = sqrt( pow(next_inner_sect_point.x-inner_sect_point.x,2)+pow(next_inner_sect_point.y-inner_sect_point.y,2));
		outer_length = outer_length/subsect;
		inner_length = inner_length/subsect;
		
		// Set the render_flags according to the "drawmode" attribute:
		if( attrs["drawmode"].compare("filled")==0 ) {
			tpointset.render_flags=RENDER_SIDE_OUTLINE|RENDER_FILL;
		} else if( attrs["drawmode"].compare("torus")==0 ) {
			tpointset.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INLINE|RENDER_FILL;
		} else if( attrs["drawmode"].compare("triangle")==0 ) {
			tpointset.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_DIAGONAL|RENDER_FILL;
		} else if( attrs["drawmode"].compare("triangle2")==0 ) {
			tpointset.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_DIAGONAL|RENDER_SIDE_INNER|RENDER_FILL;
		} else if( attrs["drawmode"].compare("outline")==0 ) {
			tpointset.render_flags=RENDER_SIDE_OUTLINE;
		} else if( attrs["drawmode"].compare("outlinetriangle")==0 ) {
			tpointset.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_DIAGONAL|RENDER_SIDE_RADIAL;
		} else if( attrs["drawmode"].compare("outlinetriangle2")==0 ) {
			tpointset.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INNER|RENDER_SIDE_RADIAL;
		} else if( attrs["drawmode"].compare("radius")==0 ) {
			tpointset.render_flags=RENDER_SIDE_RADIAL;
        } else if( attrs["drawmode"].compare("crisscross")==0 ) {
            tpointset.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INNER|RENDER_SIDE_INLINE|RENDER_SIDE_RADIAL|RENDER_SIDE_DIAGONAL;
        } else if( attrs["drawmode"].compare("hourglass")==0 ) {
            tpointset.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INNER|RENDER_SIDE_INLINE|RENDER_SIDE_DIAGONAL;
		}

		tpointset.color=col;
		// Iterate through each subsection
		for(unsigned int m=0; m<=subsect; ++m) {
			theta= normalise_angle(PI+static_cast<long double>(angle_per_sect*n) - ((PI-static_cast<long double>(angle_per_sect))/2.0L));
			// Now generate the points for this subsection...
			inner_point.x = (inner_sect_point.x + (m*inner_length) * cos( theta ));	
			inner_point.y = (inner_sect_point.y + (m*inner_length) * sin( theta ));
			outer_point.x = (outer_sect_point.x + (m*outer_length) * cos( theta ));
			outer_point.y = (outer_sect_point.y + (m*outer_length) * sin( theta ));
			if( attrs["dirtyfix"].compare("")!=0 ) {
				if(m==subsect) {
					inner_point.x=next_inner_sect_point.x;
					inner_point.y=next_inner_sect_point.y;
					outer_point.x=next_outer_sect_point.x;
					outer_point.y=next_outer_sect_point.y;
				}
			}
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
