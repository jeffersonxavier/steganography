#pragma once

#include <iostream>

using std::string;

class Connection
{
public:
	Connection(string server_ip, int server_port);
	~Connection();

	void client_connection();
	
	void send_message(int id, int size, string message);
	string receive_message(int client_id);

private:
	string server_ip;
	int server_port;
	int socket_descriptor;

	int do_connect(struct sockaddr_in* server_addr);
};