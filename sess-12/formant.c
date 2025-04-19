#include "common.h"

int find_formants(waveform_t form, int nr_formants, matrix_t* formant_values, matrix_t* formant_bandwidths, bool do_extra_division) {
    double rate = (double) form.samples_per_second;
    double* data = calloc(form.data_length, sizeof(double));
    for(int i = 1; i < form.data_length; i++) {
        data[i] = ((0.54-0.46*cos(2*M_PI*((float)i)/(form.data_length)))* form.amplitude_data[i]) + 0.63f*data[i-1];
    }
    
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
        if (F >= 0 && bw < 400 && filtered_polynomial_roots2[2*i+1] > 0.1) {
            printf ("%f Hz +- %f Hz\n", F, bw);
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