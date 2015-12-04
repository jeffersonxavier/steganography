#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <err.h>
#include <string.h>

#define BYTE 1

int bits = 0, width = 0, height = 0;

using namespace std;

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

	int file_size = get_file_size(key_file);
  	char *key = (char *) malloc(sizeof(char) * (file_size + 1));

  	fgets(key, file_size, key_file);
  	string result = key;

  	free(key);
  	fclose(key_file);

  	return result;
}

vector<vector<string>> generate_blocks(vector<string> lines)
{
	int position = 0;

	vector<vector<string>> blocks;
	
	while (lines[0].substr(position, 3).size() == 3 and
		lines[1].substr(position, 3).size() == 3 and
		lines[2].substr(position, 3).size() == 3)
	{
		vector<string> v(3);

		for (int i = 0; i < 3; ++i)
			v[i] = lines[i].substr(position, 3);

		blocks.push_back(v);
		position += 3;
	}

	return blocks;
}

string read_line(FILE* file)
{
	if (feof(file))
		return "";

	char c = fgetc(file);

	string result = "";
	result += c;

	for (int i = 1; i < width; ++i)
	{
		c = fgetc(file);
		if (not feof(file))
			result += c;
		else
			break;

	}
	
	return result;
}

void read_image()
{
	FILE* image_file = fopen("image.y", "r");

	for (int i = 0; i < height; i+=3)
	{
		vector<string> lines(3);
		lines[0] = read_line(image_file);
		lines[1] = read_line(image_file);
		lines[2] = read_line(image_file);
	}

  	fclose(image_file);
}

int main(int argc, char const *argv[])
{
	if (argc < 4)
		errx(-1, "Invalid number of params!");

	string key = load_key();
	int key_position = 0, file_position = 0;

	bits = atoi(argv[1]);
	width = atoi(argv[2]);
	height = atoi(argv[3]);

	read_image();

	return 0;
}
