/*
box.cpp - 
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

#include "box.h"

Element* fn_create_box(void) { return new Box; }

Box::Box(void) {
    geometry[2]=1.0;
    geometry[3]=1.0;
    name("");
    sect=1;
    attrs["drawmode"]="filled"; // torus,filled,outline,radius|radial
}

void Box::construct(void) {
	access.lock();
	PointSet ps;
	ps.owner=this;
	Point p;
	int w,h; w=geometry[2]/2; h=geometry[3]/2;
	points.clear();
		ps.color=col;
	for(int n=0;n<sect;++n) {
		ps.points.clear();
        if( attrs["drawmode"].compare("filled")==0 ) {
            ps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INLINE|RENDER_FILL;
        } else if( attrs["drawmode"].compare("outline")==0 ) {
            ps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INLINE|RENDER_SIDE_RADIAL;;
        } else if( attrs["drawmode"].compare("radius")==0 ) {
            ps.render_flags=RENDER_SIDE_RADIAL;
        } else if( attrs["drawmode"].compare("triangle")==0 ) {
            ps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_DIAGONAL|RENDER_FILL;
        } else if( attrs["drawmode"].compare("triangle2")==0 ) {
            ps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_DIAGONAL|RENDER_SIDE_INNER|RENDER_FILL;
        } else if( attrs["drawmode"].compare("outlinetriangle")==0 ) {
            ps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_DIAGONAL|RENDER_SIDE_INLINE|RENDER_SIDE_RADIAL;
        } else if( attrs["drawmode"].compare("outlinetriangle2")==0 ) {
            ps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INNER|RENDER_SIDE_INLINE|RENDER_SIDE_RADIAL;
        } else if( attrs["drawmode"].compare("crisscross")==0 ) {
            ps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INNER|RENDER_SIDE_INLINE|RENDER_SIDE_RADIAL|RENDER_SIDE_DIAGONAL;
        } else if( attrs["drawmode"].compare("hourglass")==0 ) {
            ps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INNER|RENDER_SIDE_INLINE|RENDER_SIDE_DIAGONAL;
        } else {
            ps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INLINE|RENDER_FILL;
		}

		double lengthx = geometry[2] / subsect ;
		double lengthy = geometry[3] / sect;
		for(int m=0;m<=subsect;++m) {
			p.x = 0-w + (lengthx*m);
			p.y = 0-h + (lengthy*n); 
			ps.points.push_back(p);
			p.x = 0-w + (lengthx*m);
			p.y = 0-h + (lengthy*(n+1)); 
			ps.points.push_back(p);
			
		}
		points.push_back(ps);
	}
	access.unlock();

}
