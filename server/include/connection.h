#pragma once

#include <iostream>

using std::string;

class Connection
{
public:
	Connection(string server_ip, int server_port);
	~Connection();

	void server_connection();
	void accept_connections();
	void receive_messages(int client_id);
	
	void send_message(int id, int size, string message);

private:
	string server_ip;
	int server_port;
	int socket_descriptor;
	string client_ip;
	float temperature;
	bool status_air;

	int do_connect(struct sockaddr_in* server_addr);
};