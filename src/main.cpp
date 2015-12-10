#include "steganography.h"
#include "connection.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3000

int main(int argc, char const *argv[])
{
	if (argc < 6)
		errx(-1, "Usage: <bits> <width> <height> <width_final> <height_final>");

	Steganography steganography(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	
	if (steganography.extract_image())
	{
		printf("Valid Image!\n");

		Connection connection(SERVER_IP, SERVER_PORT);
		connection.send_image();
	}

	return 0;
}
