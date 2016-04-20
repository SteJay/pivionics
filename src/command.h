/*
command.h - 
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

#include "core_elements.h"

class CommandInstance {
private:
    Element* current;
    std::string out;
protected:
    void output(std::string);
    void output(long double);
    void output(double);
    void output(int);
    void output(unsigned int);
    void output(Element*);
    void ops(void);
    std::string get_output(void);
public:
    CommandInstance();
    void command(Window*,std::string);
    std::string treepos (void);
    
    
};