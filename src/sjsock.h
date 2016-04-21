/*
sjsock.h - My C++ Socket Library Wrapper 
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
#ifndef SJS_SOCK_H
#define SJS_SOCK_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <list>
#include <queue>
namespace sjs {

class IP_Address {
        
    
};

class IP_Message {
    protected:
        std::queue<char>* raw;
    public:
        IP_Message();
        IP_Message(std::queue<char>*);
        ~IP_Message();
        virtual void set_string(std::string);
        virtual std::string get_string(void);
        virtual char get_char(void);
        virtual int get_int(void);
        virtual unsigned int get_uint(void);
        virtual long get_long(void);
        virtual unsigned long get_ulong(void);
        virtual float get_float(void);
        virtual double get_double(void);
        virtual void get_chararr(char*,int);
};


class IP_TcpServerClient {
    private:
        pollfd pfd;
    public:
        std::thread* thread;
        IP_TcpServerClient(int);
        std::mutex access;
        std::queue<IP_Message*> out;
        std::queue<IP_Message*> in;
        void push(IP_Message*);
        IP_Message* pull(void);
        void run(void);
};

class IP_TcpClient {
    private: 
        pollfd pfd;
        struct sockaddr_in cli;
        socklen_t socklen;
        std::mutex access;
        std::thread* mythread;
        void run_thread(void);
        std::queue<IP_Message*> in;
        std::queue<IP_Message*> out;
    public:
        int con(char*,unsigned short);
        void dis(void);
        IP_Message* receive(void);
        void send(IP_Message*);
        virtual void got_message(IP_Message*,void*)=0;
        
};

class IP_TcpServer {
    private: 
        int sockfd;
        struct sockaddr_in srv;
        socklen_t socklen;
        int sockopts;
        std::mutex access;
        std::thread* athread;
        std::list<IP_TcpServerClient*> clients;
        bool accepting;
        void accept_thread(void);
        int pausetime;
        void* responder;
    public:
        IP_TcpServer();
        void register_responder(void* v) { responder=v; }
        int start(void);
        void stop(void);
        void receive(void);
        void send(IP_TcpServerClient*,IP_Message*);
        virtual void got_message(IP_TcpServerClient*,IP_Message*,void*)=0;
        
};


}

#endif
