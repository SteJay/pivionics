/*
sdlrenderer.h - 
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
sdlrenderer.h - 
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

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "core_elements.h"

class SdlRenderer: public Renderer {
	private:
		SDL_Window* sdl_window;
		SDL_Surface* sdl_screen;
		SDL_Renderer* sdl_renderer;
	public:
		SdlRenderer();
		int init(void);
		int shutdown(void);
        void draw_point(unsigned int*, const IntPoint*); // called by render_frame
        void draw_line(unsigned int*,const IntPoint*, const IntPoint*); // Called by render_frame
        void draw_triangle(unsigned int*, const IntPoint*, const IntPoint*, const IntPoint*); // Called by render_frame
        void draw_quad(unsigned int*, const IntPoint*, const IntPoint*, const IntPoint*, const IntPoint*); // called by render_frame
        void draw_surface(int,double,const IntPoint*,const IntPoint*);
		void flip(void);
		void clear(void);
		int allocate_surface(void*);
		void deallocate_surface(int);
};
