#include "connection.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <err.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define QUEUE_LISTEN 10

Connection::Connection(string server_ip, int server_port)
	: server_ip(server_ip), server_port(server_port), socket_descriptor(0), client_ip("")
{
}

Connection::~Connection()
{
	if (socket_descriptor)
		close(socket_descriptor);
}

int
Connection::do_connect(struct sockaddr_in* server_addr)
{
	int descriptor = socket(AF_INET, SOCK_STREAM, 0);

	if (descriptor == -1)
		errx(-1, "Fail in socket function!");

	bzero((char *) server_addr, sizeof(*server_addr));

	server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(server_port);
    server_addr->sin_addr.s_addr = inet_addr(server_ip.c_str());

    return descriptor;
}

void
Connection::server_connection()
{
	struct sockaddr_in server_addr;
	socket_descriptor = do_connect(&server_addr);

	if (bind(socket_descriptor, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) < 0)
		errx(-1, "Fail in bind function!");

	if (listen(socket_descriptor, QUEUE_LISTEN) < 0)
		errx(-1, "Fail in listen function!");
}

void
Connection::accept_connections()
{
	struct sockaddr_in client;
	socklen_t client_len;

	while(true)
	{
		int client_id = accept(socket_descriptor, (struct sockaddr *) &client, &client_len);

		if (client_id < 0)
			continue;
		else
		{
			client_ip = inet_ntoa(client.sin_addr);
			printf("Connection success to client %s\n", client_ip.c_str());

			receive_image(client_id);
		}

		close(client_id);
	}
}

void
Connection::receive_image(int id)
{
	int size;
	if (recv(id, &size, sizeof(size), 0) <= 0)
		printf("Fail in receive message size from client %s\n", client_ip.c_str());

	char* message = (char*) malloc(size);

	if (recv(id, message, size, 0) <= 0)
		printf("Fail in receive message from client %s\n", client_ip.c_str());
	else
	{
		printf("Message received with %d bytes\n", size);
		send_confirmation(id);

		FILE *out_file = fopen("out.y", "a+");
		if (not out_file)
			errx(-1, "Error in open out file!");

		fwrite(message, sizeof(char), size, out_file);
		fclose(out_file);
	}
	
	free(message);
}

void
Connection::send_confirmation(int id)
{
	unsigned char confirmation = 0x01;

	if (send(id, &confirmation, sizeof(confirmation), 0) < 0)
		printf("Error in send confirmation!\n");
}
