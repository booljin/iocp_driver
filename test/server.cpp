#include "iocp_driver.h"
#include <iostream>
#include <winsock2.h>
#include <string>


int main() {
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);

	IOCP_DRIVER::iocp_driver server;

	server.regist_monitor(nullptr);

	int proto_id = server.regist_protocol(
		std::bind(
			[&](SOCKET fd, IN_ADDR addr, int port) {
				std::cout << "fd: " << fd << " addr: " << addr.S_un.S_addr << " port: " << port << " connected" << std::endl;
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
					if (std::string((char*)buff, cur_len) == "killme") {
						server.close_node(fd);
					} else {
						server.send_data(fd, buff, ret);
					}return ret;
				}
				return -1;
			},
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3
			),
		std::bind(
			[&](SOCKET fd) {
				std::cout << "fd: " << fd << " disconnected" << std::endl;
			},
			std::placeholders::_1
			)
		);
	server.listen_from("0.0.0.0", 8089, proto_id);

	server.run();


	getchar();
	
	WSACleanup();
	return 0;
}

