#include "steganography.h"
#include <cstdlib>
#include <limits.h>
#include <openssl/md5.h>
#include <string.h>

using namespace std;

Steganography::Steganography(int bits_number, int width, int height, int width_final, int height_final)
	: m_bits_number(bits_number), m_width(width), m_height(height),
	m_width_final(width_final), m_height_final(height_final),
	m_line_block(0), m_bytes(0), m_key_size(0), m_key_position(0), m_key(""),
	m_byte_to_write('0'), m_image_file(nullptr), m_out_file(nullptr)
{
	m_image_file = fopen("image.y", "r+b");
	m_out_file = fopen("out.y", "w+");

	if (not m_image_file or not m_out_file)
		errx(-1, "Fail in open files!");

	load_key();

	m_byte_to_write = 0xFF;
	m_byte_to_write <<= m_bits_number;

	for (int i = 0; i < CHAR_BIT; ++i)
		m_bits.push_back(0);
}

Steganography::~Steganography()
{
	fclose(m_image_file);
	fclose(m_out_file);
}

bool
Steganography::extract_image()
{
	int hash_size = HASH_SIZE / 8;
	bool hash_mode = false;
	
	fprintf(m_out_file, "P5 %d %d 255 ", m_width_final, m_height_final);
	
	while (m_bytes < (m_width_final * m_height_final) + hash_size)
	{
		for (int i = 0; i < m_width / 3; ++i)
		{
			if (hash_mode)
				get_byte(i, "hash");
			else
				get_byte(i, "image");

			if (m_bytes == (m_width_final * m_height_final))
				hash_mode = true;

			if (m_bytes == (m_width_final * m_height_final) + hash_size)
				break;
		}

		m_line_block++;
	}

	return compare_hashs();
}

int
Steganography::get_file_size(FILE* file)
{
	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
  	rewind (file);

  	return file_size;
}

void
Steganography::load_key()
{
	FILE *key_file = fopen("key.txt", "r");

	if (not key_file)
		errx(-1, "Fail in open key file!");

	m_key_size = get_file_size(key_file);
  	char *key = (char *) malloc(sizeof(char) * (m_key_size + 1));

  	fread(key, sizeof(char), m_key_size, key_file);
  	m_key = key;

  	free(key);
  	fclose(key_file);
}

void
Steganography::set_position(int column_block, int byte_position)
{
	int column = (byte_position % 3 ? byte_position / 3 : byte_position / 3 - 1);
	int line = (byte_position - 1) % 3;

	line += m_line_block * 3;
	column += column_block * 3;

	int position = m_width * line + column;
	fseek(m_image_file, position, SEEK_SET);
}

char
Steganography::transform_to_byte(vector<int> bits)
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

void
Steganography::write_byte(FILE *file, vector<int> bits)
{
	char result_byte = transform_to_byte(bits);
	fprintf(file, "%c", result_byte);
	m_bytes++;
}

void
Steganography::get_bits(char byte)
{
	for (int i = 0; i < m_bits_number; ++i)
		m_bits[i] = (byte >> i) & 0x01;

	write_byte(m_out_file, m_bits);
}

void
Steganography::get_hash_bits(char byte)
{
	for (int i = 0; i < m_bits_number; ++i)
	{
		m_hash_bits.push_back((byte >> i) & 0x01);

		if (m_hash_bits.size() == CHAR_BIT)
		{
			char byte = transform_to_byte(m_hash_bits);
			m_hash_found.push_back(byte);
			m_bytes++;
			m_hash_bits.clear();
		}
	}
}

void
Steganography::get_byte(int column_block, string type)
{
	int byte_position = m_key[m_key_position] - '0';
	m_key_position = (m_key_position + 1) % m_key_size;

	if (not byte_position)
		return;

	set_position(column_block, byte_position);
	
	char byte = fgetc(m_image_file);
	if (type == "image")
		get_bits(byte);
	else
	{
		get_hash_bits(byte);

		int position = ftell(m_image_file);
		char new_byte = byte & m_byte_to_write;

		fseek(m_image_file, position-1, SEEK_SET);
		fwrite(&new_byte, sizeof(char), sizeof(char), m_image_file);
		fseek(m_image_file, position, SEEK_SET);
	}
}

vector<char>
Steganography::generate_md5()
{
	FILE *image = fopen ("image.y", "rb");

	if (not image)
		errx(-1, "Fail in open image!");

	MD5_CTX md5_context;
    MD5_Init(&md5_context);

    char c = fgetc(image);
    while (c != EOF)
    {
        MD5_Update(&md5_context, &c, sizeof(c));
        c = fgetc(image);
    }

    char hash_generated[MD5_DIGEST_LENGTH];
    MD5_Final((unsigned char*) hash_generated, &md5_context);
    fclose(image);

    vector<char> result;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
    	result.push_back(hash_generated[i]);

    return result;
}

bool
Steganography::compare_hashs()
{
	if (m_hash_found == generate_md5())
		return true;

	return false;
}