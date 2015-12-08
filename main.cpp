#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <err.h>
#include <string.h>
#include <limits.h>

using namespace std;

int bits_number = 0, line_block = 0, key_position = 0, key_size = 0, bytes = 0;
int width = 0, height = 0, width_final = 0, height_final = 0;
string key;
vector<int> bits(CHAR_BIT, 0);
FILE *image_file, *out_file;

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

char transform_to_byte()
{
	unsigned char byte = 0x00;

	for (int i = 7, j = 0; i >= 0; --i, ++j)
    {
    	if (bits[j] == 1)
    	{
	    	byte = (byte >> i) | 0x01;
	    	byte <<= i;
    	}
    }

    return byte;
}

void get_bits(char byte)
{
	if (bytes >= (width_final * height_final))
		return;

	for (int i = 0; i < bits_number; ++i)
		bits[i] = (byte >> i) & 0x01;

	char result_byte = transform_to_byte();
	fprintf(out_file, "%c", result_byte);
	bytes++;
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
	get_bits(byte);
}

int main(int argc, char const *argv[])
{
	if (argc < 6)
		errx(-1, "Usage: <bits> <width> <height> <width_final> <height_final>");

	key = load_key();

	bits_number = atoi(argv[1]);
	width = atoi(argv[2]);
	height = atoi(argv[3]);
	width_final = atoi(argv[4]);
	height_final = atoi(argv[5]);

	image_file = fopen("image.y", "r");
	out_file = fopen("out.y", "w+");

	if (not image_file or not out_file)
		errx(-1, "Fail in open files!");

	while (line_block < height / 3)
	{
		for (int i = 0; i < width / 3; ++i)
		{
			get_byte(i);			
		}

		line_block++;
	}

	fclose(image_file);
	fclose(out_file);

	return 0;
}
