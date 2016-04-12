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
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_rotozoom.h>
#include "text.h"

Element* fn_create_text(void) { return new Text; }

Text::Text() {
	attrs["font"]="./fonts/monofonto/MONOFONT.TTF";
	attrs["fontsize"]="24";
	txt="Test";
	vpsurface=NULL; vpcomposed_surface=NULL;
}
void Text::compose(Origin origin) {
	access.lock();
	SDL_Surface* tsurf = rotozoomSurfaceXY( static_cast<SDL_Surface*>(vpsurface), (180.0/PI)*origin.angle, origin.scale.x,origin.scale.y,SMOOTHING_ON);
	vpcomposed_surface=tsurf;
	Point tp ={ origin.position.x,origin.position.y };
	PointSet tps;
	tps.points.clear();
	tps.points.push_back(tp);
	tp.x=tsurf->w;
	tp.y=tsurf->h;
	tps.points.push_back(tp);
	tps.surface=vpsurface;
	tps.render_flags=RENDER_SURFACE;
	composed_points.clear();
	composed_points.push_back(tps);
	cout << "Text surface composed," << composed_points.size() << endl;
	access.unlock();
}

void Text::construct(void) {
	access.lock();
	cout << "Using font " << attrs["font"] << " at size " << attrs["fontsize"] << endl;
	TTF_Font* font=TTF_OpenFont(attrs["font"].c_str(), stoi( attrs["fontsize"].c_str(),nullptr,0 ) );
	if(font==NULL) {
		//cout << "FONT LOAD FAILED" << endl; 
	} else {
		//cout << "Font loaded." << endl;
		SDL_Color k;
		k.r = (col & 4278190080) << 24;
		k.g = (col & 16711680) << 16;
		k.b = (col & 65280) << 8;
		k.a = col & 255;
		//cout << "Colour set." << endl;
		if(vpsurface!=NULL) { SDL_FreeSurface(static_cast<SDL_Surface*>(vpsurface)); }
		//cout << "Old surface freed." << endl;
		vpsurface = TTF_RenderText_Blended(font,"TEST",k);
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
	//cout << "Construct complete." << endl;
}
