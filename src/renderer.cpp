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

#include <thread>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "core_elements.h"

Renderer::~Renderer() {
	if(run) {
		// Stop the renderer to ensure its thread is quit and freed...
		render_stop();
	}
}
bool Renderer::ready(void) {
	if( access.try_lock() ) {
		// Lock obtained...
		access.unlock();// ...but we don't need it.
		return true;
	}
	// No lock was obtained
	return false;
}
unsigned int Renderer::get_fps(void) { return fps; }
bool Renderer::render_frame(void) {

}
bool Renderer::render_loop(void) {
	unsigned int fpstime=SDL_GetTicks();
	unsigned int framecount;
	while(run) {
		framecount++;
		render_frame();
		if( SDL_GetTicks()-fpstime>1000 ) {
			fps=framecount;
			fpstime=SDL_GetTicks();
			framecount=0;
		}
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
