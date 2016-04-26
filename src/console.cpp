/*
console.cpp - 
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

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include "core_elements.h"
#include "console.h"
#include "stringsplit.h"

PivConsole::PivConsole(){
    ok=false;
    view=0;
    running=false;
    is_color=false;
    commandline="";
    width=0;
    height=0;
    columns=0;
    rows=0;
    fpsdisp="";
    focus=0;
    cursor=0;
    cur_type="Window";
    cur_name="Default Window";
    hist.clear();
    screen = newterm(NULL,stdout,stdin);
    status="Pivionics Console:";
    linebuffer.push_back("Welcome to the Pivionics Console.");
    linebuffer.push_back(" ");
    linebuffer.push_back("Pivionics is free software: you can redistribute it and/or modify");
    linebuffer.push_back("it under the terms of the GNU General Public License as published by");
    linebuffer.push_back("the Free Software Foundation, either version 3 of the License, or");
    linebuffer.push_back("(at your option) any later version.");
    linebuffer.push_back(" ");
    linebuffer.push_back("Pivionics is distributed in the hope that it will be useful,");
    linebuffer.push_back("but WITHOUT ANY WARRANTY; without even the implied warranty of");
    linebuffer.push_back("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
    linebuffer.push_back("GNU General Public License for more details.");
    linebuffer.push_back(" ");
    linebuffer.push_back("You should have received a copy of the GNU General Public License");
    linebuffer.push_back("along with Pivionics.  If not, see <http://www.gnu.org/licenses/>.");
    if(screen!=NULL&&set_term(screen)!=NULL) {
        if(is_color=has_colors()) {
            start_color();
            // now we set up the colours...
            init_pair(W_ON_K,COLOR_WHITE,COLOR_BLACK);
            init_pair(R_ON_K,COLOR_RED,COLOR_BLACK);
            init_pair(G_ON_K,COLOR_GREEN,COLOR_BLACK);
            init_pair(B_ON_K,COLOR_BLUE,COLOR_BLACK);
            init_pair(C_ON_K,COLOR_CYAN,COLOR_BLACK);
            init_pair(M_ON_K,COLOR_MAGENTA,COLOR_BLACK);
            init_pair(Y_ON_K,COLOR_YELLOW,COLOR_BLACK);
            init_pair(K_ON_W,COLOR_BLACK,COLOR_WHITE);
            init_pair(R_ON_W,COLOR_RED,COLOR_WHITE);
            init_pair(G_ON_W,COLOR_GREEN,COLOR_WHITE);
            init_pair(B_ON_W,COLOR_BLUE,COLOR_WHITE);
            init_pair(M_ON_W,COLOR_MAGENTA,COLOR_WHITE);
            init_pair(W_ON_B,COLOR_WHITE,COLOR_BLUE);
            init_pair(C_ON_B,COLOR_CYAN,COLOR_BLUE);
            init_pair(Y_ON_B,COLOR_YELLOW,COLOR_BLUE);
        }
        ok=true;
        nodelay(stdscr,TRUE);
        noecho();
        cbreak();

        keypad(stdscr,TRUE);
        curs_set(0);
        // We may have resized the window...
        getmaxyx(stdscr,height,width);
    }
}

PivConsole::~PivConsole(){
    if(screen!=NULL) {
        endwin();
        delscreen(screen);
    }
}

int PivConsole::draw_status(int line) {
    // Draw the status bar at the top, yellow on black
    attron(COLOR_PAIR(Y_ON_K)|A_BOLD);
    mvaddstr(line,0,status.c_str());
    attroff(COLOR_PAIR(Y_ON_K)|A_BOLD);
    attron(COLOR_PAIR(B_ON_K));
    for(int i=0;i<width-status.size()-fpsdisp.size();++i) addstr("#");
    attroff(COLOR_PAIR(B_ON_K));
    // Draw the fps in white (if we have it)
    attron(COLOR_PAIR(Y_ON_K));
    if( fpsdisp.size()>0) {
        addstr(fpsdisp.c_str());
    }
    attroff(COLOR_PAIR(Y_ON_K));
        return 1;
}

int PivConsole::draw_current(int line) {
    int topline=line;
    // Draw the current element details:
    attron(COLOR_PAIR(W_ON_B));
    mvaddstr(topline,0,"CURRENT ELEMENT: ");
    attroff(COLOR_PAIR(W_ON_B));
    attron(COLOR_PAIR(C_ON_B)|A_BOLD);
    addstr(cur_type.c_str());
    attroff(COLOR_PAIR(C_ON_B)|A_BOLD);
    attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
    addstr(" (");
    attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
    attron(COLOR_PAIR(C_ON_B)|A_BOLD);
    addstr(cur_name.c_str());
    attroff(COLOR_PAIR(C_ON_B)|A_BOLD);
    attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
    addstr(")");
    for(int i=0;i<width-cur_type.size()-cur_name.size()-20;++i) addstr(" ");
    attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
    // Now draw the current element details
    // Yes, really really long winded. Sorry about that.
    std::string propstr="";std::string valstr=""; int xi=0;
    topline++;
    for(int i=3;i<18;++i) {
        if(i==3) { propstr="cx: ";valstr=cur_cx; }
        else if(i==4) { propstr="cy: ";valstr=cur_cy; }
        else if(i==5) { propstr="width: ";valstr=cur_width; }
        else if(i==6) { propstr="height: ";valstr=cur_height; }
        else if(i==7) { propstr="xscale: ";valstr=cur_xscale; }
        else if(i==8) { propstr="yscale: ";valstr=cur_yscale; }
        else if(i==9) { propstr="angle: ";valstr=cur_angle; }
        else if(i==10) { propstr="arc: ";valstr=cur_arc; }
        else if(i==11) { propstr="thickness: ";valstr=cur_thickness; }
        else if(i==12) { propstr="sections: ";valstr=cur_sections; }
        else if(i==13) { propstr="subsections: ";valstr=cur_subsections; }
        else if(i==14) { propstr="color: ";valstr=cur_color; }
        else if(i==15) { propstr="inherit position: ";valstr=cur_inherit_position; }
        else if(i==16) { propstr="inherit angle: ";valstr=cur_inherit_angle; }
        else if(i==17) { propstr="inherit scale: ";valstr=cur_inherit_scale; }
        valstr+=" ";
        if(xi+propstr.size()+valstr.size()>width) {
            topline++; 
            attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
            while(xi<width) { addstr(" ");xi++;}
            attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
            xi=0;
        }
        attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
        mvaddstr(topline,xi,propstr.c_str());
        attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
        attron(COLOR_PAIR(C_ON_B)|A_BOLD);
        addstr(valstr.c_str());
        attroff(COLOR_PAIR(C_ON_B)|A_BOLD);
        
        xi+=propstr.size()+valstr.size();
    }
    attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
    while(xi<width) { addstr(" ");xi++;}
    attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
    topline ++;
    return topline;
    
}

int PivConsole::draw_attrs(int line) {
    int topline=line; int xi=0;
    // We want to draw the smallest size first...
    std::list<std::string> v;
    for(auto iter=attrbuffer.begin(); iter!=attrbuffer.end(); ++iter) {
        v.push_back(iter->first);
    }
    int sizenow;
    std::string attrnow="";
    std::string attrthen="";
    while(v.size()>0) {
        sizenow=4096;
        attrthen="";attrnow="";
        for(auto it=v.begin();it!=v.end();++it) {
            attrthen=*it;
            if(attrthen.size()+attrbuffer[attrthen].size()<sizenow) {
                attrnow=*it;
                sizenow=attrnow.size()+attrbuffer[attrnow].size();
            }
        }
        // Now we know the smallest index, lets remove it from our list
        v.remove(attrnow);
        // Now we draw it.
        if((sizenow+xi+6)<width) {
            // There's space right here...
            attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
            mvaddstr(topline,xi,attrnow.c_str());
            addstr(" = \"");
            attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
            attron(COLOR_PAIR(Y_ON_B)|A_BOLD);
            addstr(attrbuffer[attrnow].c_str());
            attroff(COLOR_PAIR(Y_ON_B)|A_BOLD);
            attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
            addstr("\" ");
            attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
            xi+=sizenow+6;
        } else {
            // Too big to fit here. Is it too big for the width?
            if(sizenow+6>=width) {
                // Too big for screen, we'll have to split the line
                if(attrnow.size()+7+xi>=width) {
                    attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
                    while(xi<width) {
                        addstr(" ");
                        ++xi;
                    }
                    attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
                    topline++; xi=0;
                }
                attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
                mvaddstr(topline,xi,attrnow.c_str());
                addstr(" = \"");
                attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
                std::string mystr=attrbuffer[attrnow];
                attron(COLOR_PAIR(Y_ON_B)|A_BOLD);
                while( mystr.size()>0 ) {
                    addch(mystr.front());
                    xi++;
                    mystr.erase(0,1);
                    if(xi>=width) {
                        xi=0;
                        topline++;
                    }
                    
                }
                attroff(COLOR_PAIR(Y_ON_B)|A_BOLD);
                attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
                addstr("\" ");
                attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
            } else {
                // It'll fit on the next line.
                attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
                while(xi<width) {
                    addstr(" ");
                    ++xi;
                }
                xi=0; topline++;
                mvaddstr(topline,xi,attrnow.c_str());
                addstr(" = \"");
                attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
                attron(COLOR_PAIR(Y_ON_B)|A_BOLD);
                addstr(attrbuffer[attrnow].c_str());
                addstr("\" ");
                attroff(COLOR_PAIR(Y_ON_B)|A_BOLD);
                xi+=sizenow+6;
            }
        }
    
    }
    attron(COLOR_PAIR(W_ON_B)|A_NORMAL);
    if(xi>0) {
        while(xi<width) {
            addch(' ');
            xi++;
        }
        topline++;
    }
    attroff(COLOR_PAIR(W_ON_B)|A_NORMAL);
    return topline-line;
}

void PivConsole::display(void) {
    int lines_left=height;
    int topline=draw_status(0);
    
    // Draw the commandline at the bottom
    int nlines=(commandline.size()+3)/width;
    int rchar=(commandline.size()+3)%width;
    if(rchar>0)++nlines;
    attron(COLOR_PAIR(K_ON_W)|A_NORMAL);
    mvaddstr(height-nlines,0,"@> ");
    attroff(COLOR_PAIR(K_ON_W)|A_NORMAL);
    int spos=0; int x=3;
    attron(COLOR_PAIR(K_ON_W)|A_BOLD);
    for(int y=0;y<nlines;y++) {
        for(;x<width;x++) {
                if(spos==cursor) {
                    attron(COLOR_PAIR(W_ON_B)|A_BOLD);
                } else {
                    attron(COLOR_PAIR(B_ON_W)|A_BOLD);
                }
            if(spos>=commandline.size()) {
                mvaddch(height-nlines+y,x,' ');
            } else {
                
                mvaddch(height-nlines+y,x,commandline[spos]);
            }
            spos++;
        }
        x=0;
    }
    attroff(COLOR_PAIR(K_ON_W)|A_BOLD);
    lines_left-=nlines;
    topline = draw_current(topline);
    lines_left-=topline;
    int ttopline = draw_attrs(topline);
    lines_left-=ttopline;
    topline+=ttopline;
    // Finally draw the line buffer - start at the bottom and work up to fill remaining space
    std::string thisline="";
    for(auto it=linebuffer.rbegin();it!=linebuffer.rend();++it) {
        // Calculate the space needed to show this line...
        thisline=*it;
        
        nlines=thisline.size()/width;
        rchar=thisline.size()%width;
        if(rchar>0)nlines++;
        if(nlines<1)nlines=1; // For empty lines
        lines_left -= nlines;
        // Can we fit it in?
        if( lines_left>0 ) {
            // If so, we draw this line
            spos=0;
            // Set the colours...
            if(thisline[0]=='@') {
                attron(COLOR_PAIR(Y_ON_K)|A_BOLD);
            } else {
                attron(COLOR_PAIR(W_ON_K)|A_NORMAL);
            }
            for(int y=0;y<nlines;y++) {
                for(x=0;x<width;x++) {
                    if(spos>=thisline.size()) {
                        mvaddch(topline+lines_left+y,x,' ');
                    } else {
                        mvaddch(topline+lines_left+y,x,thisline[spos]);
                    }
                    spos++;
                }
            }
            if(thisline[0]=='@') {
                attroff(COLOR_PAIR(Y_ON_K)|A_BOLD);
            } else {
                attroff(COLOR_PAIR(W_ON_K)|A_NORMAL);
            }
        }
    }
    attron(COLOR_PAIR(W_ON_K)|A_BOLD);
    while(lines_left>=0) {
        for(x=0;x<width;x++) {
            mvaddch(topline+lines_left,x,' ');
        }
        lines_left--;
    }
    attroff(COLOR_PAIR(W_ON_K)|A_BOLD);
    refresh();
    
}

void PivConsole::entered(void) { 
    // Push the commandline to the back of the history...
    hist.push_back(commandline);
    // Remove the oldest history line if the history has grown too big...
    if( hist.size() > 25 ) hist.pop_front();
    // "echo" the commandline to our line buffer...
    linebuffer.push_back("@> " + commandline);
    
    if(commandline.compare("quit")==0) { running=false; }
    // TODO: Process the commandline
    sjs::IP_Message* msg=new sjs::IP_Message;
    msg->set_string(commandline);
    tcp.send(msg);
    sjs::IP_Message* msg2=new sjs::IP_Message;
    msg2->set_string("ex fps ");
    tcp.send(msg2);
    // Clear the commandline:
    commandline="";
    cursor=0;
    
}

void PivConsole::get_input(void) {
    // Before we process terminal input, lets see what the network has had to say
    while( inputbuffer.size()>0) {
        std::list<std::string> args=get_arguments(inputbuffer.front());
        std::string thisline="";
        while(args.size()>0) {
            std::string s=args.front();
            args.pop_front();
            if(s.compare("ELEMENT:")==0) {
                attrbuffer.clear();
                int i=0; while( i<18 && args.size()>0 ) {
                    if(i==1) { cur_type = args.front(); }
                    else if(i==2) { cur_name = args.front(); }
                    else if(i==3) { cur_cx = args.front(); }
                    else if(i==4) { cur_cy = args.front(); }
                    else if(i==5) { cur_width = args.front(); }
                    else if(i==6) { cur_height = args.front(); }
                    else if(i==7) { cur_xscale = args.front(); }
                    else if(i==8) { cur_yscale = args.front(); }
                    else if(i==9) { cur_angle = args.front(); }
                    else if(i==10) { cur_arc = args.front(); }
                    else if(i==11) { cur_thickness = args.front(); }
                    else if(i==12) { cur_sections = args.front(); }
                    else if(i==13) { cur_subsections = args.front(); }
                    else if(i==14) { cur_color = args.front(); }
                    else if(i==15) { cur_inherit_position = args.front(); }
                    else if(i==16) { cur_inherit_angle = args.front(); }
                    else if(i==17) { cur_inherit_scale = args.front(); }
                    args.pop_front();
                    ++i;
                }
            } else if(s.compare("ATTR:")==0) {
                std::string a,b;
                if(args.size()>=3) {
                    a=args.front(); args.pop_front();
                    args.pop_front();
                    b=args.front(); args.pop_front();
                    attrbuffer[a]=b;
                }
            } else {
                if(s.size()>0&&s.compare(" ")!=0) {
                    thisline.append(s+" ");
                }
            }
            if(thisline.size()>0) linebuffer.push_back(thisline);
            
        }
        inputbuffer.pop();
//        linebuffer.push_back(inputbuffer.front());
//        inputbuffer.pop();
    }
    int chin = getch();
    if(chin!=ERR) {
        if(chin>=' '&&chin<='~'){ 
            commandline.push_back(static_cast<char>(chin));
            ++cursor;
        } else {
            switch(chin) {
                case KEY_ENTER:
                case '\r':
                case '\n':
                    entered();
                    break;
                case 127:
                    if(cursor<=commandline.size()) commandline.erase(cursor,1);
                    break;
                case KEY_BACKSPACE:
                    if(cursor>0) {
                        if(cursor<=commandline.size()) {
                            cursor--;
                            commandline.erase(cursor,1);
                        }
                    }
                    break;
                case KEY_F0:
                    // TODO : Switch Windows
                    break;
                case KEY_LEFT:
                    if(cursor>0) cursor--;
                    break;
                case KEY_RIGHT:
                    if(cursor<commandline.size()) cursor++;
                    break;
                case KEY_UP:
                    break;
                case KEY_DOWN:
                    break;
                case '\t':
                    break;
                case KEY_NPAGE:
                    break;
                case KEY_PPAGE:
                    break;
                case KEY_RESIZE:
                    // We may have resized the window...
                    getmaxyx(stdscr,height,width);
                    break;
            }
        }
    }
    
}

void PivConsole::run(void) {
    tcp.register_responder(&inputbuffer);
    if((tcp.start("127.0.0.1",6890))<0) { return; }
    
    running=true;
    timespec sleep_time;
    timespec rem_time;
    sleep_time.tv_sec=0;
    sleep_time.tv_nsec=10000000;
    while(running) {
        tcp.receive();
        get_input();
        display();
        // do something else...
        nanosleep(&sleep_time,&rem_time);
    }
}
void main_fn (void) {
    PivConsole* console=new PivConsole;
    console->display();
    console->run();
    delete console;
}

void PivClient::got_message(sjs::IP_Message* msg,void* vpoint) {
    std::queue<std::string>* q=static_cast<std::queue<std::string>*>(vpoint);
    q->push(msg->get_string());
    delete msg;
}
