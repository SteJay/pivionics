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

struct ServerResponder {
    std::string response;
    sjs::IP_TcpServerClient* cli;
};

class Server:public sjs::IP_TcpServer {
    public:
        ServerResponder resp;
        void got_message(sjs::IP_TcpServerClient* cli, sjs::IP_Message* msg, void* q) {
            resp.response=msg->get_string();
            resp.cli=cli;
            static_cast<std::queue<ServerResponder>*>(q)->push(resp);
        }
};



int main(int argc,char* argv[] ) {
    int err=0;
    std::queue<ServerResponder> rsp;
    CommandInstance kommand;
    SdlRenderer rendur;
    Compositor kompozit;
    rendur.render_run();
    if((err=kommand.init(&rendur,&kompozit))>=0) {
        kommand.run();
        // Set up the TCPness...
        Server srv;
        
        int sret=srv.start();
        srv.register_responder(&rsp);
        if(sret>=0) {
            while(rendur.ready()) {
                srv.receive();
                while(rsp.size()>0) {
                    ServerResponder r=rsp.front();
                    rsp.pop();
                    if(r.response.size()>0) {
                        if(r.response[0]=='q'&&r.response[1]=='u'&&r.response[2]=='i'&&r.response[3]=='t') {
                            rendur.render_stop();
                            break;
                        } else {
                            std::string rs=kommand.do_input(r.response);
                            rs.append("\n\0");
                            sjs::IP_Message* msg=new sjs::IP_Message;
                            msg->set_string(rs);
                            std::cerr << rs << "\n";
                            r.cli->push(msg);
                        }
                    }
                }
                kommand.run();
                
            }
        } else {
            std::cerr << "Server returned " << sret << " - Is the address/port available?\n";
        }

        // This is where we do stuff.
    } else { 
        std::cerr << err << std::endl;
        return err;
    }
    
    return 0;
}