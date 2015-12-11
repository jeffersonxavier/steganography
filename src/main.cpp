#include "connection.h"
#include "steganography.h"
#include <signal.h>

Steganography *steganography;

void abort_client(int)
{
	printf("Abort client!\n") ;
	printf("Progress: %.1lf%%\n", steganography->get_progress());

	steganography->abort_client();

	printf("Finishing client...\n");
}

int main(int argc, char const *argv[])
{
	if (argc < 6)
		errx(-1, "Usage: <bits> <width> <height> <width_final> <height_final>");

	signal(SIGINT, abort_client);

	steganography = new Steganography(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	
	if (steganography->extract_image())
	{
		printf("Valid Image!\n");

		Connection connection(SERVER_IP, SERVER_PORT);
		connection.send_image();
	}
	else
		printf("Invalid Image!\n");

	return 0;
}
