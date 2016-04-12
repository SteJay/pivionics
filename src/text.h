/*
text.h - 
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
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "core_elements.h"

Element* fn_create_text(void);

class Text: public Element {
	public:
		Text();
		void compose(Origin);
		void construct(void);
		
};
