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
#include <cmath>
#include "core_elements.h"
using namespace std;

Compositor::~Compositor() {
	rend=NULL;
	wind=NULL;
}

void Compositor::p2p(IntPoint* intpoint,Point* point) {
	intpoint->x=round(point->x);
	intpoint->y=round(point->y);
}

int Compositor::link_renderer(Renderer* r) {
	access.lock();
	while(!r->ready());
	rend=r;
	if(wind!=NULL) {
		wind->cx( rend->width/2 );
		wind->cy( rend->height/2 );
		wind->width( rend->width );
		wind->height( rend->height );
	}
	access.unlock();
	return 0;
}
int Compositor::link_window(Window* w) {
	access.lock();
	wind=w;
	if(rend!=NULL) {
		wind->cx( rend->width/2 );
		wind->cy( rend->height/2 );
		wind->width( rend->width );
		wind->height( rend->height );
	}
	access.unlock();
	return 0;
}
int Compositor::flashout(void) {
	
}

int Compositor::compose(void) {
	///cout << "Compositor::compose called" << endl;
	if( wind==NULL ) {
		// No window set. As the window will contain everything needed to perform the composition, we can't continue
		return ERR_DISPLAY_NO_WINDOW;
	} else {
		if( rend==NULL ) {
			// No renderer set. If we don't know what rendering options we have, we can't perform the composition.
			return ERR_DISPLAY_NO_RENDERER;
		} else {
			///cout << "Compositor ok, proceeding to compose..." << endl;
			// We are a go so far, so we need to take a snapshot of the elements within the window.
			// We're gonna need a mutex...
			access.lock();
			///cout << "Compositor lock obtained" << endl;
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
			///cout << "Window composed, generated ."<<endl;
			Point wpoints[4]; for(int i=0;i<4;i++) { wpoints[i].x=0.0; wpoints[i].y=0.0; }
			PointSet nowset;
			nowset.render_flags = RENDER_SIDE_OUTLINE|RENDER_SIDE_INLINE|RENDER_FILL;
			nowset.color=0;
			Rendergon rg;
			rg.point_count=0;
			rg.color=0;
			rg.is_surface=false;
			rg.surface_index=-1;
			vector<Rendergon> rgv;
			int wp=0;
			///cout << "Compositor now starting its work..." << endl;
			int ino=0,iino=0;
			for(auto iter=wind->composed_points.begin(); iter!=wind->composed_points.end();++iter) {
				ino++;
				//cout << "Compositor on composed pontset number " << ino << "..." << endl;
				nowset = *iter;
				unsigned int render_flags=nowset.render_flags;
				iino=0;
				auto piter=nowset.points.begin(); // Get Iterator
				if( (render_flags & RENDER_SURFACE)>0 ) {
					wpoints[0]=*piter;
					wpoints[1]=*piter;
					++piter;
					if(piter!=nowset.points.end()) wpoints[1]=*piter;
					p2p(&rg.points[0],&wpoints[0]);
					p2p(&rg.points[1],&wpoints[1]);
					rg.is_surface=true;
					rg.point_count=2;
					//rg.surface = nowset.surface;
					rg.surface_index = rend->allocate_surface(nowset.surface);
					rg.surface_angle = nowset.surface_angle;
					rgv.push_back(rg);
					//cout << "Compositor dealt with surface at " << nowset.surface << " for " << rg.points[0].x << "," << rg.points[0].y << "," << rg.points[1].x << "," << rg.points[1].y << "." << endl;
				} else {	
					// Below we get the first two points...
					for(;piter!=nowset.points.end()&&iino<2; ++piter) { wpoints[iino+2]=*piter; ++iino; }
					// Now we loop through our iterator...
					while(piter!=nowset.points.end() ) {
						for(int ijuju=0;piter!=nowset.points.end()&&ijuju<2; ++piter) { 
							wpoints[0]=wpoints[1]; wpoints[1]=wpoints[2]; wpoints[2]=wpoints[3];
							wpoints[3]=*piter; ++ijuju; ++iino;
						}
#ifdef REVERSE_COLOR_ORDER
						rg.color= __builtin_bswap32(nowset.color);
#else
						rg.color=nowset.color;
#endif
					//cout << "\t\tTempQuad:\n"; for(int i=0;i<=3;i++) {//cout <<"\t\t" << wpoints[i].x-360 << ", " <<wpoints[i].y-360 << endl; }
						rg.point_count=0;
					//cout << render_flags;
						if((render_flags & RENDER_SIDE_OUTLINE) > 0) {
					//cout << "\t\tRENDER_SIDE_OUTLINE selected" << endl;
							p2p(&rg.points[0],&wpoints[0]);
							p2p(&rg.points[1],&wpoints[2]);
							if((render_flags&RENDER_FILL)>0) {
						//cout << "\t\tRENDER_FILL" << endl;
								if( (render_flags&RENDER_SIDE_INLINE)>0) {
							//cout << "\t\tRENDER_SIDE_INLINE" << endl;
									p2p(&rg.points[2],&wpoints[3]);
									p2p(&rg.points[3],&wpoints[1]);
									rg.point_count=4;
									rg.is_surface=false;
								} else if( (render_flags&RENDER_SIDE_INNER) >0 ) {
							//cout << "\t\tRENDER_SIDE_DIAGONAL|RENDER_SIDE_INNER" << endl;
									p2p(&rg.points[2],&wpoints[3]);
									rg.point_count=3;
									rg.is_surface=false;
								} else if(( render_flags&(RENDER_SIDE_DIAGONAL)) >0 ) {
							//cout << "\t\tRENDER_SIDE_DIAGONAL" << endl;
									p2p(&rg.points[2],&wpoints[1]);
									rg.point_count=3;
									rg.is_surface=false;
								} else {
									p2p(&rg.points[2],&wpoints[3]);
									rg.point_count=3;
									rg.is_surface=false;
	
								}
							} else { // unfilled
						//cout << "\t\tUNFILLED" << endl;
								rg.point_count=2;
								rg.is_surface=false;
								if( (render_flags&RENDER_SIDE_INLINE)>0) {
									rg=cull_rendergon(rg);
									if(rg.point_count>0) { rgv.push_back(rg); }
							//cout << "\t\tRENDER_SIDE_INLINE" << endl;
									p2p(&rg.points[0],&wpoints[3]);
									p2p(&rg.points[1],&wpoints[1]);
								    rg.point_count=2;
								}
								if( (render_flags&(RENDER_SIDE_INNER)) >0 ) {
							//cout << "\t\tRENDER_SIDE_DIAGONAL|RENDER_SIDE_INNER" << endl;
									rg=cull_rendergon(rg);
									if(rg.point_count>0) rgv.push_back(rg);
									p2p(&rg.points[0],&wpoints[1]);
									p2p(&rg.points[1],&wpoints[2]);
									rg.point_count=2;
									rg.is_surface=false;
								} 
								if(( render_flags&(RENDER_SIDE_DIAGONAL)) >0 ) {
							//cout << "\t\tRENDER_SIDE_DIAGONAL" << endl;
									rg=cull_rendergon(rg);
									if(rg.point_count>0) rgv.push_back(rg);
									p2p(&rg.points[0],&wpoints[0]);
									p2p(&rg.points[1],&wpoints[3]);
									rg.point_count=2;
									rg.is_surface=false;
								}
								if(( render_flags&RENDER_SIDE_RADIAL)>0) {
									rg=cull_rendergon(rg);
									if(rg.point_count>0) rgv.push_back(rg);
									p2p(&rg.points[0],&wpoints[0]);
									p2p(&rg.points[1],&wpoints[1]);
									rg.point_count=2;
									rg=cull_rendergon(rg);
									if(rg.point_count>0) rgv.push_back(rg);
									p2p(&rg.points[0],&wpoints[2]);
									p2p(&rg.points[1],&wpoints[3]);
									rg.point_count=2;
									rg.is_surface=false;
								}
								//rg.point_count=0;
							}
						} else if((render_flags&RENDER_SIDE_RADIAL)>0) {
					//cout << "\t\tRENDER_SIDE_RADIAL" << endl;
							p2p(&rg.points[0],&wpoints[0]);
							p2p(&rg.points[1],&wpoints[1]);
							rg.point_count=2;
							rg.is_surface=false;
						} else {
					//cout << "\t\tComposing a single zero Point!" << endl;
							p2p(&rg.points[0],&wpoints[0]);
							rg.point_count=1;
							rg.is_surface=false;
						}
						// cull any points which point to a previous point in the set, or if all of the points in the rendergon are beyond visible range
						rg=cull_rendergon(rg);
						if(rg.point_count>0) {
							rgv.push_back(rg); // Add the Rendergon if it has more than one point
						}
					} // end while loop through individual points
				}
			} // Main For Loop through composed points
			// Copy our newly worked out rendergons ready to transfer to the renderer
			rendergons=rgv;
		//cout << "Compositor readied " << rendergons.size() << " Rendergons." << endl;
		} // End of no_renderer else block
		access.unlock();
		///cout << "Compositor lock released..." << endl;
		rend->set_rendergons(&rendergons);
		///cout << "Compositor sent " << rendergons.size() << " Rendergons to renderer." << endl;
	}
}

Rendergon Compositor::cull_rendergon(Rendergon rg) {
    bool insideof=false;
   	for(int i=0;i<rg.point_count;i++) {
       	if(rg.points[i].x>=0 && rg.points[i].x<rend->width && rg.points[i].y>=0 && rg.points[i].y<rend->height) insideof=true;
           	if(i<rg.point_count-1 && rg.points[i].x==rg.points[i+1].x&&rg.points[i].y==rg.points[i+1].y) {
               	for(int j=i+1; j<=rg.point_count; j++) { // Shuffle all points forward
                   	rg.points[j-1]=rg.points[j];
               	}
           	rg.point_count--;
           	i--;
       	}
   	}
    // cull any rendergons which are entirely outside of the viewing area
   	if(!insideof) rg.point_count=0;
	return rg;
}
