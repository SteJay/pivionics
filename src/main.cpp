/*
main.cpp - Main Program Source -- Currently used for testing
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

#include <SDL2/SDL2_gfxPrimitives.h>
#include "core_elements.h"
#include "circle.h"

using namespace std;

int main(int argc, char* argv[]) {
	Window w;
	w.register_creator("Circle",&fn_create_circle);
	w.command("add Circle { set width 400 set height 400 set thickness 100 set sections 180  }",&w);
	Element* p = &w;
	p->construct();

	// Here we go with the SDL and the segmentation faults and the headaches...

	SDL_Window* sdlwindow=NULL;
	SDL_Surface* screensurf=NULL;
	SDL_Renderer* renderer=NULL;
	if( SDL_Init( SDL_INIT_VIDEO < 0 )) {
		cout << "Failure initialising SDL: " << SDL_GetError() << endl;
	} else {
		sdlwindow=SDL_CreateWindow("Pivionics Display", 0, 0,0,0,SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL );
		if(sdlwindow==NULL) {
			cout << "Failure creating sdlwindow." << SDL_GetError() << endl;
		} else {
			screensurf=SDL_GetWindowSurface(sdlwindow);
			renderer=SDL_CreateRenderer(sdlwindow,-1,SDL_RENDERER_ACCELERATED);
			// Now we have a sdlwindow surface :)

			//SDL_FillRect( screensurf, NULL, SDL_MapRGB( screensurf->format, 0xFF, 0xFF, 0xFF ) );
			bool runme=true;
			unsigned int lasttime=0,currenttime,fcount;
		    while (runme) {
		        SDL_Event e;
		        if (SDL_PollEvent(&e)) {
		            if (e.type == SDL_QUIT) {
						runme=false;
		                break;
		            } else {
						switch( e.type ) {
							case SDL_KEYDOWN:
								switch(e.key.keysym.sym) {
									case SDLK_ESCAPE:
										runme=false;
										break;
								}
								break;
						}
					}

		        }
				SDL_SetRenderDrawColor(renderer,0,0,0,255);
				SDL_RenderClear(renderer);
				filledTrigonColor( renderer,10,10,500,100,500,500,0xFF0000FF);
				SDL_RenderPresent(renderer);
				currenttime=SDL_GetTicks();
				fcount++;
				if( currenttime>lasttime+1000) {
					cout << fcount << "Frames in " << currenttime-lasttime << "usec, so " << fcount/((currenttime-lasttime)/1000) << " fps." << endl;
					lasttime=currenttime;
					fcount=0;
				}
			}
		}
	}
	SDL_DestroyWindow(sdlwindow);
	SDL_Quit();
	return 0;
};
