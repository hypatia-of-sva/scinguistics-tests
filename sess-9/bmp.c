#include "common.h"

/* saves data as BITMAPV4HEADER ARGB32 BMP file*/
void write_bitmap_data(char* file_name, uint8_t* red_data, uint8_t* green_data, uint8_t* blue_data, uint8_t* alpha_data, size_t len, int width, int height, float DPI) {
    assert(len == width*height*sizeof(uint8_t));

	FILE* fp;

	fp = fopen(file_name, "w");

    /* BMP header */

	fputc('B', fp);
	fputc('M', fp);

	size_t writing_size = 122 + len*4;
	fputc((writing_size&0xFF), fp);
	fputc((writing_size&0xFF00)>>8, fp);
	fputc((writing_size&0xFF0000)>>16, fp);
	fputc((writing_size&0xFF000000)>>24, fp);

	fputc(0, fp);
	fputc(0, fp);

	fputc(0, fp);
	fputc(0, fp);

    fputc(122, fp);
	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);


    /* DIB header */

    fputc(108, fp);
	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);

	fputc((width&0xFF), fp);
	fputc((width&0xFF00)>>8, fp);
	fputc((width&0xFF0000)>>16, fp);
	fputc((width&0xFF000000)>>24, fp);

	fputc((height&0xFF), fp);
	fputc((height&0xFF00)>>8, fp);
	fputc((height&0xFF0000)>>16, fp);
	fputc((height&0xFF000000)>>24, fp);

	fputc(1, fp);
	fputc(0, fp);

	fputc(32, fp);
	fputc(0, fp);

    fputc(3, fp);
	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);

	size_t data_size = len*4;
	fputc((data_size&0xFF), fp);
	fputc((data_size&0xFF00)>>8, fp);
	fputc((data_size&0xFF0000)>>16, fp);
	fputc((data_size&0xFF000000)>>24, fp);

    int px_per_meter = floor(DPI*39.3701);
	fputc((px_per_meter&0xFF), fp);
	fputc((px_per_meter&0xFF00)>>8, fp);
	fputc((px_per_meter&0xFF0000)>>16, fp);
	fputc((px_per_meter&0xFF000000)>>24, fp);
	fputc((px_per_meter&0xFF), fp);
	fputc((px_per_meter&0xFF00)>>8, fp);
	fputc((px_per_meter&0xFF0000)>>16, fp);
	fputc((px_per_meter&0xFF000000)>>24, fp);

    fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);

    fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);


    fputc(0, fp);
	fputc(0, fp);
	fputc(255, fp);
	fputc(0, fp);

    fputc(0, fp);
	fputc(255, fp);
	fputc(0, fp);
	fputc(0, fp);

    fputc(255, fp);
	fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);

    fputc(0, fp);
	fputc(0, fp);
	fputc(0, fp);
	fputc(255, fp);

	fputc(' ', fp);
	fputc('n', fp);
	fputc('i', fp);
	fputc('W', fp);

    for(int i = 0; i < 12; i++) {
        fputc(0, fp);
        fputc(0, fp);
        fputc(0, fp);
        fputc(0, fp);
    }


    /* pixel array */

    for(int i = 0; i < len; i++) {
        fputc((blue_data[i]), fp);
        fputc((green_data[i]), fp);
        fputc((red_data[i]), fp);
        fputc((alpha_data[i]), fp);
    }

	fclose(fp);
}
