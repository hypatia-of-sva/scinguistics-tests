#include "common.h"


float calculate_lpc_factor(waveform_t form, int k) {
    float res = 0.0f;
    for(int i = 0; i < form.data_length - k; i++) {
        res += form.amplitude_data[i]*form.amplitude_data[i+k];
    }
    return res;
}


matrix_t lpc(waveform_t form, int nr_formants) {
    matrix_t lpc_matrix = create_matrix(nr_formants, nr_formants);
    matrix_t lpc_vector = create_matrix(1, nr_formants);
    for(int i = 0; i < nr_formants; i++) {
        float factor = calculate_lpc_factor(form, i+1);
        set(lpc_vector, 0, i, factor );
        for(int k = 0; k < nr_formants-(i+1); k++) {
            set(lpc_matrix, (i+1)+k, k, factor);
            set(lpc_matrix, k, (i+1)+k, factor);
        }
    }
    float base_factor = calculate_lpc_factor(form, 0);
    for(int i = 0; i < nr_formants; i++) {
        set(lpc_matrix, i, i, base_factor);
    }
    //print_matrix(lpc_matrix);
    //print_matrix(lpc_vector);

    matrix_t result = gaussian(lpc_matrix, lpc_vector);

    destroy_matrix(lpc_matrix);
    destroy_matrix(lpc_vector);

    return result;
}
