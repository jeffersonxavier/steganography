#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <err.h>

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

  	fscanf(key_file, "%s", key);
  	fclose(key_file);

  	return key;
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

void read_image()
{
	FILE* image_file = fopen("test.y", "r");

	int file_size = get_file_size(image_file);
  	char *line_readed = (char *) malloc(sizeof(char) * (file_size + 1));
  	fpos_t file_position;
  	long position = 0;

  	while (position < file_size)
  	{
  		vector<string> lines(3);
  		bool valid = true;
  		
  		for (int i = 0; i < 3; ++i)
  		{
  			fgets(line_readed, file_size, image_file);
  			lines[i] = line_readed;
  			lines[i].pop_back();

  			fgetpos(image_file, &file_position);
  			position = file_position.__pos;

  			if (position >= file_size and i < 2)
  			{
  				valid = false;
  				break;
  			}
  		}

		if (valid)
	  		cout << generate_blocks(lines).size() << endl;
  	}

  	fclose(image_file);
}

int main(int argc, char const *argv[])
{
	if (argc < 2)
		errx(-1, "Invalid number of params!");

	string key = load_key();
	int key_position = 0, file_position = 0;

	int bits = atoi(argv[1]);

	read_image();

	return 0;
}
