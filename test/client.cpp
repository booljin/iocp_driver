#include <iostream>
#include "iocp_driver.h"
#include <iostream>
#include <winsock2.h>
#include <chrono>
#include <thread>

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(1,1);
    err = WSAStartup(wVersionRequested,&wsaData);

	SOCKET _fd;

	IOCP_DRIVER::iocp_driver client;
	int proto_id = client.regist_protocol(
		std::bind(
			[&](SOCKET fd, IN_ADDR addr, int port) {
				std::cout << "fd: " << fd << " addr: " << addr.S_un.S_addr << " port: " << port << " connect success" << std::endl;
				_fd = fd;
				return 0;
			},
			std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
				),
		std::bind(
			[&](SOCKET fd, unsigned char* buff, int cur_len) {
				int ret = cur_len;
				if (ret > 0) {
					std::string a((char*)buff, ret);
					std::cout << "from server: " << a << std::endl;
					return ret;
				}
				return -1;
			},
			std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3
				),
		std::bind(
			[&](SOCKET fd) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				std::cout << "try to connect" << std::endl;
				client.connect_to("127.0.0.1", 8089, proto_id);
				return 0;
			},
			std::placeholders::_1
				)
				);


	
	client.connect_to("127.0.0.1", 8089, proto_id);
	client.run();
	Sleep(100);

	while(1) {
		std::string a;
		std::getline(std::cin, a);
		if (a == "exit") {
			break;
		}
		client.send_data(_fd, (unsigned char*)a.c_str(), a.size());
	}



    WSACleanup();
    return 0;
}