#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <err.h>
#include <string.h>

using namespace std;

int bits = 0, width = 0, height = 0, line_block = 0, key_position = 0, key_size = 0;
string key;
FILE* image_file;

int get_file_size(FILE* file)
{
	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
  	rewind (file);

  	return file_size;
}

string load_key()
{
	FILE *key_file = fopen("key.txt", "r");

	if (not key_file)
		errx(-1, "Fail in open key file!");

	key_size = get_file_size(key_file);
  	char *key = (char *) malloc(sizeof(char) * (key_size + 1));

  	fread(key, sizeof(char), key_size, key_file);
  	string result = key;

  	free(key);
  	fclose(key_file);

  	return result;
}

void set_position(int line, int column)
{
	int position = width * line + column;
	fseek(image_file, position, SEEK_SET);
}

void get_byte(int column_block)
{
	int byte_position = key[key_position] - '0';
	key_position = (key_position + 1) % key_size;

	if (not byte_position)
		return;

	int column = (byte_position % 3 ? byte_position / 3 : byte_position / 3 - 1);

	int line;
	if (byte_position % 3 == 0)
		line = 2;
	else if (byte_position % 3 == 1)
		line = 0;
	else
		line = 1;

	line += line_block * 3;
	column += column_block * 3;

	set_position(line, column);
	
	char byte = fgetc(image_file);
	cout << "byte: " << byte << endl;
}

int main(int argc, char const *argv[])
{
	if (argc < 4)
		errx(-1, "Invalid number of params!");

	key = load_key();

	bits = atoi(argv[1]);
	width = atoi(argv[2]);
	height = atoi(argv[3]);

	image_file = fopen("test.y", "r");
	if (not image_file)
		errx(-1, "Fail in open image file!");

	while (line_block < height / 3)
	{
		for (int i = 0; i < width / 3; ++i)
		{
			get_byte(i);			
		}

		line_block++;
	}

	fclose(image_file);

	return 0;
}
