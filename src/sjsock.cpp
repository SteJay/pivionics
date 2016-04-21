/*
sjsock.cpp - 
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

#include "sjsock.h"
#include <cstring>
#include <iostream>
namespace sjs {

    IP_Message::IP_Message() {
        raw=new std::queue<char>;
    }
    IP_Message::IP_Message(std::queue<char>* q) {
        raw=q;
    }
    IP_Message::~IP_Message(){ delete raw; }
    void IP_Message::set_string(std::string str) {
        for(int i=0;i<str.size();++i) {
            raw->push(str[i]);
        }
    }
    std::string IP_Message::get_string(void){
        std::string s="";
        while(raw->size()>0) {
            char c[2]={raw->front()};
            c[1]='\0';
            s.append(c);
            raw->pop();
        }
        return s;
    }
    
    char IP_Message::get_char(void){
        char c='\0';
        if(raw->size()>0) {
            c=raw->front();
            raw->pop();
        }
        return c;
    }
    
    int IP_Message::get_int(void) {
        int i=0;
        char buf[8]="\0\0\0\0\0\0\0";
        for(int j=0;j<sizeof(int);++j) {
            buf[j] = raw->front();
            raw->pop();
        }
        i = *static_cast<int*>(static_cast<void*>(&buf));
        return i;
    }
    
    IP_TcpServerClient::IP_TcpServerClient (int i) {
        pfd.fd=i;
        pfd.events=POLLIN|POLLOUT|POLLERR;
    }
    
    
    void IP_TcpServerClient::run(void) {
        int runme=true;
        int pollval;
        char buf[2];
        std::queue<char> instr;
        while(runme){
            pollval=poll(&pfd,1,0);
            access.lock();
            if(pfd.revents&POLLIN) {
                while(read(pfd.fd,buf,1)>0&&buf[0]!='\0'&&buf[0]!='\n') {
                    buf[1]='\0';
                    instr.push(buf[0]);
                }
                if(instr.size()>0) {
                    in.push( new IP_Message(&instr) );
                }
            }
            if(pfd.revents&POLLOUT) {
                if(out.size()>0) {
                    IP_Message* msg=out.front();
                    out.pop();
                    std::string msgs = msg->get_string();
                    delete msg;
                    for(int i=0;i<msgs.size();++i) {
                        buf[0]=msgs[i];
                        buf[1]='\0';
                        write(pfd.fd,&buf,1);
                    }
                }
            }
            if(pfd.revents&POLLERR) {
                access.unlock();
                return;
            }
            access.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    void IP_TcpClient::run_thread(void) {
        int runme=true;
        int pollval;
        char buf[2];
        std::queue<char> instr;
        while(runme){
            pollval=poll(&pfd,1,0);
            access.lock();
            if(pfd.revents&POLLIN) {
                while(read(pfd.fd,buf,1)>0&&buf[0]!='\0'&&buf[0]!='\n') {
                    buf[1]='\0';
                    instr.push(buf[0]);
                }
                if(instr.size()>0) {
                    in.push( new IP_Message(&instr) );
                }
            }
            if(pfd.revents&POLLOUT) {
                if(out.size()>0) {
                    IP_Message* msg=out.front();
                    out.pop();
                    std::string msgs = msg->get_string();
                    delete msg;
                    for(int i=0;i<msgs.size();++i) {
                        buf[0]=msgs[i];
                        buf[1]='\0';
                        write(pfd.fd,&buf,1);
                    }
                }
            }
            if(pfd.revents&POLLERR) {
                access.unlock();
                return;
            }
            access.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        
    }
    void IP_TcpServerClient::push(IP_Message* msg) {
        access.lock();
        out.push(msg);
        access.unlock();
    }
    IP_Message* IP_TcpServerClient::pull(void) {
        access.lock();
        IP_Message* t=NULL;
        if(in.size()>0) {
            t=in.front();
            in.pop();
        }
        return t;
        access.unlock();
    }
    IP_TcpServer::IP_TcpServer() {
        sockfd=-1;
        //memset(&srv,0,sizeof(srv));
        sockopts=1;
        socklen=0;
        accepting=false;
        pausetime=100;
        
    }
    int IP_TcpServer::start(void) {
        access.lock();
        sockfd=socket(PF_INET,SOCK_STREAM,0);
        if(sockfd>=0) {
            setsockopt(sockfd,SOL_SOCKET,0,&sockopts, sizeof(sockopts));
            memset(&srv,0,sizeof(srv));
            srv.sin_family=AF_INET;
            srv.sin_port=htons(6789);
            socklen=sizeof(srv);
            // Bind the socket...
            if( (bind(sockfd, (struct sockaddr*)&srv, socklen)) < 0) {
                return -1;
            }
            if( (listen(sockfd,5))<0) {
                return -2;
            }
            accepting=true;
            athread = new std::thread(&IP_TcpServer::accept_thread,this);
            access.unlock();
            return 0;
        }
        access.unlock();
        return -3;
    }
    void IP_TcpServer::stop(void) {
        access.lock();
        accepting=false;
        access.unlock();
    }
    void IP_TcpServer::accept_thread(void) {
        bool nowacc=true;
        while(nowacc) {
            access.lock(); nowacc=accepting; access.unlock();
            if(nowacc) {
                int t=accept4(sockfd, (struct sockaddr*)&srv,&socklen,SOCK_NONBLOCK);
                if(t>=0) {
                    access.lock();
                    IP_TcpServerClient* cli=new IP_TcpServerClient(t);
                    clients.push_back(cli);
                    cli->thread = new std::thread(&IP_TcpServerClient::run,cli);
                    access.unlock();
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(pausetime));
        }
    }
    

    void IP_TcpServer::receive(void) {
        for(auto it=clients.begin();it!=clients.end();++it) {
            IP_TcpServerClient* cli=*it;
            if(cli->access.try_lock()) {
                while(cli->in.size()>0) {
                    this->got_message(cli,cli->in.front(),responder);
                    cli->in.pop();
                }
                cli->access.unlock();
            }
        }
        
    }
    
    int IP_TcpClient::con(char* ipaddr,unsigned short port) {
        pfd.fd=socket(PF_INET,SOCK_STREAM,0);
        if(pfd.fd<0) return -3;
        pfd.events=POLLIN|POLLOUT|POLLERR;
        memset(&cli,0,sizeof(cli));
        cli.sin_family=AF_INET;
        cli.sin_port=port;
        if(!(inet_aton(ipaddr,&cli.sin_addr))) return -1;
        socklen=sizeof(cli);
        if(connect(pfd.fd,(struct sockaddr *)&cli,socklen)) return -2;
        mythread=new std::thread(&IP_TcpClient::run_thread,this);
        return 0;
    }
    
    IP_Message* IP_TcpClient::receive(void) {
        IP_Message* msg=NULL;
        access.lock();
        if(in.size()>0) {
            msg=in.front();
            in.pop();
        }
        access.unlock();
        return msg;
    }
    void IP_TcpClient::send(IP_Message* msg) {
        access.lock();
        out.push(msg);
        access.unlock();
    }
}