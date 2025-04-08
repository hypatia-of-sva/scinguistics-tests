#include "common.h"

//#define M_PI acos(-1.0)


uint64_t reverse_bits(uint64_t num, uint64_t len) {
	if(len > 64) die("too large width for reversing!");
	uint64_t result = num&1;
	for(int i = 1; i < len; i++) {
		result <<= 1;
		num >>= 1;
		result |= num&1;
	}
	return result;
}

float* fft_power_of_two(float* data, size_t len) {
	float* copy = malloc(len*sizeof(float));
	memcpy(copy, data, len*sizeof(float));
	float* dest = calloc(len, sizeof(float));
	fft_power_of_two_inplace(copy, dest, len, false);
	return dest;
}
float* ifft_power_of_two(float* data, size_t len) {
	float* copy = malloc(len*sizeof(float));
	memcpy(copy, data, len*sizeof(float));
	float* dest = calloc(len, sizeof(float));
	fft_power_of_two_inplace(copy, dest, len, true);
	return dest;
}

/* old recursive version:
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
*/


/* destroys the data array! */
void fft_power_of_two_inplace(float* data, float* dest, size_t len, bool reverse) {
    if(!is_power_of_2(len) || len == 1) return;
    int N = log2(len);
    for(int l = 0; l < N-1; l++) {
        for(int combined_i = 0; combined_i < len/4; combined_i++) {
			int bucket = ((1<<((N-2)-l))-1) & combined_i;
			int inner_index = (combined_i - bucket) >> ((N-2)-l);
			int even_index = ((combined_i - bucket) << 2) | (bucket << 1);
			int odd_index = ((combined_i - bucket) << 2) | (1 << (N-l-1)) | (bucket << 1);
			int i_value = reverse_bits(inner_index, l);
			float c = cos(2*M_PI*i_value/(1<<(l+1))),
					s = (reverse ? -1.0 : 1.0)*sin(2*M_PI*i_value/(1<<(l+1)));
			float even_real = data[even_index], even_imag = data[even_index+1];
			float odd_real  = data[odd_index],  odd_imag  = data[odd_index+1];
			data[even_index]   = even_real + c*odd_real - s*odd_imag;
			data[even_index+1] = even_imag + s*odd_real + c*odd_imag;
			data[odd_index]    = even_real - c*odd_real + s*odd_imag;
			data[odd_index+1]  = even_imag - s*odd_real - c*odd_imag;
		}
    }
    for (int i = 0; i < len/2; i++) {
        int real_index = reverse_bits(i, N-1);
        dest[2*i] = data[2*real_index] / (reverse ? (float)(1<<(N-1)) : 1.0f);
        dest[2*i+1] = data[2*real_index+1] / (reverse ? (float)(1<<(N-1)) : 1.0f);
    }
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
