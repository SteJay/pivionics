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

SdlRenderer::SdlRenderer() {
	sdl_screen=NULL;
	sdl_renderer=NULL;
	sdl_window=NULL;
}

int SdlRenderer::init(void) {
	access.lock();
	if( SDL_Init(SDL_INIT_VIDEO)<0) {
	//cout << "Failure initialising SDL: " << SDL_GetError() << endl;
		access.unlock();
		return ERR_RENDERER_CANNOT_INIT;
	}
	sdl_window = SDL_CreateWindow("Pivionics Renderer", 0,0,width,height,/*SDL_WINDOW_FULLSCREEN|*/SDL_WINDOW_OPENGL/*|SDL_WINDOW_BORDERLESS*/ );
	if( sdl_window==NULL) {
	//cout << "Failure creating SDL window: " << SDL_GetError() << endl;
		access.unlock();
		return ERR_RENDERER_CANNOT_CREATE_WINDOW;
	}
	sdl_screen=SDL_GetWindowSurface(sdl_window);
	if(sdl_screen==NULL) {
	//cout << "Failure creating SDL screen surface: " << SDL_GetError() << endl;
		access.unlock();
		return ERR_RENDERER_CANNOT_CREATE_SCREEN;
	}
    sdl_renderer=SDL_CreateRenderer(sdl_window,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
	if(sdl_renderer==NULL) {
	//cout << "Failure creating SDL renderer: " << SDL_GetError() << endl;
		access.unlock();
		return ERR_RENDERER_CANNOT_CREATE_RENDER;
	}
	if(TTF_Init()==-1) {
		access.unlock();
		return ERR_RENDERER_CANNOT_INIT_TTF;
	}
//	test_surface = IMG_Load("logo.png");
//	test_texture = SDL_CreateTextureFromSurface(sdl_renderer,test_surface);
	access.unlock();
	return 0;
}

int SdlRenderer::shutdown(void) {
	access.lock();
	if(run) render_stop();
	//SDL_FreeSurface(sdl_screen);
	TTF_Quit();
	SDL_DestroyRenderer(sdl_renderer);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
	access.unlock();
	return 0;
}

void SdlRenderer::clear(void) {
	SDL_SetRenderDrawColor(sdl_renderer,0,0,0,255);
	SDL_RenderClear(sdl_renderer);
}

void SdlRenderer::flip(void){
	SDL_RenderPresent(sdl_renderer);
}

void SdlRenderer::draw_point(unsigned int *c, const IntPoint* p) {
#ifdef ENABLE_RENDER_AA_POINT
	aalineColor(sdl_renderer,p->x,p->y,p->x,p->y,*c);
#else
	lineColor(sdl_renderer,p->x,p->y,p->x,p->y,*c);
#endif
}
void SdlRenderer::draw_line(unsigned int *c, const IntPoint* p1, const IntPoint* p2) {
#ifdef ENABLE_RENDER_AA_LINE
	aalineColor(sdl_renderer,p1->x,p1->y,p2->x,p2->y,*c);
#else
	lineColor(sdl_renderer,p1->x,p1->y,p2->x,p2->y,*c);
#endif
}
void SdlRenderer::draw_triangle(unsigned int *c, const IntPoint* p1, const IntPoint* p2, const IntPoint* p3) {
	filledTrigonColor(sdl_renderer,p1->x,p1->y,p2->x,p2->y,p3->x,p3->y,*c);
#ifdef ENABLE_RENDER_AA_TRIANGLE
	aatrigonColor(sdl_renderer,p1->x,p1->y,p2->x,p2->y,p3->x,p3->y,c);
#endif
}
void SdlRenderer::draw_quad(unsigned int *c, const IntPoint* p1, const IntPoint* p2, const IntPoint* p3, const IntPoint* p4) {
//	filledTrigonColor(sdl_renderer,p1->x,p1->y,p2->x,p2->y,p3->x,p3->y,*c);
//	filledTrigonColor(sdl_renderer,p1->x,p1->y,p3->x,p3->y,p4->x,p4->y,*c);
	short vx[4]={p1->x,p2->x,p3->x,p4->x};
	short vy[4]={p1->y,p2->y,p3->y,p4->y};
	filledPolygonColor(sdl_renderer,vx,vy,4,*c);
#ifdef ENABLE_RENDER_AA_QUAD
#endif
}

void SdlRenderer::draw_surface(int surfid,double angle,const IntPoint* p, const IntPoint* ps) {
	if(surfid < surfaces.size() && surfid>=0) {
		SDL_Texture* s = static_cast<SDL_Texture*>(surfaces[surfid]);
		SDL_Rect dst;
		SDL_Point pnt;
		pnt.x = ps->x/2;
		pnt.y = ps->y/2;
		dst.x=p->x;dst.y=p->y;dst.w=ps->x;dst.h=ps->y;
		SDL_RenderCopyEx(sdl_renderer,static_cast<SDL_Texture*>(surfaces[surfid]),NULL,&dst,angle,&pnt,SDL_FLIP_NONE);
	}
}

int SdlRenderer::allocate_surface(void* surf) {
	access.lock();
	SDL_Texture* tex = SDL_CreateTextureFromSurface(sdl_renderer,static_cast<SDL_Surface*>(surf));
	auto iter = surfaces.begin();
	for(;iter!=surfaces.end()&&static_cast<void*>(*iter)!=NULL&&static_cast<void*>(*iter)!=surf;++iter);
	int i = 0;
	if(iter!=surfaces.end()) {
		cout << "Found surface to be over written";
		if(*iter!=NULL) {
			SDL_DestroyTexture(static_cast<SDL_Texture*>(*iter));
			cout << " which has now been deleted.";
		} else {
			cout << " which was NULL";
		}
		cout << endl;
		*iter=tex;
		i = iter-surfaces.begin();
	} else {
		cout << "No free surfaces" << endl;
		i=surfaces.size();
		surfaces.push_back(tex);
	}
	access.unlock();
	return i;
}

void SdlRenderer::deallocate_surface(int surfid) {
	SDL_DestroyTexture(static_cast<SDL_Texture*>(surfaces[surfid]));
	surfaces[surfid]=NULL;
}
