#include "connection.h"
#include <arpa/inet.h>
#include <err.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define ERROR_SOCKET -2
#define ERROR_CONNECT -3
#define ERROR_RECV -4
#define ERROR_SEND -5

using std::cerr;
using std::endl;

//Init server ip, server port and socket descriptor
Connection::Connection(string server_ip, int server_port)
	: server_ip(server_ip), server_port(server_port), socket_descriptor(0)
{
}

//Close socket
Connection::~Connection()
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
		errx(ERROR_SOCKET, "Fail in socket function!");

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
		errx(ERROR_CONNECT, "Fail in connect function!");

	printf("Connection success\n");
}

//Send server message
void
Connection::send_message(int id, int size, string message)
{
	if (send(id, &size, sizeof(size), 0) < 0)
		errx(ERROR_SEND, "Fail in send function!");

	if (send(id, message.c_str(), size, 0) < 0)
		errx(ERROR_SEND, "Fail in send function!");
}

//Receive server message and return this message
string
Connection::receive_message(int id)
{
	int size;
	if (recv(id, &size, sizeof(size), 0) <= 0)
		errx(ERROR_RECV, "Fail in first recv function!");

	char* message = (char*) malloc(size);

	if (recv(id, message, size, 0) <= 0)
		errx(ERROR_RECV, "Fail in second recv function!");

	string result = message;
	free(message);

	return result;
}
