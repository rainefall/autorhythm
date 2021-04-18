#include "id3v2_image_extract.h"

char* autorhythm_id3v2_image_extract(const char* file_path)
{
	FILE f;
	// open file for reading
	fopen_s(&f, file_path, "r");

	// id3v2 headers are 10 bytes long, unless they're not
	char id3v2_header[10];

	//fread(id3v2_header, 10, 1, )
}
