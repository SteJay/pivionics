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
#include "core_elements.h"
#include "circle.h"

using namespace std;

int main(int argc, char* argv[]) {
	Window w;
	w.register_creator("Circle",&fn_create_circle);
	w.command("add Circle { set cx 360 set cy 360 set width 200 set height 200 set thickness 50 } add Circle { set width 170 set height 170 set thickness 50 }",&w);
	Element* p = &w;
	p->construct();
	return 0;
};
