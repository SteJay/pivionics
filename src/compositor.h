/*
compositor.h - Compositor Class Definitions 
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

/* 

The Compositor is probably the most time-critical component of
Pivionics. The compositor is responsible for taking all the
individual geometries given to it by active Window and Element
class instances and arranging them as efficiently as possible for
the renderer.

Once the compositor is done, the rendering loop will have a complete
list of visible polygons, what colour they are and what method
should be used to draw them.

The compositor, therefore, needs not only to colourise and format
its arrays of points (indeed much of this should be done by the
individual elements before they get transferred to the Compositor),
and speed is of the essence as the Compositor needs to keep up
with the rendering.

This note is more for my benefit in getting my thoughts together as
it is for you!

*/

#include <vector>
#include <list>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "core_elements.h"
#include "stringsplit.h"


class Compositor:public Display {
/*
	The Compositor Class
*/
	private:
		vector<Point> compositepoints;
		vector<unsigned int> compositecolors;
		map<unsigned int,vector<Point>> layers;
	protected:
	public:
		
};

