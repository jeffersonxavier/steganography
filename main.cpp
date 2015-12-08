#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <err.h>
#include <string.h>
#include <limits.h>

using namespace std;

#define HASH_SIZE 128

int bits_number = 0, line_block = 0, key_position = 0, key_size = 0, bytes = 0;
int width = 0, height = 0, width_final = 0, height_final = 0;
string key;
vector<int> bits(CHAR_BIT, 0);
vector<int> hash_bits;
FILE *image_file, *out_file, *hash_file;

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

void set_position(int column_block, int byte_position)
{
	int column = (byte_position % 3 ? byte_position / 3 : byte_position / 3 - 1);
	int line = (byte_position - 1) % 3;

	line += line_block * 3;
	column += column_block * 3;

	int position = width * line + column;
	fseek(image_file, position, SEEK_SET);
}

char transform_to_byte(vector<int> bits_found)
{
	unsigned char byte = 0x00;

	for (int i = 7, j = 0; i >= 0; --i, ++j)
    {
    	if (bits_found[j] == 1)
    	{
	    	byte = (byte >> i) | 0x01;
	    	byte <<= i;
    	}
    }

    return byte;
}

char write_byte(FILE *file, vector<int> bits_found)
{
	char result_byte = transform_to_byte(bits_found);
	fprintf(file, "%c", result_byte);
	bytes++;	
}

void get_bits(char byte)
{
	for (int i = 0; i < bits_number; ++i)
		bits[i] = (byte >> i) & 0x01;

	write_byte(out_file, bits);
}

void get_hash_bits(char byte)
{
	for (int i = 0; i < bits_number; ++i)
	{
		hash_bits.push_back((byte >> i) & 0x01);

		if (hash_bits.size() == CHAR_BIT)
		{
			write_byte(hash_file, hash_bits);
			hash_bits.clear();
		}
	}
}

void get_byte(int column_block, string type)
{
	int byte_position = key[key_position] - '0';
	key_position = (key_position + 1) % key_size;

	if (not byte_position)
		return;

	set_position(column_block, byte_position);
	
	char byte = fgetc(image_file);
	if (type == "image")
		get_bits(byte);
	else
	{
		get_hash_bits(byte);
	}
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
	out_file = fopen("out.y", "w");
	hash_file = fopen("hash.txt", "w+");

	if (not image_file or not out_file or not hash_file)
		errx(-1, "Fail in open files!");

	//get steganography image
	int hash_size = HASH_SIZE / 8;
	bool hash_mode = false;
	while (bytes < (width_final * height_final) + hash_size)
	{
		for (int i = 0; i < width / 3; ++i)
		{
			if (hash_mode)
				get_byte(i, "hash");
			else
				get_byte(i, "image");

			if (bytes == (width_final * height_final))
				hash_mode = true;

			if (bytes == (width_final * height_final) + hash_size)
				break;
		}

		line_block++;
	}

	fclose(image_file);
	fclose(out_file);

	return 0;
}
