#include "common.h"


formant_params_t g_params;

void init_formant_params(void) {
    g_params.filter_strength = 0.63f;
    g_params.window_param1 = 0.54f;
    g_params.window_param2 = 0.46f;
    g_params.min_freq = 0.0f;
    g_params.max_bw = 400.0f;
    g_params.min_imag = 0.1f;
}

int find_formants(waveform_t form, int nr_formants, matrix_t* formant_values, matrix_t* formant_bandwidths, bool do_extra_division) {
    float filter_strength = g_params.filter_strength;
    float window_param1 = g_params.window_param1;
    float window_param2 = g_params.window_param2;

    double rate = (double) form.samples_per_second;
    double* data = calloc(form.data_length, sizeof(double));
    for(int i = 1; i < form.data_length; i++) {
        data[i] = ((window_param1-window_param2*cos(2*M_PI*((float)i)/(form.data_length)))* form.amplitude_data[i]) + filter_strength*data[i-1];
    }


/*
    size_t new_len = truncate_power_of_2(form.data_length);
    float* amplitudes = malloc(new_len * 2 * sizeof(float));
    for(int i = 0; i < new_len; i++) {
        amplitudes[2*i] = (float) data[i];
        amplitudes[2*i+1] = 0.0f;
    }
    float* frequencies = fft_power_of_two(amplitudes, new_len*2);
    float* combined_frequencies = compute_complex_absolute_values(frequencies, new_len);
    float* normalized_frequencies = normalize_float_array(combined_frequencies, new_len);
    write_float_array(normalized_frequencies, new_len, "norm_filter_frequencies_test.txt");
    calculate_peaks(normalized_frequencies, new_len, "norm_filter_frequencies_peaks.txt");
*/


    matrix_t filtered_lpc_values = lpc_float(data, form.data_length, nr_formants);
    double* filtered_polynomial_coefficients = calloc(nr_formants+1, sizeof(double));
    filtered_polynomial_coefficients[nr_formants] = 1.0;
    for(int i = 0; i < nr_formants; i++) {
        filtered_polynomial_coefficients[nr_formants-i-1] = -get(filtered_lpc_values, 0, i);
    }

    double* filtered_polynomial_roots2 = calloc(2*nr_formants, sizeof(double));
    double* filtered_tmp_matrix = calloc(nr_formants*nr_formants, sizeof(double));
    poly_complex_solve(filtered_polynomial_coefficients, nr_formants+1, filtered_polynomial_roots2, filtered_tmp_matrix);
    
    if(formant_values != NULL) {
        formant_values[0] = create_matrix(1,nr_formants);
    }
    if(formant_bandwidths != NULL) {
        formant_bandwidths[0] = create_matrix(1,nr_formants);
    }

    for (int i = 0; i < nr_formants; i++) {
        double F = (atan2(filtered_polynomial_roots2[2*i+1],filtered_polynomial_roots2[2*i]))*(rate/(2*M_PI));
        if(do_extra_division) F /= (2*M_PI);
        double bw = (-0.5)*((rate)/(2*M_PI))*log(sqrt(filtered_polynomial_roots2[2*i]*filtered_polynomial_roots2[2*i] + filtered_polynomial_roots2[2*i+1]*filtered_polynomial_roots2[2*i+1]));
        //if(filtered_polynomial_roots2[2*i+1] > 0.1 && F > 90 && bw < 400)
        //printf ("z%d = %+.18f %+.18f\nF=%f, bw=%f\n", i, filtered_polynomial_roots2[2*i], filtered_polynomial_roots2[2*i+1], F, bw);
        if (F >= g_params.min_freq && bw < g_params.max_bw && filtered_polynomial_roots2[2*i+1] > g_params.min_imag) {
            /*printf ("%f Hz +- %f Hz\n", F, bw);*/
            if(formant_values != NULL) {
                set(formant_values[0],0,i,F);
            }
            if(formant_bandwidths != NULL) {
                set(formant_bandwidths[0],0,i,bw);
            }
        }
    }

    return 0;
}
