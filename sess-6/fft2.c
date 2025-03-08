
/* destroys src! if needed later, make a copy!*/
void fft_power_of_two_inplace(float* data, float* dest, size_t len, bool reverse) {
    if(!is_power_of_2(len) || len == 1) return;
    int N = log2(len);
    /*  l is the log2 of the size of the small (even or odd), so l = 0 creates the synthesis of two 1-cells, i.e. four floats each
        the biggest size is N-2, because the last bit differentiates the values within a cell, so it can't be N, and we have to synthesize two cells */
    for(int l = 0; l < N-1; l++) {
        /* k is the number of the pair which is to be synthesized; since we used l bits for the size of the cell, and one bit for number and even each, we have l+2 bits
         * already in use, hence we need 2^(N-l-2) pairs
         */
        for(int k = 0; k < (1<<(N-l-2)); k++) {
            /*  now we need to reconstruct the even and odd array to change them in place.
                because we split for even and odd recursively, this will set the lower bits, i.e. k will be the value of the _lower_ N-l-2 bit, except the last bit, which
                is used to store which number value of the cell it is (real or imaginary).
                So we have a pattern:
                        f1 .... fl                                              e                               k1 ... k(N-l-2)     v
                        (free bits - indices within the even and odd arrays)    (selector for even and odd)     (pair number)       (number value)
                And we now need to synthesize, for a given k, all the other values.
                If we look at the recursion formula:
                    fft_array[2*i] = even_fft[2*i] + c*odd_fft[2*i] - s*odd_fft[2*i+1];
                    fft_array[2*i+1] = even_fft[2*i+1] + s*odd_fft[2*i] + c*odd_fft[2*i+1];
                    fft_array[2*i+iteration_size] = even_fft[2*i] - c*odd_fft[2*i] + s*odd_fft[2*i+1];
                    fft_array[2*i+1+iteration_size] = even_fft[2*i+1] - s*odd_fft[2*i] - c*odd_fft[2*i+1];
                where iteration_size here would be 2^l, we see that even_fft[2*i], even_fft[2*i+1] and odd_fft[2*i], odd_fft[2*i+1] create
                fft_array[2*i], fft_array[2*i+1], fft_array[2*i+2^l], fft_array[2*i+1+2^l] for i from 0 to 2^(l-1)-1, i.e. we have synthesis of the patterns
                    i1 ... il X k1 ... k(N-l-2) Y
                into
                    X i1 ... il k1 ... k(N-l-2) Y
                for both X and Y variable bits. If we do this in place, we have the problem of overriding values we need later for other i1 values, but if we vary i1
                we need to be careful to not override i2 values etc. There is however an easy solution to this, to not complicate the algorithm: store the value in place, and
                treat
                    i1 ... il X k1 ... k(N-l-2) Y
                in the next round as _if_ it were
                    X i1 ... il k1 ... k(N-l-2) Y
                This means a constant shifting pattern of how to do the addition, but saves copy operations.
                This means a bit permutation of (1 l+1) in each step (with ascending bit numbers), so that after m (i.e. l = m+1 now) rounds we have to evaluate the permutation
                    (1 1)(1 2)...(1 m)
                which for the first few values is
                    (1 1) = id for round 2 (l = 1)
                    (1 1)(1 2) = (1 2) for round 3 (l = 2)
                    (1 1)(1 2)(1 3) = (1 2 3) for round 4 (l = 3)
                    (1 1)(1 2)(1 3)(1 4) = (1 2 3)(1 4) = (1 2 3 4) for round 5 (l = 4)
                i.e. in general the rotation (1 ... m) for round m+1 (i.e l = m) since the first permutation is lost, as it's the identity;
                i.e. we have all bits in the correct order, except the m-th bit, which is shifted forward to the first bit. I.e., when processing the
                    i1 ... il X k1 ... k(N-l-2) Y
                fields, they are actually stored like
                    il i1 ... i(l-1) X k1 ... k(N-l-2) Y
                and thus the first bit actually saves the l-th bit. Since afterwards we save
                    X i1 ... il k1 ... k(N-l-2) Y
                in that position, it will then hold the (l+1)-th bit, repeating this cycle.
                Because we only operate on different indices within the even and odd array, and operate on them _independently_, we can actually go throught the i-bits
                as if they are sequential; the only issue is with the c and s factors. They are
                    c = cos(2*M_PI*i/2^l);
                    s = sin(2*M_PI*i/2^l);
                so that, if i_eff is the index we go through, we need to add a function: f(i,l): il i1....i(l-1) -> i1 ... il (in bits)
                and with that we can set
                    c = cos(2*M_PI*f(i,l)/2^l);
                    s = sin(2*M_PI*f(i,l)/2^l); (or -s for reverse)
                and can just iterate over the i as if no bit shifts happen.
                Only at the very end we have to do the f-tranform in the indices, to write the result out.
                We can define f via:
                    f(i,l) := (i<<1)|((i&(1>>(l-1)))>>(l-1))
                which we can now insert into the c and s definitions
                With this, we can create this simple loop:
            */
            for (int i = 0; i < (1 << (l-1)); i++) {
                int even_index = (i<<(N-l)) | (k << 1),
                    odd_index = (i<<(N-l)) | (1 << (N-l-1)) | (k << 1);
                int i_value = (i<<1)|((i&(1>>(l-1)))>>(l-1));
                float c = cos(2*M_PI*i_value/2^l),
                      s = (reverse ? -1.0 : 1.0)*sin(2*M_PI*i_value/2^l);
                float even_real = data[even_index], even_imag = data[even_index+1];
                float odd_real = data[odd_index], odd_imag = data[odd_index+1];
                data[even_index] = even_real + c*odd_real - s*odd_imag;
                data[even_index+1] = even_imag + s*odd_real + c*odd_imag;
                data[odd_index] = even_real - c*odd_real + s*odd_imag;
                data[odd_index+1] = even_imag - s*odd_real - c*odd_imag;
            }
        }
    }
    /* now we have to undo the index shinanegans. we are left, after l = N-2, with a rotation (1...N-1), i.e. all the bits except the value bit for real/complex are rotated
     * In order to fix this, we need to go through the whole array again, and actually save it to dest:
     */
    for (int i = 0; i < (1<<(N-1)); i++) {
        int real_index = ((i&1)<<(N-1)) | (i >> 1)
        dest[2*i] = data[2*real_index] / (reverse ? (float)(1<<(N-1)) : 1.0f); /* also do the reverse division in the same step*/
        dest[2*i+1] = data[2*real_index+1]/ (reverse ? (float)(1<<(N-1)) : 1.0f);
    }
}






/*without comments:*/

void fft_power_of_two_inplace(float* data, float* dest, size_t len, bool reverse) {
    if(!is_power_of_2(len) || len == 1) return;
    int N = log2(len);
    for(int l = 0; l < N-1; l++) {
        for(int k = 0; k < (1<<(N-l-2)); k++) {
            for (int i = 0; i < (1 << (l-1)); i++) {
                int even_index = (i<<(N-l)) | (k << 1),
                    odd_index = (i<<(N-l)) | (1 << (N-l-1)) | (k << 1);
                int i_value = (i<<1)|((i&(1>>(l-1)))>>(l-1));
                float c = cos(2*M_PI*i_value/2^l),
                      s = (reverse ? -1.0 : 1.0)*sin(2*M_PI*i_value/2^l);
                float even_real = data[even_index], even_imag = data[even_index+1];
                float odd_real  = data[odd_index],  odd_imag  = data[odd_index+1];
                data[even_index]   = even_real + c*odd_real - s*odd_imag;
                data[even_index+1] = even_imag + s*odd_real + c*odd_imag;
                data[odd_index]    = even_real - c*odd_real + s*odd_imag;
                data[odd_index+1]  = even_imag - s*odd_real - c*odd_imag;
            }
        }
    }
    for (int i = 0; i < (1<<(N-1)); i++) {
        int real_index = ((i&1)<<(N-1)) | (i >> 1)
        dest[2*i] = data[2*real_index] / (reverse ? (float)(1<<(N-1)) : 1.0f);
        dest[2*i+1] = data[2*real_index+1] / (reverse ? (float)(1<<(N-1)) : 1.0f);
    }
}








/* fixed version: */


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
			//printf("%b: l=%i i=%b, k=%b, e=%b, o=%b, iv=%b, c=%f, s=%f, er=%f, ei=%f, or =%f, oi=%f\n", combined_i, l, inner_index, bucket, even_index, odd_index, i_value, c, s, even_real, even_imag, odd_real, odd_imag);
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


/* based on:
l=0
X k1 .... kN-2 Y
X k1 ... kN-2 Y => at right place

l=1
i1 X k1 ... kN-3 Y
X i1 k1 ... kN-3 Y
so in other words, b1 ... bn has to be looked up at b2 b1 ... bn

l=2
so we look up
i1 i2 X k1 ... kN-4 Y
at index
i2 i1 X k1 ... kN-4 Y
into
X i1 i2 k1 ... kN-3 Y
which is stored at the same index
so in other words, b1 ... bn has to be looked up at b3 b2 b1 b4 ... bn

i=3
so we look up
i1 i2 i3 X k1 ... kN-5 Y
at indext
i3 i2 i1 X k1 .. kN-5 Y
into
X i1 i2 i3 k1 ... kN-5 Y
which is stored at the same index,
in other words, b1 ... bn has to be looked up at b4 b3 b2 b1 b5 ... bn

l=4
so we look up
i1 i2 i3 i4 X k1 ... kN-6 Y
at index
i4 i3 i2 i1 X k1 ... kN-6 Y
into
X i1 i2 i3 i4 k1 ... kN-6 Y
which is stored at the same index
in other words, b1...bn has to be looked up at b5 b4 b3 b2 b1 b6... bn

 */




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

float* fft_power_of_two_real(float* data, size_t len, bool reverse) {
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
    for(int i = 0; i < iteration_size/2; i++) {
        even[2*i] = data[4*i];
        even[2*i+1] = data[4*i+1];
        odd[2*i] = data[4*i+2];
        odd[2*i+1] = data[4*i+3];
    }
    float* even_fft = fft_power_of_two_real(even, iteration_size, reverse);
    float* odd_fft = fft_power_of_two_real(odd, iteration_size, reverse);
    free(even); free(odd);

    for(int i = 0; i < iteration_size/2; i++) {
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
