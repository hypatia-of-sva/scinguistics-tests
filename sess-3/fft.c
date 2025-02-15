#include "common.h"

//#define M_PI acos(-1.0)


float* fft_power_of_two(float* data, size_t len) {
	return fft_power_of_two_real(data, len, false);
}
float* ifft_power_of_two(float* data, size_t len) {
	float* rec_result = fft_power_of_two_real(data, len, true);
	for(int i = 0; i < len; i++) {
		rec_result[i] /= len/2;
	}
	return rec_result;
}


/* returns a pointer to be freed with free() (i.e. gives ownership)
   the array is made up of complex numbers as in pairs, with real first and then imaginary
   this means that the full frequency part is the sum of squares of both */
float* fft_power_of_two_real(float* data, size_t len, bool reverse) {
    int i;

	if(!is_power_of_2(len) || len == 1) return NULL;
	float* fft_array = malloc(len*sizeof(float));
	if(len == 2) {
		fft_array[0] = data[0];
		fft_array[1] = data[1];
		return fft_array;
	}
	size_t iteration_size = len/2;
	float* even = malloc(iteration_size * sizeof(float));
	float* odd = malloc(iteration_size * sizeof(float));
	for(i = 0; i < iteration_size/2; i++) {
		even[2*i] = data[4*i];
		even[2*i+1] = data[4*i+1];
		odd[2*i] = data[4*i+2];
		odd[2*i+1] = data[4*i+3];
	}
	float* even_fft = fft_power_of_two_real(even, iteration_size, reverse);
	float* odd_fft = fft_power_of_two_real(odd, iteration_size, reverse);
	free(even); free(odd);

	for(i = 0; i < iteration_size/2; i++) {
		float c, s;

		c = cos(2*M_PI*i/iteration_size);
		s = sin(2*M_PI*i/iteration_size);

		if (reverse) {
			/* changes the phase to -2*M_PI*i/iteration_size, due to cos(-x) = cos(x), sin(-x) = -sin(x)*/
			s = -s;
		}
		fft_array[2*i] = even_fft[2*i] + c*odd_fft[2*i] - s*odd_fft[2*i+1];
		fft_array[2*i+1] = even_fft[2*i+1] + s*odd_fft[2*i] + c*odd_fft[2*i+1];
		fft_array[2*i+iteration_size] = even_fft[2*i] - c*odd_fft[2*i] + s*odd_fft[2*i+1];
		fft_array[2*i+1+iteration_size] = even_fft[2*i+1] - s*odd_fft[2*i] - c*odd_fft[2*i+1];
	}
	free(even_fft); free(odd_fft);
	return fft_array;
}
