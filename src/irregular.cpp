/*
irregular.cpp - 
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
#include "irregular.h"
#include "stringsplit.h"

Element* fn_create_irregular(void) { return new Irregular; }

Irregular::Irregular(void) {
	attrs["points"]="50,0~0,0~0,50~50,50~50,100~100,100~100,50";
	attrs["drawmode"]="filled";
}

void Irregular::construct(void) {
	access.lock();
	points.clear();
	std::string s = attrs["points"];
	std::string t = "";
	std::vector<std::string> mypoints=split(s,'~');
	std::vector<std::string> omp;
	Point plast[4];
	PointSet tps;
	tps.color = col;
	tps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_DIAGONAL|RENDER_SIDE_INLINE;
	if( attrs["predef"].compare("arrow1")==0 ) {
		attrs["points"]="0,0~-10,10,10,10~-2,10,2,10~-2,50,2,50~0,55";
	}
	if( attrs["drawmode"].compare("filled")==0 ) {
	    tps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INLINE|RENDER_FILL;
	} else if( attrs["drawmode"].compare("wireframe")==0 ) {
	    tps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INLINE|RENDER_SIDE_RADIAL;
	} else if( attrs["drawmode"].compare("outline")==0 ) {
	    tps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_RADIAL;
    } else if( attrs["drawmode"].compare("crisscross")==0 ) {
        tps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INNER|RENDER_SIDE_INLINE|RENDER_SIDE_RADIAL|RENDER_SIDE_DIAGONAL;
    } else if( attrs["drawmode"].compare("hourglass")==0 ) {
        tps.render_flags=RENDER_SIDE_OUTLINE|RENDER_SIDE_INNER|RENDER_SIDE_INLINE|RENDER_SIDE_DIAGONAL;
	}
	int last=0;
	for(auto piter=mypoints.begin(); piter!=mypoints.end(); ++piter) {
		t=*piter;
		omp=split(t,',');
		if(omp.size()==2) {
			plast[0]=plast[2];
			plast[1]=plast[3];
			plast[3].x=stod(omp[0]);
			plast[3].y=stod(omp[1]);
			plast[2]=plast[3];
			if(last<4) last+=2;
		} else if(omp.size()==4) {
			plast[0]=plast[2];
			plast[1]=plast[3];
			plast[2].x=stod(omp[0]);
			plast[2].y=stod(omp[1]);
			plast[3].x=stod(omp[2]);
			plast[3].y=stod(omp[3]);
			if(last<4) last+=2;
		}
		if(last>=4) {
			tps.points.clear();
			tps.points.push_back(plast[0]);
			tps.points.push_back(plast[2]);
			tps.points.push_back(plast[1]);
			tps.points.push_back(plast[3]);
			points.push_back(tps);
		}
	}
	access.unlock();
}
