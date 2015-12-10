#pragma once

#include <cstdio>
#include <err.h>
#include <iostream>
#include <vector>

#define HASH_SIZE 128

using namespace std;

class Steganography
{
public:
	Steganography(int bits_number, int width, int height, int width_final, int height_final);
	~Steganography();

	bool extract_image();
	static int get_file_size(FILE* file);

private:
	int m_bits_number;
	int m_width;
	int m_height;
	int m_width_final;
	int m_height_final;
	int m_line_block;
	int m_bytes;
	int m_key_size;
	int m_key_position;
	string m_key;
	unsigned char m_byte_to_write;

	FILE *m_image_file;
	FILE *m_out_file;

	vector<int> m_bits;
	vector<int> m_hash_bits;
	vector<char> m_hash_found;

	void load_key();
	void set_position(int column_block, int byte_position);
	char transform_to_byte(vector<int> bits);
	void write_byte(FILE *file, vector<int> bits);
	void get_bits(char byte);
	void get_hash_bits(char byte);
	void get_byte(int column_block, string type);
	vector<char> generate_md5();
	bool compare_hashs();
};