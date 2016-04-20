/*
pivrend.cpp - 
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

// Include the staples...
#include <unistd.h>
#include <iostream>
#include <fstream>
// Include the entire library of elements and control structures...
#include "stringsplit.h"
#include "core_elements.h"
#include "box.h"
#include "circle.h"
#include "command.h"
#include "irregular.h"
#include "sdlrenderer.h"
#include "text.h"
// Aaaand let's include this
#include "sjsock.h"

class Server:public sjs::IP_TcpServer {
    public:
        std::string response;
        void got_message(sjs::IP_TcpServerClient* cli, sjs::IP_Message* msg, void* str) {
            response=msg->get_string();
            static_cast<std::string*>(str)->append(response);
        }
};



int main(int argc,char* argv[] ) {
    int err=0;
    std::string sstr="";
    CommandInstance kommand;
    SdlRenderer rendur;
    Compositor kompozit;
    rendur.render_run();
    if((err=kommand.init(&rendur,&kompozit))>=0) {
        kommand.run();
        // Set up the TCPness...
        Server srv;
        
        int sret=srv.start();
        srv.register_responder(&sstr);
        if(sret>=0) {
            while(rendur.ready()&&srv.response[0]!='q') {
                srv.receive();
                if(sstr.size()>0) {
                    kommand.do_input(sstr);
                    sstr.clear();
                }
                
            }
        } else {
            std::cerr << "Starting server returned " << sret << "\n";
        }

        // This is where we do stuff.
    } else { 
        std::cerr << err << std::endl;
        return err;
    }
    
    sleep(3);
    return 0;
}