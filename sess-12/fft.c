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

/* destroys the data array! */
static void fft_power_of_two_inplace(float* data, float* dest, size_t len, bool reverse) {
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
