/*
element.cpp - Implementation for the Element class
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
#include <vector>
#include <list>
#include <map>
#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>
#include "core_elements.h"

using namespace std;
Container::Container() { 
	name(""); 
	calc_rel_pos=true;
	calc_rel_scale=true;
	calc_rel_rot=true;
} 

Rotation::Rotation(void) {
	name("");
	compose_order=COMPOSE_ORDER_STR; // Scale, Translate, Rotate
	calc_rel_rot=false;
}

Rescale::Rescale(void) {
	name("");
	calc_rel_scale=false;
	compose_order=COMPOSE_ORDER_TRS; // Translate, Rotate, Scale
}

Element* fn_create_container(void) { return new Container; }
Element* fn_create_rotation(void) { return new Rotation; }
Element* fn_create_rescale(void) { return new Rescale; }

bool Container::pre_compose(Origin origin) {
	composed_points.clear();
    if(!inherit_position) { origin.position.x=0.0; origin.position.y=0.0; }
    if(!inherit_scale) { origin.scale.x=1.0; origin.scale.y=1.0; }
    if(!inherit_angle) { origin.angle=0.0; }
	if(calc_rel_pos) {
	    origin.position.x+=geometry[0];
	    origin.position.y+=geometry[1];
	} else {
	    origin.position.x=geometry[0];
	    origin.position.y=geometry[1];
	}
	if(calc_rel_scale) {
	    origin.scale.x *= scale[0];
	    origin.scale.y *= scale[1];
	} else {
		origin.scale.x=scale[0];
		origin.scale.y=scale[1];
	}
	if(calc_rel_rot) {
	    origin.angle += angles[0]; // Not interested in passing on rotation as we'll do this after...
	} else {
		origin.angle = normalise_angle(angles[0]);
	}
//    Element* el;
    for(auto iter=contents.begin(); iter!=contents.end();++iter) {
        // Now we compose each child element...
        auto el2=*iter;
        el2->compose(origin);
   /* }
    // Now we compose this element as a whole
    PointSet tps,tps2;
    Point tp,tp2;
    vector<PointSet> tpsv;
    for(auto iter=contents.begin(); iter!=contents.end();++iter) {
        // And add the composed_points of our children that we worked out earlier
        auto el = *iter;
        composed_points.insert(composed_points.end(),el->composed_points.cbegin(),el->composed_points.cend());
        el->composed_points.clear();
		*/
        composed_points.insert(composed_points.end(),el2->composed_points.cbegin(),el2->composed_points.cend());
        el2->composed_points.clear();
    }
	return false;
}
bool Rotation::post_compose(Origin origin) {
	PointSet tps,tps2; tps.owner=this; tps2.owner=this;
	Point tp,tp2,tp3;
	vector<PointSet> tpsv;
	origin.angle = normalise_angle(PI+origin.angle);
    // Now the extra rotation magic...  
    for( auto iter=composed_points.begin(); iter!=composed_points.end();++iter) {
        tps = *iter;
        tps2=*iter;
        tps2.points.clear();
        for(auto magnumpi=tps.points.begin();magnumpi!=tps.points.end();++magnumpi) {
			if((tps.render_flags&RENDER_SURFACE)>0) {
	            tp=*magnumpi;
				if( magnumpi - tps.points.begin() < 1 ) {
				    tp2=*magnumpi;
					tp.x=origin.position.x-tp.x;
					tp.y=origin.position.y-tp.y;
	            	tp2.x=tp.x*cos(origin.angle)-tp.y*sin(origin.angle);
	            	tp2.y=tp.x*sin(origin.angle)+tp.y*cos(origin.angle);
					tp2.x+=origin.position.x;
					tp2.y+=origin.position.y;
					tp=tp2; 
				} else {
				}
	            tps2.points.push_back(tp);
				// This is a special case for surfaces as they are rotated and scaled differently
			} else {
	            tp=*magnumpi;
				tp.x=origin.position.x-tp.x;
				tp.y=origin.position.y-tp.y;
	            tp2.x=tp.x*cos(origin.angle)-tp.y*sin(origin.angle);
	            tp2.y=tp.x*sin(origin.angle)+tp.y*cos(origin.angle);
				tp2.x+=origin.position.x;
				tp2.y+=origin.position.y;
	            tps2.points.push_back(tp2);
			}			
        }
		if((tps2.render_flags&RENDER_SURFACE)>0) {
			Element* argh=static_cast<Element*>(tps2.owner);
			if( argh->inherit_angle ) {
				tps2.surface_angle += (normalise_angle(origin.angle-PI) * (180/PI));
			}
			//cout << "Composed surface: " << tps2.points[0].x << ", " << tps2.points[0].y << ", " << tps2.points[1].x << ", " << tps2.points[1].y << ", @" << tps2.surface_angle << "(origin says " << origin.angle << ")" << endl;
		}
        tpsv.push_back(tps2);
    }
    composed_points=tpsv;
	return false;
	// Composition is good to go!
}
bool Rescale::post_compose(Origin origin) {
	PointSet tps,tps2; tps.owner=this; tps2.owner=this;
	Point tp;
	vector<PointSet>tpsv;
    for( auto iter=composed_points.begin(); iter!=composed_points.end();++iter) {
        tps = *iter;
		tps2 = *iter;
		tps2.points.clear();
        for(auto magnumpi=tps.points.begin();magnumpi!=tps.points.end();++magnumpi) {
            tp=*magnumpi;
			tp.x=origin.position.x-tp.x;
			tp.y=origin.position.y-tp.y;
			tp.x*=origin.scale.x*-1;
			tp.y*=origin.scale.y*-1;
			tp.x+=origin.position.x;
			tp.y+=origin.position.y;
            tps2.points.push_back(tp);
			
        }
        tpsv.push_back(tps2);
    }
    composed_points=tpsv;
	return false;
	
}
