#include "common.h"

float calculate_lpc_float_factor(double* data, size_t len, int k) {
    float res = 0.0f;
    for(int i = 0; i < len - k; i++) {
        res += data[i]*data[i+k];
    }
    return res;
}

matrix_t lpc_float(double* data, size_t len, int nr_formants) {
    matrix_t lpc_matrix = create_matrix(nr_formants, nr_formants);
    matrix_t lpc_vector = create_matrix(1, nr_formants);
    for(int i = 0; i < nr_formants; i++) {
        float factor = calculate_lpc_float_factor(data, len, i+1);
        set(lpc_vector, 0, i, factor );
        for(int k = 0; k < nr_formants-(i+1); k++) {
            set(lpc_matrix, (i+1)+k, k, factor);
            set(lpc_matrix, k, (i+1)+k, factor);
        }
    }
    float base_factor = calculate_lpc_float_factor(data, len, 0);
    for(int i = 0; i < nr_formants; i++) {
        set(lpc_matrix, i, i, base_factor);
    }

    matrix_t result = gaussian(lpc_matrix, lpc_vector);

    destroy_matrix(lpc_matrix);
    destroy_matrix(lpc_vector);

    return result;
}


float lpc_pole_function(matrix_t lpc_vector, float x) {
    float result = 0.0f;
    for (int i = lpc_vector.height; i >= 0; i--) {
        result += get(lpc_vector, 0, i);
        result /= x;
    }
    return 1/(1-result);
}
