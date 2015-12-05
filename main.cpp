#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <err.h>
#include <string.h>
#include <limits.h>

using namespace std;

int bits_number = 0, width = 0, height = 0, line_block = 0, key_position = 0, key_size = 0;
string key;
vector<int> bits;
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

void complet_bits()
{
	int size_to_move = CHAR_BIT - bits.size();

	for (int i = 0; i < bits.size() - CHAR_BIT; ++i)
		bits.push_back(0);

	char byte = transform_to_byte();
	byte >>= size_to_move;
	fprintf(out_file, "%c", byte);
}

void get_bits(char byte)
{
	for (int i = (bits_number - 1), j = 0; i >= 0; --i, ++j)
	{
		int bit = (byte >> i) & 0x01;
		bits.push_back(bit);

		if (bits.size() == CHAR_BIT)
		{
			char byte = transform_to_byte();
			fprintf(out_file, "%c", byte);

			bits.clear();
		}
	}
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
	if (argc < 4)
		errx(-1, "Invalid number of params!");

	key = load_key();

	bits_number = atoi(argv[1]);
	width = atoi(argv[2]);
	height = atoi(argv[3]);

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

	if (not bits.empty())
		complet_bits();

	fclose(image_file);
	fclose(out_file);

	return 0;
}
