#pragma once

#include <iostream>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3000

using std::string;

class Connection
{
public:
	Connection(string server_ip, int server_port);
	~Connection();

	void client_connection();
	void send_image();

private:
	string server_ip;
	int server_port;
	int socket_descriptor;

	void close_connection();
	int do_connect(struct sockaddr_in* server_addr);
	void send_message(int id, int size, char* message);
	bool receive_confirmation();
};