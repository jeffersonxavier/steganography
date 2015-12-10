#include "connection.h"
#include <cstdlib>
#include <err.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	if (argc < 3)
		errx(-1, "Usage: <server_ip> <server_port>");

	string server_ip = argv[1];
	int server_port = atoi(argv[2]);

	Connection connection(server_ip, server_port);
	connection.server_connection();
	connection.accept_connections();

	return 0;
}