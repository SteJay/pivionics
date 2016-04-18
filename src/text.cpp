/*
text.cpp - 
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
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_rotozoom.h>
#include "text.h"

Element* fn_create_text(void) { return new Text; }

Text::Text() {
	attrs["font"]="fonts/monofonto/MONOFONT.TTF";
	attrs["fontsize"]="24";
	txt="Test";
	vpsurface=NULL; composed_surface=-1;
}
void Text::compose(Origin origin) {
	access.lock();
	//if(dirty) {
		composed_points.clear();
		if(vpsurface!=NULL) {
			SDL_Surface* tsurf = static_cast<SDL_Surface*>(vpsurface);
		    if(!inherit_position) { origin.position.x=0.0; origin.position.y=0.0; }
		    if(!inherit_scale) { origin.scale.x=1.0; origin.scale.y=1.0; }
		    if(!inherit_angle) { origin.angle=0.0; }
		    origin.position.x+=geometry[0];//-(tsurf->w/2);
		    origin.position.y+=geometry[1];//-(tsurf->h/2);
		    origin.scale.x *= scale[0];
		    origin.scale.y *= scale[1];
		    origin.angle += angles[0];
			Point tp ={ origin.position.x,origin.position.y };
			PointSet tps;
			tps.points.clear();
			tps.points.push_back(tp);
			tp.x=tsurf->w*origin.scale.x;
			tp.y=tsurf->h*origin.scale.y;
			//cout << "Text \"" << txt << "\" has size of " << tp.x << " by " << tp.y << endl;
			tps.points.push_back(tp);
			tps.surface=vpsurface;
			tps.owner=this;
			tps.surface_angle = (180/PI)*normalise_angle(origin.angle);	
			tps.render_flags=RENDER_SURFACE;
			composed_points.push_back(tps);
			//cout << "Text surface composed," << composed_points.size() << endl;
		}
	//}
	access.unlock();
}

void Text::construct(void) {
	if(dirty) {
	access.lock();
	//cout << "Using font " << attrs["font"] << " at size " << attrs["fontsize"] << endl;
	TTF_Font* font=TTF_OpenFont(attrs["font"].c_str(), std::stoi( attrs["fontsize"].c_str(),nullptr,0 ) );
	if(font==NULL) {
		//cout << "FONT LOAD FAILED" << endl;
		points.clear();
	} else {
		//cout << "Font loaded." << endl;
		SDL_Color k = {255,255,255};
		k.r = (col >> 24 ) & 0xFF;
		k.g = (col >> 16 ) & 0xFF;
		k.b = (col >> 8 ) & 0xFF;
		k.a = col & 255; 
		//cout << "Colour: " << hex << (int)k.r << "," << (int)k.g << "," << (int)k.b << "," << (int)k.a << dec << endl;
		//cout << "Colour set." << endl;
		if(vpsurface!=NULL) { SDL_FreeSurface(static_cast<SDL_Surface*>(vpsurface)); }
		//cout << "Old surface freed." << endl;
		vpsurface = TTF_RenderText_Blended(font,txt.c_str(),k);
		//cout << "New surface rendererd." << endl;
		points.clear();
		Point p={0.0};
		PointSet ps;
		ps.points.push_back(p);
		ps.render_flags=RENDER_SURFACE;
		ps.surface = vpsurface;
		points.push_back(ps);
	}
	access.unlock();
	}
	//cout << "Construct complete." << endl;
}
