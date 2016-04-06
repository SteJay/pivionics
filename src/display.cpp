/*
display.cpp - 
Copyright (C) 2016 Stephen J Sullivan

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <iostream>
#include "core_elements.h"
using namespace std;

int Display::link_renderer(Renderer* r) {
	access.lock();
	while(!r->ready());
	rend=r;
	access.unlock();
	return 0;
}
int Display::link_window(Window* w) {
	access.lock();
	wind=w;
	access.unlock();
	return 0;
}
int Display::flashout(void) {

}
int Display::compose(void) {
	if( wind==NULL ) {
		// No window set. As the window will contain everything needed to perform the composition, we can't continue
		return ERR_DISPLAY_NO_WINDOW;
	} else {
		if( rend==NULL ) {
			// No renderer set. If we don't know what rendering options we have, we can't perform the composition.
			return ERR_DISPLAY_NO_RENDERER;
		} else {
			// We are a go so far, so we need to take a snapshot of the elements within the window.
			// We're gonna need a mutex...
			access.lock();
			vector<PointSet> working_points;
			
			pointsets.clear();
			
			Point offset={ width/2,height/2 };
			Scale scale={ 1.0,1.0 };
			Origin origin={offset,scale,0.0};
			// Here we call the Window's compose method which will recursively
			// compose all its child elements into one massive set of points
			// Note that this is the stage where all children are rotated according
			// to their geometry and parents geometry.
			wind->compose(origin);
			// Now we have a complete pointset for each element within the Window.
			// We need to figure out how these need to appear to the renderer, ie convert
			// them to Rendergons.
			// Not only that, we also want to remove any consecutive identical points
			// and cull any polygons that are out of our display's range of coordinates
			Point wpoints[4];
			PointSet nowset,lastset;
			Rendergon rg;
			vector<Rendergon> rgv;
			int wp=0;
			int rf;
			int readno=4;
			bool dirty=false;
			for(auto iter=wind->composed_points.begin(); iter!=wind->composed_points.end();++iter) {
				nowset = *iter;
				unsigned int render_flags=nowset.render_flags;
				for( auto piter=nowset.points.begin(); piter!=nowset.points.end(); ++piter) {
					if(rf==3) {
						for(int i=0;i<3;++i) {
							wpoints[i]=wpoints[i+1];
						}
					}
					wpoints[rf]=*piter;
					if(rf<3) rf++;
					readno--;
					if( readno==0) {
						rg.point_count=0;
						if(render_flags&RENDER_SIDE_OUTLINE>0) {
							rg.points[0] = wpoints[0];
							rg.points[1] = wpoints[2];
							if(render_flags&RENDER_FILL) {
								if( render_flags&RENDER_SIDE_INLINE>0) {
									rg.points[2] = wpoints[3];
									rg.points[3] = wpoints[1];
									rg.point_count=4;
									rg.color=nowset.color;
									rg.is_surface=false;
									readno=1;
								} else if( render_flags&(RENDER_SIDE_DIAGONAL|RENDER_SIDE_INNER) >0 ) {
									rg.points[2] = wpoints[3];
									rg.point_count=3;
									rg.color=nowset.color;
									rg.is_surface=false;
									readno=1;
								} else if( render_flags&(RENDER_SIDE_DIAGONAL) >0 ) {
									rg.points[2] = wpoints[1];
									rg.point_count=3;
									rg.color=nowset.color;
									rg.is_surface=false;
									readno=1;
								}
							} else { // unfilled
								rg.point_count=2;
								rg.color=nowset.color;
								rg.is_surface=false;
								readno=1;
							}
						} else if(render_flags&RENDER_SIDE_RADIAL) {
							rg.points[0]=wpoints[0];
							rg.points[1]=wpoints[1];
							rg.point_count=2;
							rg.color=nowset.color;
							rg.is_surface=false;
							readno=1;
						} else {
							rg.points[0]=wpoints[0];
							rg.point_count=1;
							rg.color=nowset.color;
							rg.is_surface=false;
						}
					}
					// Now we cull any points which are repeated, reducing a quad to a triangle or a triangle to a line:
					for(int i=0;i<rg.point_count-1;) {
						if(rg.points[i].x==rg.points[i+1].x&&rg.points[i].y==rg.points[i+1].y) {
							for(int j=i+1; j<rg.point_count; j++) { // Shuffle all points forward
								rg.points[j-1]=rg.points[j];
							}
							rg.point_count--;
						} else {
							++i;
						}
					}
					// Now we've optimised our points, we'll push them to our vector...
					if(rg.point_count>0) rgv.push_back(rg); // Add the Rendergon if it has more than one point
				}
				// Copy our newly worked out rendergons ready to transfer to the renderer
				rendergons=rgv;

			}
			access.unlock();
		}
	}
}
