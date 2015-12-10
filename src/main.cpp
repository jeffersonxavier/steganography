#include "steganography.h"

int main(int argc, char const *argv[])
{
	if (argc < 6)
		errx(-1, "Usage: <bits> <width> <height> <width_final> <height_final>");

	Steganography steganography(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	printf("%d\n", steganography.extract_image());

	return 0;
}
