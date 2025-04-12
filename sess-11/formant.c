#include "common.h"
/*
#include <gsl/gsl_poly.h>
*/


int find_formants(waveform_t form, int nr_formants, matrix_t* formant_values, matrix_t* formant_bandwidths) {



    matrix_t lpc_values = lpc(form, nr_formants);
    double* polynomial_coefficients = calloc(nr_formants+1, sizeof(double));
    polynomial_coefficients[0] = 1.0;
    for(int i = 0; i < nr_formants; i++) {
        polynomial_coefficients[i+1] = -get(lpc_values, 0, i);
    }

    /*
    double* polynomial_roots = calloc(2*nr_formants, sizeof(double));

    gsl_poly_complex_workspace* workspace = gsl_poly_complex_workspace_alloc(nr_formants+1);
    gsl_poly_complex_solve(polynomial_coefficients, nr_formants+1, workspace, polynomial_roots);
    gsl_poly_complex_workspace_free (workspace);


    for (int i = 0; i < nr_formants; i++) {
        printf ("z%d = %+.18f %+.18f\n", i, polynomial_roots[2*i], polynomial_roots[2*i+1]);
    }
    */

    double* polynomial_roots2 = calloc(2*nr_formants, sizeof(double));
    double* tmp_matrix = calloc(nr_formants*nr_formants, sizeof(double));
    poly_complex_solve(polynomial_coefficients, nr_formants+1, polynomial_roots2, tmp_matrix);


    for (int i = 0; i < nr_formants; i++) {
        double F = (atan2(polynomial_roots2[2*i],polynomial_roots2[2*i+1])/(2*M_PI))*((double)form.samples_per_second);
        double bw = (-0.5)*(((double)form.samples_per_second)/(2*M_PI))*log(sqrt(polynomial_roots2[2*i]*polynomial_roots2[2*i] + polynomial_roots2[2*i+1]*polynomial_roots2[2*i+1]));
        //printf ("z%d = %+.18f %+.18f\nF=%f, bw=%f\n", i, polynomial_roots2[2*i], polynomial_roots2[2*i+1], F, bw);
        if(polynomial_roots2[2*i+1] >= 0 && F > 90 && bw < 400)
        printf ("%f Hz +- %f Hz\n", F, bw);
    }




    waveform_t filtered_form;
    filtered_form.samples_per_second = form.samples_per_second;
    filtered_form.data_length = form.data_length;
    filtered_form.amplitude_data = calloc(form.data_length, sizeof(int16_t));

    filtered_form.amplitude_data[0] = form.amplitude_data[0];
    for(int i = 1; i < filtered_form.data_length; i++) {
        filtered_form.amplitude_data[i] = ((0.53836-0.46164*cos(2*M_PI*((float)i)/(filtered_form.data_length-1)))* form.amplitude_data[i]) - 0.63f*filtered_form.amplitude_data[i-1];
    }

    matrix_t filtered_lpc_values = lpc(filtered_form, nr_formants);
    double* filtered_polynomial_coefficients = calloc(nr_formants+1, sizeof(double));
    filtered_polynomial_coefficients[0] = 1.0;
    for(int i = 0; i < nr_formants; i++) {
        filtered_polynomial_coefficients[i+1] = -get(filtered_lpc_values, 0, i);
    }

    double* filtered_polynomial_roots2 = calloc(2*nr_formants, sizeof(double));
    double* filtered_tmp_matrix = calloc(nr_formants*nr_formants, sizeof(double));
    poly_complex_solve(filtered_polynomial_coefficients, nr_formants+1, filtered_polynomial_roots2, filtered_tmp_matrix);

    printf("\n\nfiltered: \n");
    for (int i = 0; i < nr_formants; i++) {
        double F = (atan2(filtered_polynomial_roots2[2*i],filtered_polynomial_roots2[2*i+1]))*((double)form.samples_per_second/(2*M_PI));
        double bw = (-0.5)*(((double)form.samples_per_second)/(2*M_PI))*log(sqrt(filtered_polynomial_roots2[2*i]*filtered_polynomial_roots2[2*i] + filtered_polynomial_roots2[2*i+1]*filtered_polynomial_roots2[2*i+1]));
        if(filtered_polynomial_roots2[2*i+1] > 0.1 && F > 90 && bw < 400)
        //printf ("z%d = %+.18f %+.18f\nF=%f, bw=%f\n", i, filtered_polynomial_roots2[2*i], filtered_polynomial_roots2[2*i+1], F, bw);
        printf ("%f Hz +- %f Hz\n", F, bw);
    }

    return 0;
}






int find_formants_2(waveform_t form, int nr_formants, matrix_t* formant_values, matrix_t* formant_bandwidths, bool do_extra_division) {
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