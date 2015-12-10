#include "connection.h"
#include "steganography.h"
#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SIZE_TO_READ 4096

//Init server ip, server port and socket descriptor
Connection::Connection(string server_ip, int server_port)
	: server_ip(server_ip), server_port(server_port), socket_descriptor(0)
{
}

//Close socket
Connection::~Connection()
{
	close_connection();
}

void
Connection::send_image()
{
	FILE *out_file = fopen("out.y", "r");
	if (not out_file)
		errx(-1, "Error in open out file");

	char buffer[SIZE_TO_READ];
	int bytes = 0;

	while ((bytes = fread(buffer, sizeof(char), SIZE_TO_READ, out_file)))
	{
		client_connection();

		send_message(socket_descriptor, bytes, buffer);
		receive_confirmation();

		close_connection();
	}
}

void
Connection::close_connection()
{
	if (socket_descriptor)
		close(socket_descriptor);
}

//Open socket with tcp
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

//Connect to server
void
Connection::client_connection()
{
	struct sockaddr_in server_addr;
	socket_descriptor = do_connect(&server_addr);

    if (connect(socket_descriptor,(struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
		errx(-1, "Fail in connect function!");
}

//Send server message
void
Connection::send_message(int id, int size, char* message)
{
	if (send(id, &size, sizeof(size), 0) < 0)
		errx(-1, "Fail in send function!");

	if (send(id, message, size, 0) < 0)
		errx(-1, "Fail in send function!");
}

//Receive server message and return this message
bool
Connection::receive_confirmation()
{
	unsigned char confirmation;
	if (recv(socket_descriptor, &confirmation, sizeof(confirmation), 0) <= 0)
	{
		printf("Fail in first recv function!\n");
		return false;
	}

	if (confirmation == 0x01)
		return true;

	return false;
}
