#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "raylib.h"

/* wav.c */

typedef struct {
	int samples_per_second;
	int16_t* amplitude_data;
	size_t data_length;
} waveform_t;
/*
	Reads a 16 bit PCM Wave file bytewise; kills the program if it doesn't work (gory!)
	For documentation see the specs and overview at
	https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
*/
waveform_t read_amplitude_data(char* file_name, int chosen_channel);

void write_amplitude_data(char* file_name, waveform_t data);


/* fft.c */
/* returns a pointer to be freed with free() (i.e. gives ownership)
   the array is made up of complex numbers as in pairs, with real first and then imaginary
   this means that the full frequency part is the sum of squares of both */
float* fft_power_of_two_real(float* data, size_t len, bool reverse);
float* fft_power_of_two(float* data, size_t len);
float* ifft_power_of_two(float* data, size_t len);


/* util.c */
size_t filesize(FILE* fp);
void die(char* str);
bool is_power_of_2(uint32_t x);
uint32_t truncate_power_of_2(uint32_t x);
float clamp(float val, float min, float max);
