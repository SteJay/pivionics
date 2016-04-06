/*
sdlrenderer.cpp - 
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
#include "sdlrenderer.h"

int SdlRenderer::init(void) {
	access.lock();
	if( SDL_Init(SDL_INIT_VIDEO)<0) {
		cout << "Failure initialising SDL: " << SDL_GetError() << endl;
		access.unlock();
		return ERR_RENDERER_CANNOT_INIT;
	}
	sdl_window = SDL_CreateWindow("Pivionics Compositor", 0,0,720,720,/*SDL_WINDOW_FULLSCREEN|*/SDL_WINDOW_OPENGL );
	if( sdl_window==NULL) {
		cout << "Failure creating SDL window: " << SDL_GetError() << endl;
		access.unlock();
		return ERR_RENDERER_CANNOT_CREATE_WINDOW;
	}
	sdl_screen=SDL_GetWindowSurface(sdl_window);
	if(sdl_screen==NULL) {
		cout << "Failure creating SDL screen surface: " << SDL_GetError() << endl;
		access.unlock();
		return ERR_RENDERER_CANNOT_CREATE_SCREEN;
	}
    sdl_renderer=SDL_CreateRenderer(sdl_window,-1,SDL_RENDERER_ACCELERATED);
	if(sdl_renderer==NULL) {
		cout << "Failure creating SDL renderer: " << SDL_GetError() << endl;
		access.unlock();
		return ERR_RENDERER_CANNOT_CREATE_RENDER;
	}
	access.unlock();
	return 0;
}

int SdlRenderer::shutdown(void) {
	access.lock();
	if(run) render_stop();
	SDL_DestroyRenderer(sdl_renderer);
	SDL_FreeSurface(sdl_screen);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
	access.unlock();
	return 0;
}

void SdlRenderer::flip(void){
	SDL_RenderPresent(sdl_renderer);
	cout << "FLIP!" << endl;
}

void SdlRenderer::draw_point(unsigned int c, const Point* p) {
#ifdef ENABLE_RENDER_AA_POINT
	// NOT IMPLIMENTED YET!
	lineColor(sdl_renderer,p->x,p->y,p->x,p->y,c);
#else
	// NOT IMPLIMENTED YET!
	lineColor(sdl_renderer,p->x,p->y,p->x,p->y,c);
#endif
	cout << "Point Draw Called for " << p->x << "," <<p->y << endl;
}
void SdlRenderer::draw_line(unsigned int c, const Point* p1, const Point* p2) {
#ifdef ENABLE_RENDER_AA_LINE
	aalineColor(sdl_renderer,p1->x,p1->y,p2->x,p2->y,c);
#else
	lineColor(sdl_renderer,p1->x,p1->y,p2->x,p2->y,c);
#endif
	cout << "Line Draw Called for " << p1->x << "," <<p1->y << endl;
}
void SdlRenderer::draw_triangle(unsigned int c, const Point* p1, const Point* p2, const Point* p3) {
	filledTrigonColor(sdl_renderer,p1->x,p1->y,p2->x,p2->y,p3->x,p3->y,c);
#ifdef ENABLE_RENDER_AA_TRIANGLE
	aatrigonColor(sdl_renderer,p1->x,p1->y,p2->x,p2->y,p3->x,p3->y,c);
#endif
	cout << "Triangle Draw Called for " << p1->x << "," <<p1->y << endl;
}
void SdlRenderer::draw_quad(unsigned int c, const Point* p1, const Point* p2, const Point* p3, const Point* p4) {
	filledTrigonColor(sdl_renderer,p1->x,p1->y,p2->x,p2->y,p4->x,p4->y,c);
	filledTrigonColor(sdl_renderer,p3->x,p3->y,p2->x,p2->y,p4->x,p4->y,c);
#ifdef ENABLE_RENDER_AA_QUAD
	// NOT IMPLIMENTED YET!
#endif
	cout << "Quad Draw Called for " << p1->x << "," <<p1->y << endl;
}
void SdlRenderer::draw_surface(void* surf,const Point* p) {
	// NOT IMPLIMENTED YET!
}
