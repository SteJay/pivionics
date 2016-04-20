/*
console.h - 
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

#include "curses.h"
#include "core_elements.h"
#include "command.h"

#ifndef PIV_CONSOLE_H
#define PIV_CONSOLE_H


#define W_ON_K 0
#define R_ON_K 1
#define G_ON_K 2
#define B_ON_K 3
#define C_ON_K 4
#define M_ON_K 5
#define Y_ON_K 6
#define K_ON_W 7
#define R_ON_W 8
#define G_ON_W 9
#define B_ON_W 10
#define M_ON_W 11
#define W_ON_B 12
#define C_ON_B 13
#define Y_ON_B 14

class PivConsole {
    private:
        std::list<const char*> keywords;
        std::list<const char*> endwords;
        SCREEN* screen;
        bool running;
        bool ok;
        bool is_color;
        int width;
        int height;
        int columns;
        int rows;
        int view;
        int bracelevel;
        std::string status;
        std::string fpsdisp;
        std::string commandline;
        std::list<std::string> hist;
        std::list<std::string> linebuffer;
        std::list<std::string> childbuffer;
        std::list<std::string> siblingbuffer;
        std::list<std::string> propbuffer;
        std::list<std::string> attrbuffer;
        std::string cur_type;
        std::string cur_name;
        
        int cursor;
        int focus;
        
    protected:
        void get_input(void);
        void entered(void);
        int draw_commandline(void); // Colours and draws the commandline and returns the number of lines used
        int draw_status(void); // Draws the status and summary and returns the number of lines used
        
    public:
        void display(void);
        PivConsole();
        ~PivConsole();
        void run(void);
        
};
void main_fn(void);

#endif