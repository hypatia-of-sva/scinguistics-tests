#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <errno.h>

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
float duration(waveform_t form);


/* fft.c */
/* returns a pointer to be freed with free() (i.e. gives ownership)
   the array is made up of complex numbers as in pairs, with real first and then imaginary
   this means that the full frequency part is the sum of squares of both */
float* fft_power_of_two_real(float* data, size_t len, bool reverse);
float* fft_power_of_two(float* data, size_t len);
float* ifft_power_of_two(float* data, size_t len);
void fft_power_of_two_inplace(float* data, float* dest, size_t len, bool reverse);


/* util.c */
size_t filesize(FILE* fp);
void die(const char* str);
bool is_power_of_2(uint32_t x);
uint32_t truncate_power_of_2(uint32_t x);
float clamp(float val, float min, float max);
float *transform_to_complex_array(const int16_t* old_array, size_t length);
float* compute_complex_absolute_values(const float* old_array, size_t length);
void maximum_abs_value_and_position_int_array(const int16_t* array, size_t length, int* max_array_index, int16_t* max_array_value);
void maximum_abs_value_and_position_float_array(const float* array, size_t length, int* max_array_index, float* max_array_value);
float *normalize_int_array(int16_t* old_array, size_t length);
float *normalize_float_array(float* old_array, size_t length);
int16_t *transform_complex_to_int_array(const float* old_array, size_t length);
Vector2* create_graph_from_float_array (float* array, size_t length, float base_x, float base_y, float max_x, float max_y);

