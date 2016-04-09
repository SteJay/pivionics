/*
renderer.cpp - 
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
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "core_elements.h"

#ifdef ENABLE_RENDER_AA_ALL
#define ENABLE_RENDER_AA_POINT
#define ENABLE_RENDER_AA_LINE
#define ENABLE_RENDER_AA_TRIANGLE
#define ENABLE_RENDER_AA_QUAD
#define ENABLE_RENDER_AA_SURFACE
#endif


Renderer::~Renderer() {
	if(run) {
		// Stop the renderer to ensure its thread is quit and freed...
		render_stop();
	}
}
int Renderer::init(void) {

}
int Renderer::shutdown(void) {

}
bool Renderer::ready(void) {
	if( access.try_lock() ) {
		// Lock obtained...
		access.unlock();// ...but we don't need it.
		if(run) {
			return true;
		} else {
			return false;
		}
	}
	// No lock was obtained
	return false;
}
unsigned int Renderer::get_fps(void) { 
	access.lock();
	unsigned int f = fps;
	access.unlock();
	return f; 
}
bool Renderer::render_frame(void) {
	access.lock();
	clear();
	Rendergon thisgon;
	bool renderedall=true;
//cout << "Rendering frame, " << points.size() << " Rendergons to process." << endl;
	int i=0;
	for(auto iter=points.begin();iter!=points.end(); ++iter) {
		i++;
		// Iterate through points
		thisgon=*iter;
	//cout << "Rendergon " << i << " has " << thisgon.point_count << " points." << endl; 
		if(thisgon.is_surface) {
			// TODO: Surface rendering
		} else {
			switch(thisgon.point_count) {
				case 1:
					draw_point(&thisgon.color,&thisgon.points[0]);
					break;
				case 2:
					draw_line(&thisgon.color,&thisgon.points[0],&thisgon.points[1]);
					break;
				case 3:
					draw_triangle(&thisgon.color,&thisgon.points[0], &thisgon.points[1], &thisgon.points[2]);
					break;
				case 4:
					draw_quad(&thisgon.color,&thisgon.points[0], &thisgon.points[1], &thisgon.points[2], &thisgon.points[3]);
					break;
				default:
					renderedall=false;
				//cout << "Rendergon fail: <1 or >4 points in rendergon." << endl;
			}
		}
	}
//cout << "Rendergons drawn." << endl;
	access.unlock();
	return renderedall;
}
bool Renderer::render_loop(void) {
	int rr=0;
	if(rr=init() >=0) {
		unsigned int fpstime=SDL_GetTicks();
		unsigned int framecount;
		bool renderedall=true;
		while(run) {
			framecount++;
			renderedall=render_frame();
			flip();
			if( SDL_GetTicks()-fpstime>1000 ) {
				access.lock();
				fps=framecount;
				access.unlock();
				fpstime=SDL_GetTicks();
				framecount=0;
			}
				
		}
		return renderedall;
	} else {
		return false;
	}
}

void Renderer::render_run(void) {
	run=true;
	runthread = new thread(&Renderer::render_loop,this);
}

void Renderer::render_stop(void) {
	run=false;
	runthread->join(); // Wait for the thread to quit
}

bool Renderer::set_rendergons(const vector<Rendergon>* rgv) {
	access.lock();
	points = *rgv;
	access.unlock();
	return true;
}


void Renderer::flip(void) { }
void Renderer::clear(void) { }
void Renderer::draw_point(unsigned int *c, const IntPoint* p) {
#ifdef ENABLE_RENDER_AA_POINT
#else
#endif
}
void Renderer::draw_line(unsigned int *c, const IntPoint* p1, const IntPoint* p2) {
#ifdef ENABLE_RENDER_AA_LINE
#else
#endif
}
void Renderer::draw_triangle(unsigned int  *c, const IntPoint* p1, const IntPoint* p2, const IntPoint* p3) {
#ifdef ENABLE_RENDER_AA_TRIANGLE
#else
#endif
}
void Renderer::draw_quad(unsigned int *c, const IntPoint* p1, const IntPoint* p2, const IntPoint* p3, const IntPoint* p4) {
#ifdef ENABLE_RENDER_AA_QUAD
#else
#endif
}
void Renderer::draw_surface(void* surf,const IntPoint* p) {

}