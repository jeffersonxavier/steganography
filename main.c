#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <err.h>

using namespace std;

string load_key()
{
	FILE *key_file = fopen("key.txt", "r");

	if (not key_file)
		errx(-1, "Fail in open key file!");

	fseek(key_file, 0, SEEK_END);
	int file_size = ftell(key_file);
  	rewind (key_file);

  	char *key = (char *) malloc(sizeof(char) * (file_size + 1));
  	fscanf(key_file, "%s", key);

  	return key;
}

int main(int argc, char const *argv[])
{
	if (argc < 2)
		errx(-1, "Invalid number of params!");

	string key = load_key();
	int key_position = 0;

	int bits = atoi(argv[1]);
	printf("%d\n", bits);

	return 0;
}
