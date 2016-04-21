#include "sjsock.h"
#include <iostream>

// First we derive a class to represent our server
class MyServer: public sjs::IP_TcpServer {
    public:
        // This string is used to cache the last response; we only define it here to prevent it
        // from being created and destroyed for every message
        std::string response;
        // At the very least we have to provide a got_message function to override the pure
        // virtual in the superclass (sjs::IP_TcpServer)
        void got_message(sjs::IP_TcpServerClient* cli,sjs::IP_Message* msg,void* vpoint) {
            // The idea of overriding this function is to provide a specific implementation for 
            // the message type your application is using; this will be pointed to by vpoint.
            // In this case we just want a string, for which we can simply use the get_string()
            // method of sjs::IP_Message. For more complex extractions there are other getters
            // provided which can be used sequentially.
            response=msg->get_string();
            // Because we're using a void pointer, we also need to cast back to the data type
            // we're using.
            static_cast<std::string*>(vpoint)->append(response);
        }
};

// Now we need to do the same for our client

class MyClient: public sjs::IP_TcpClient {
    public:
        // Very similar to the above, so I'll skip those notes...
        std::string response;
        void got_message(sjs::IP_Message* msg,void* vpoint) {
            // Notice that for sjs::IP_TcpClient there is one less argument; clients implicitly
            // know which connection they need to send to, whereas a server could have a number
            // of choices.
            response=msg->get_string();
            // Again, cast and use the pointer
            static_cast<std::string*>(vpoint)->append(response);
        }
};

// Now our main functions. Rather than having two separate files I figured I'd put all the code
// in one file for reference. The following _main functions could be considered just to be 
// int main (void) 
int server_main(void) {
    
    MyServer srv;
    int s=srv.start("127.0.0.1",6789);
    std::string sstr="";
    if(s>=0) {
        std::cout << "Server Started...\n";
        srv.register_responder(&sstr);
        while(true) {
            srv.receive();
            if(sstr.size()>0) {
                std::cout << "Input: " << sstr << "\n";
                sstr.clear();
            }
            sleep(1);
        }
    } else {
        std::cout << "Server Not started, Error " << s << "\n";
    }
    return 0;
}

int client_main(void) {
    MyClient cli;
    int c=cli.start("127.0.0.1",6789);
    std::string sstr="";
    char chrbuf[255]; // Input buffer
    if(c>=0) {
        std::cout << "Client Started...\n";
        cli.register_responder(&sstr);
        while(true) {
            cli.receive();
            if(sstr.size()>0) {
                std::cout << "Input: " << sstr << "\n";
                sstr.clear();
            } 
            // Now get some text and send it...
            sjs::IP_Message* msg=new sjs::IP_Message;
            std::cin.getline(chrbuf,sizeof(chrbuf));
            sstr=chrbuf;
            msg->set_string(sstr);
            cli.send(msg);
            sleep(1);
        }
    } else {
        std::cout << "Client Not started, Error " << c << "\n";
    }
    return 0;
}

// Now the main function - we won't go crazy...

int main(int argc,char* argv[]) {
    if(argc>1) {
        std::string arg = argv[1];
        if(arg.compare("server")==0) {
            return server_main();
        } else if(arg.compare("client")==0) {
            return client_main();
        } else {
            std::cout << "Usage: " << argv[0] << " <server|client>\nYou must specify whether you want to run the server or client code.\n\n";
        }
    }
    return 0;
}
