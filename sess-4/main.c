#include "common.h"


void reverse_analyze(char* path, int chosen_channel, int chosen_reduction_power, int frequency_display_reduction_power) {
    /*1. loading of data and FFT */
    waveform_t testform = read_amplitude_data(path, chosen_channel);
    size_t new_len = truncate_power_of_2(testform.data_length) >> chosen_reduction_power;
    float* amplitudes = transform_to_complex_array(testform.amplitude_data, new_len);
    float* frequencies = ifft_power_of_two(amplitudes, new_len * 2);
    float* combined_frequencies = compute_complex_absolute_values(frequencies, new_len);
    float* normalized_amplitudes = normalize_int_array(testform.amplitude_data, new_len);
    float* normalized_frequencies = normalize_float_array(combined_frequencies, new_len);


    /* 2. string and graph creation: */
    int16_t max_abs_amplitude = 0;
    int max_amp_index = 0;
    maximum_abs_value_and_position_int_array(testform.amplitude_data, new_len, &max_amp_index, &max_abs_amplitude);
    char max_amplitude_string[80];
    snprintf(max_amplitude_string, 80, "Maximum Amplitude: %.6f \n(index %i)", (float) max_abs_amplitude, max_amp_index);

    float max_combined_freq = 0.0f;
    int max_freq_index = 0;
    maximum_abs_value_and_position_float_array(frequencies, new_len, &max_freq_index, &max_combined_freq);
    char max_frequency_string[80];
    snprintf(max_frequency_string, 80, "Maximum Frequency: %.6f \n(index %i)", max_combined_freq, max_freq_index);

    char duration_string[40];
    float actual_play_duration = duration(testform)/(1<<chosen_reduction_power);
    snprintf(duration_string, 40, "Duration: %.6fs\nSamples/s: %i", actual_play_duration, testform.samples_per_second);

    Vector2* normalized_amplitude_graph = create_graph_from_float_array(normalized_amplitudes, new_len, 15.0f, 350.0f, 350.0f, 200.0f);
    Vector2* normalized_freuquency_graph = create_graph_from_float_array(normalized_frequencies, (new_len>>frequency_display_reduction_power), 415.0f, 550.0f, 350.0f, 400.0f);


    /* 3. disply the graph: */
	int width = 800, height = 600;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(width, height, "sound wave and fft tool");
	SetTargetFPS(30);
	while(!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(RAYWHITE);

			DrawText("Analyzing File: ", 15, 20, 20, BLACK);
			DrawText(path, 200, 20, 20, BLACK);
			DrawText(duration_string, 415, 20, 20, BLACK);


			DrawText("Amplitudes", 15, 70, 20, BLACK);
			DrawText("Frequencies", 415, 70, 20, BLACK);
			DrawText(max_amplitude_string, 15, 90, 20, BLACK);
			DrawText(max_frequency_string, 415, 90, 20, BLACK);
            DrawSplineLinear(normalized_amplitude_graph, new_len, 1.0f, BLUE);
            DrawSplineLinear(normalized_freuquency_graph, new_len/2, 1.0f, BLUE);
		EndDrawing();
	}
	CloseWindow();

    /* 4. cleanup: */

    free(testform.amplitude_data);
    free(normalized_amplitudes);
    free(combined_frequencies);
    free(normalized_frequencies);
    free(frequencies);
    free(amplitudes);
	free(normalized_amplitude_graph);
	free(normalized_freuquency_graph);
}



void analyze(char* path, int chosen_channel, int chosen_reduction_power, int frequency_display_reduction_power) {
    /*1. loading of data and FFT */
    waveform_t testform = read_amplitude_data(path, chosen_channel);
    size_t new_len = truncate_power_of_2(testform.data_length) >> chosen_reduction_power;
    float* amplitudes = transform_to_complex_array(testform.amplitude_data, new_len);
    float* frequencies = fft_power_of_two(amplitudes, new_len * 2);
    float* combined_frequencies = compute_complex_absolute_values(frequencies, new_len);
    float* normalized_amplitudes = normalize_int_array(testform.amplitude_data, new_len);
    float* normalized_frequencies = normalize_float_array(combined_frequencies, new_len);


    /* 2. string and graph creation: */
    int16_t max_abs_amplitude = 0;
    int max_amp_index = 0;
    maximum_abs_value_and_position_int_array(testform.amplitude_data, new_len, &max_amp_index, &max_abs_amplitude);
    char max_amplitude_string[80];
    snprintf(max_amplitude_string, 80, "Maximum Amplitude: %.6f \n(index %i)", (float) max_abs_amplitude, max_amp_index);

    float max_combined_freq = 0.0f;
    int max_freq_index = 0;
    maximum_abs_value_and_position_float_array(frequencies, new_len, &max_freq_index, &max_combined_freq);
    char max_frequency_string[80];
    snprintf(max_frequency_string, 80, "Maximum Frequency: %.6f \n(index %i)", max_combined_freq, max_freq_index);

    char duration_string[40];
    float actual_play_duration = duration(testform)/(1<<chosen_reduction_power);
    snprintf(duration_string, 40, "Duration: %.6fs\nSamples/s: %i", actual_play_duration, testform.samples_per_second);

    Vector2* normalized_amplitude_graph = create_graph_from_float_array(normalized_amplitudes, new_len, 15.0f, 350.0f, 350.0f, 200.0f);
    Vector2* normalized_freuquency_graph = create_graph_from_float_array(normalized_frequencies, (new_len>>frequency_display_reduction_power), 415.0f, 550.0f, 350.0f, 400.0f);


    /* 3. disply the graph: */
	int width = 800, height = 600;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(width, height, "sound wave and fft tool");
	SetTargetFPS(30);
	while(!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(RAYWHITE);

			DrawText("Analyzing File: ", 15, 20, 20, BLACK);
			DrawText(path, 200, 20, 20, BLACK);
			DrawText(duration_string, 415, 20, 20, BLACK);


			DrawText("Amplitudes", 15, 70, 20, BLACK);
			DrawText("Frequencies", 415, 70, 20, BLACK);
			DrawText(max_amplitude_string, 15, 90, 20, BLACK);
			DrawText(max_frequency_string, 415, 90, 20, BLACK);
            DrawSplineLinear(normalized_amplitude_graph, new_len, 1.0f, BLUE);
            DrawSplineLinear(normalized_freuquency_graph, new_len/2, 1.0f, BLUE);
		EndDrawing();
	}
	CloseWindow();

    /* 4. cleanup: */

    free(testform.amplitude_data);
    free(normalized_amplitudes);
    free(combined_frequencies);
    free(normalized_frequencies);
    free(frequencies);
    free(amplitudes);
	free(normalized_amplitude_graph);
	free(normalized_freuquency_graph);
}

size_t* get_top_n_frequency_positions(const float* array, size_t length, size_t n) {
    size_t* top_frequency_positions = calloc(n,sizeof(size_t));
    float* top_frequency_values = calloc(n,sizeof(float));
    for(int i = 0; i < length; i++) {
        float value = fabsf(array[i]);
        for(int j = 0; j < n; j++) {
            if(value > top_frequency_values[j]) {
                memmove(&top_frequency_values[j+1], &top_frequency_values[j], (n-(j+1))*sizeof(float));
                memmove(&top_frequency_positions[j+1], &top_frequency_positions[j], (n-(j+1))*sizeof(size_t));
                top_frequency_values[j] = value;
                top_frequency_positions[j] = i;
                break;
            }
        }
    }
    free(top_frequency_values);
    return top_frequency_positions;
}

void modify(char* path, int chosen_channel, int chosen_reduction_power, char* kind, char* new_path) {
    waveform_t testform = read_amplitude_data(path, chosen_channel);
    size_t new_len = truncate_power_of_2(testform.data_length) >> chosen_reduction_power;
    float* amplitudes = transform_to_complex_array(testform.amplitude_data, new_len);
    float* frequencies = fft_power_of_two(amplitudes, new_len * 2);
    int16_t max_abs_amplitude = 0; int max_amp_index = 0;
    maximum_abs_value_and_position_int_array(testform.amplitude_data, new_len, &max_amp_index, &max_abs_amplitude);
    float max_freq = 0.0f; int max_freq_index;
    maximum_abs_value_and_position_float_array(frequencies, new_len*2, &max_freq_index, &max_freq);


    if(strcmp(kind,"closed")==0) {
        for(int i = 0; i < new_len*2; i++) {
            if(fabs(frequencies[i]) > 0.1f*max_freq) {
                frequencies[i] *= 1.05f;
            }
        }
    } else if(strcmp(kind,"closify2")==0) {
        float old = 0.0f;
        for(int i = 0; i < new_len*2; i++) {
            if(frequencies[i] < old) {
                frequencies[i] += (old - frequencies[i])*0.5f;
            }
            old = frequencies[i];
        }
    } else if (strcmp(kind,"double_len")==0) {
        float* old_frequencies = frequencies;
        frequencies = malloc(4*new_len*sizeof(float));
        for(int i = 0; i < new_len; i++) {
            frequencies[4*i] = old_frequencies[2*i];
            frequencies[4*i+1] = old_frequencies[2*i+1];
            frequencies[4*i+2] = 0.0f;
            frequencies[4*i+3] = 0.0f;
        }
        free(old_frequencies);
        new_len *= 2;
    } else if (strcmp(kind, "up_octave") == 0) {
        for(int i = (new_len-1)*2; i >= max_freq_index; i--) {
            frequencies[i] = frequencies[i-max_freq_index];
        }
        for(int i = 0; i < max_freq_index; i++) {
            frequencies[i] = 0;
        }
    } else if (strcmp(kind, "larynx_stretch_1") == 0) {
        for(int i = 0; i < new_len*2; i++) {
            if(fabs(frequencies[i]) < 0.3f*max_freq) {
                frequencies[i] = 0.0f;
            } else {
                frequencies[i] *= 0.7f;
            }
        }

    } else if (strcmp(kind, "reduce_active_frequencies") == 0) {

        const size_t nr_active_frequencies = 1000;

        size_t* active_frequencies = get_top_n_frequency_positions(frequencies, new_len*2, nr_active_frequencies);

        float* new_frequencies = calloc(new_len*2,sizeof(float));

        for(int k = 0; k < nr_active_frequencies; k++) {
            new_frequencies[active_frequencies[k]] = frequencies[active_frequencies[k]];
        }

        free(frequencies);
        frequencies = new_frequencies;

    }
    else {
        die("kind of operation not supported\n");
    }


    float* new_amps = ifft_power_of_two(frequencies, new_len * 2);
    waveform_t new_form;
    new_form.samples_per_second = testform.samples_per_second;
    new_form.data_length = new_len;
    new_form.amplitude_data = transform_complex_to_int_array(new_amps, new_len);
    write_amplitude_data(new_path, new_form);


    free(new_form.amplitude_data);
    free(new_amps);
    free(testform.amplitude_data);
    free(amplitudes);
    free(frequencies);
}

void recognize_change_point(char* path, int chosen_channel, int chosen_reduction_power) {
    /* 1. read in base graph */
    waveform_t testform = read_amplitude_data(path, chosen_channel);
    size_t new_len = truncate_power_of_2(testform.data_length)>>chosen_reduction_power;
    float* old_amplitudes = transform_to_complex_array(testform.amplitude_data, new_len);
    float* old_frequencies = fft_power_of_two(old_amplitudes, new_len * 2);


    float* absolute_differences = calloc((1<<chosen_reduction_power), sizeof(float));
    float* relative_differences = calloc((1<<chosen_reduction_power), sizeof(float));

    for(int i = 1; i < (1<<chosen_reduction_power); i++) {
        float* new_amplitudes = transform_to_complex_array((&(testform.amplitude_data[i*new_len])), new_len);
        float* new_frequencies = fft_power_of_two(new_amplitudes, new_len * 2);

        float absolute_difference = 0.0f;
        float old_size = 0.0f;
        for(int k = 0; k < 2*new_len; k++) {
            absolute_difference += fabsf(new_frequencies[i]-old_frequencies[i]);
            old_size += fabsf(old_frequencies[i]);
        }
        printf("at %i: %f (rel: %f)\n", i, absolute_difference, absolute_difference/old_size);

        free(old_amplitudes); free(old_frequencies);
        old_amplitudes = new_amplitudes; old_frequencies = new_frequencies;

        absolute_differences[i] = absolute_difference;
        relative_differences[i] = absolute_difference/old_size;
    }


    float max_abs_diff = 0; int max_abs_diff_index = 0;
    maximum_abs_value_and_position_float_array(absolute_differences, (1<<chosen_reduction_power), &max_abs_diff_index, &max_abs_diff);
    float max_rel_diff = 0.0f; int max_rel_diff_index;
    maximum_abs_value_and_position_float_array(relative_differences, (1<<chosen_reduction_power), &max_rel_diff_index, &max_rel_diff);


    for(int i = 1; i < (1<<chosen_reduction_power); i++) {
        absolute_differences[i] /= max_abs_diff;
        relative_differences[i] /= max_rel_diff;
    }


    Vector2* absolute_difference_graph = create_graph_from_float_array(absolute_differences, (1<<chosen_reduction_power), 15.0f, 350.0f, 350.0f, 200.0f);
    Vector2* relative_difference_graph = create_graph_from_float_array(relative_differences, (1<<chosen_reduction_power), 415.0f, 550.0f, 350.0f, 400.0f);


    /* 3. disply the graph: */
	int width = 800, height = 600;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(width, height, "sound wave and fft tool");
	SetTargetFPS(30);
	while(!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(RAYWHITE);

			DrawText("Analyzing File: ", 15, 20, 20, BLACK);
			DrawText(path, 200, 20, 20, BLACK);


			DrawText("Absolute Differences", 15, 70, 20, BLACK);
			DrawText("Relative Differences", 415, 70, 20, BLACK);
            DrawSplineLinear(absolute_difference_graph, (1<<chosen_reduction_power), 1.0f, BLUE);
            DrawSplineLinear(relative_difference_graph, (1<<chosen_reduction_power), 1.0f, BLUE);
		EndDrawing();
	}
	CloseWindow();





    free(old_amplitudes);
    free(old_frequencies);
    free(testform.amplitude_data);
}



void create_pitch_shifted_wave(char* path, int chosen_channel, int chosen_reduction_power, char* kind, char* new_path, char* pitch_shift) {
    waveform_t testform = read_amplitude_data(path, chosen_channel);
    size_t new_len = truncate_power_of_2(testform.data_length) >> chosen_reduction_power;
    float* amplitudes = transform_to_complex_array(testform.amplitude_data, new_len);
    float* frequencies = fft_power_of_two(amplitudes, new_len * 2);
    int16_t max_abs_amplitude = 0; int max_amp_index = 0;
    maximum_abs_value_and_position_int_array(testform.amplitude_data, new_len, &max_amp_index, &max_abs_amplitude);
    float max_freq = 0.0f; int max_freq_index;
    maximum_abs_value_and_position_float_array(frequencies, new_len*2, &max_freq_index, &max_freq);


    if (strcmp(kind, "shift_up") == 0) {
        char* check_pointer;
        float pitch_in_hertz = strtof(pitch_shift, &check_pointer);
        printf("recognized freq: %f\n", pitch_in_hertz);
        errno = 0;
        if(check_pointer == pitch_shift || errno != 0) {
            die("pitch shift value not regonized");
        }
        if(pitch_in_hertz > testform.samples_per_second) {
            die("pitch shift too high for sample frequency (above nyquist)");
        }
        float relative_frequency_shift = pitch_in_hertz / (float) testform.samples_per_second;
        size_t index_shift = floor((((float)new_len)*relative_frequency_shift))*2;

        float* new_frequencies = calloc(new_len*2,sizeof(float));
        for(int i = (new_len-1)*2; i >= index_shift; i--) {
            new_frequencies[i] = frequencies[i-index_shift];
        }
        free(frequencies);
        frequencies = new_frequencies;
    }
    else {
        die("kind of operation not supported\n");
    }


    float* new_amps = ifft_power_of_two(frequencies, new_len * 2);
    waveform_t new_form;
    new_form.samples_per_second = testform.samples_per_second;
    new_form.data_length = new_len;
    new_form.amplitude_data = transform_complex_to_int_array(new_amps, new_len);
    write_amplitude_data(new_path, new_form);


    free(new_form.amplitude_data);
    free(new_amps);
    free(testform.amplitude_data);
    free(amplitudes);
    free(frequencies);
}


int main(int argc, char** argv) {
    if(argc < 2) die("not enough arguments!\n");

    switch(argv[1][0]) {
        case 'a':
            if(argc < 6) die("not enough arguments!\n");
            analyze(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
        break;

        case 'r':
            if(argc < 6) die("not enough arguments!\n");
            reverse_analyze(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
        break;

        case 'm':
            if(argc < 7) die("not enough arguments!\n");
            modify(argv[2], atoi(argv[3]), atoi(argv[4]), argv[5], argv[6]);
        break;
        case 'c': // change
            if(argc < 5) die("not enough arguments!\n");
            recognize_change_point(argv[2], atoi(argv[3]), atoi(argv[4]));
            break;
        case 'p': //pitch
            if(argc < 8) die("not enough arguments!\n");
            create_pitch_shifted_wave(argv[2], atoi(argv[3]), atoi(argv[4]), argv[5], argv[6], argv[7]);
            break;
        case 't':
            float test[2*16] = {112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234453453.735f, 0.0f, 1.000150f, 0.0f, 0.004545f, 0.0f};
            float *testfmid = ifft_power_of_two(test, 2*16);
            float *testf = fft_power_of_two(testfmid, 2*16);
            printf("%f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n", testf[0], testf[1], testf[2], testf[3], testf[4], testf[5], testf[6], testf[7], testf[8], testf[9], testf[10], testf[11], testf[12], testf[13], testf[14], testf[15], testf[16], testf[17], testf[18], testf[19], testf[20], testf[21], testf[22], testf[23], testf[24], testf[25], testf[26], testf[27], testf[28], testf[29], testf[30], testf[31]);
            free(testf);free(testfmid);
        break;
    }
	return 0;
}
