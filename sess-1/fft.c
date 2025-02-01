#include "common.h"

//#define M_PI acos(-1.0)

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
		if (reverse) {
			c = cos(-2*M_PI*i/len)/len;
			printf("c = %f\n", c);
			s = sin(-2*M_PI*i/len)/len;
			printf("s = %f\n", s);
		} else {
			c = cos(2*M_PI*i/len);
			s = sin(2*M_PI*i/len);
		}
		fft_array[2*i] = even_fft[2*i] + c*odd_fft[2*i] - s*odd_fft[2*i+1];
		fft_array[2*i+1] = even_fft[2*i+1] + s*odd_fft[2*i] + c*odd_fft[2*i+1];
		fft_array[2*i+iteration_size] = even_fft[2*i] - c*odd_fft[2*i] + s*odd_fft[2*i+1];
		fft_array[2*i+1+iteration_size] = even_fft[2*i+1] - s*odd_fft[2*i] - c*odd_fft[2*i+1];
	}
	free(even_fft); free(odd_fft);
	return fft_array;
}
