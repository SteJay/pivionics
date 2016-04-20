#include "sjsock.h"
#include <iostream>
class MyServer: public sjs::IP_TcpServer {
	public:
		std::string response;
		void got_message(sjs::IP_TcpServerClient* cli,sjs::IP_Message* msg,void* dummy) {
			response=msg->get_string();
			static_cast<std::string*>(dummy)->append(response);
		}
};


int main(int argc, char* argv[]) {
	MyServer srv;
	int s=srv.start();
	std::string sstr="";
	if(s>=0) {
	  std::cout << "Started...\n";
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
	  std::cout << "Not started, " << s << "\n";
	}
	return 0;
}
