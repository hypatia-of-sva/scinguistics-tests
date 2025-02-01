#include "common.h"


void analyze(char* path, int chosen_channel, int chosen_reduction_power) {
    /* 1. load wave file data: */
    waveform_t testform = read_amplitude_data(path, chosen_channel);

    /* 2. calculate play duration: */
    float duration = (((float)testform.data_length)/((float)testform.samples_per_second))/(1<<chosen_reduction_power);
    char duration_string[22];
    snprintf(duration_string, 22, "Duration: %.6fs", duration);

    /* 3. transform wavefile into complex float array: */
    int16_t max_abs_amplitude = 0;
    int max_amp_index = 0;
    size_t new_len = truncate_power_of_2(testform.data_length) >> chosen_reduction_power;
    float* amplitudes = malloc(new_len * 2 * sizeof(float));
    for(int i = 0; i < new_len; i++) {
        amplitudes[2*i] = (float) testform.amplitude_data[i];
        amplitudes[2*i+1] = 0.0f;
        if(abs(testform.amplitude_data[i]) > abs(max_abs_amplitude)) {
            max_abs_amplitude = abs(testform.amplitude_data[i]);
            max_amp_index = i;
        }
    }
    free(testform.amplitude_data);

    /* 4. apply FFT: */
    float* frequencies = fft_power_of_two_real(amplitudes, new_len * 2, false);

    /* 5. combine sine and cosine frequencies: */
    float* combined_frequencies = malloc(new_len * sizeof(float));
    float max_combined_freq = 0.0f;
    int max_freq_index = 0;
    for(int i = 0; i < new_len; i++) {
        combined_frequencies[i] = sqrtf(frequencies[2*i]*frequencies[2*i] + frequencies[2*i+1]*frequencies[2*i+1]);
        if(combined_frequencies[i] > max_combined_freq) {
            max_combined_freq = combined_frequencies[i];
            max_freq_index = i;
        }
    }
    free(frequencies);

    /* 6. normalize amplitudes and freuqences to [-1,1]: */
    float* normalized_frequencies = malloc(new_len * sizeof(float));
    float* normalized_amplitudes = malloc(new_len * sizeof(float));
    for(int i = 0; i < new_len; i++) {
        normalized_frequencies[i] = combined_frequencies[i]/max_combined_freq;
        normalized_amplitudes[i] = amplitudes[2*i]/max_abs_amplitude;
    }
    free(combined_frequencies);
    free(amplitudes);

    /* 7. create strings for the amplitude and frequency maximum: */
    char max_amplitude_string[80];
    snprintf(max_amplitude_string, 80, "Maximum Amplitude: %.6f \n(index %i)", (float) max_abs_amplitude, max_amp_index);
    char max_frequency_string[80];
    snprintf(max_frequency_string, 80, "Maximum Frequency: %.6f \n(index %i)", max_combined_freq, max_freq_index);

    /* 8. create point graphs for amplitude and frequency diagrams: */
    float amp_base_x = 15.0f, amp_base_y = 350.0f, amp_max_x = 350.0f, amp_max_y = 200.0f;
    Vector2* normalized_amplitude_graph = malloc(new_len*sizeof(Vector2));
    for(int i = 0; i < new_len; i++) {
        normalized_amplitude_graph[i].x = amp_base_x + (amp_max_x/new_len)*i;
        normalized_amplitude_graph[i].y = amp_base_y - amp_max_y*normalized_amplitudes[i];
    }
    free(normalized_amplitudes);

    float freq_base_x = 415.0f, freq_base_y = 550.0f, freq_max_x = 350.0f, freq_max_y = 400.0f;
    Vector2* normalized_freuquency_graph = malloc(new_len*sizeof(Vector2));
    for(int i = 0; i < new_len; i++) {
        normalized_freuquency_graph[i].x = freq_base_x + (freq_max_x/new_len)*i;
        normalized_freuquency_graph[i].y = freq_base_y - freq_max_y*normalized_frequencies[i];
    }
    free(normalized_frequencies);


    /* 9. disply the graph: */
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
            DrawSplineLinear(normalized_freuquency_graph, new_len, 1.0f, BLUE);
		EndDrawing();
	}
	CloseWindow();

    /* 10. cleanup: */
	free(normalized_amplitude_graph);
	free(normalized_freuquency_graph);
}



void modify(char* path, int chosen_channel, int chosen_reduction_power, char* kind, char* new_path) {
    if(strcmp(kind,"closed")==0) {
        /* 1. load wave file data: */
        waveform_t testform = read_amplitude_data(path, chosen_channel);


        /* 2. transform wavefile into complex float array: */
        int16_t max_abs_amplitude = 0;
        size_t new_len = truncate_power_of_2(testform.data_length) >> chosen_reduction_power;
        float* amplitudes = malloc(new_len * 2 * sizeof(float));
        for(int i = 0; i < new_len; i++) {
            amplitudes[2*i] = (float) testform.amplitude_data[i];
            amplitudes[2*i+1] = 0.0f;
            if(abs(testform.amplitude_data[i]) > abs(max_abs_amplitude)) {
                max_abs_amplitude = abs(testform.amplitude_data[i]);
            }
        }
        free(testform.amplitude_data);


        /* 3. apply FFT: */
        float* frequencies = fft_power_of_two_real(amplitudes, new_len * 2, false);
        free(amplitudes);

        /* 4. calculate maximume fft value */
        float max_freq = 0.0f;
        for(int i = 0; i < new_len*2; i++) {
            if(fabs(frequencies[i]) > max_freq) max_freq = fabs(frequencies[i]);
        }

        /* 5. raise lower portions of the frequency spectrum */
        for(int i = 0; i < new_len*2; i++) {
            float sign = frequencies[i] < 0 ? -1 : 1;
            float sqrt_content = (frequencies[i]/max_freq)*sign;
            float sqrt_squish = sqrt(sqrt_content)*max_freq*sign;
            float small_squish = frequencies[i]*1.0001f;
            if(sqrt_content > 0.999f)
                frequencies[i] = (fabs(sqrt_squish)>fabs(small_squish)) ? small_squish : sqrt_squish;
            printf("%f\n", frequencies[i]);
        }

        /* 6. reverse FFT */
        float* new_amps = fft_power_of_two_real(frequencies, new_len * 2, true);
        free(frequencies);

        /* 4. calculate maximume reverse fft value */
        float max_new_amp = 0.0f;
        float min_new_amp = 0.0f;
        for(int i = 0; i < new_len*2; i++) {
            if(new_amps[i] > max_new_amp) max_new_amp = new_amps[i];
            if(new_amps[i] < min_new_amp) min_new_amp = new_amps[i];
            printf("%f\n", new_amps[i]);
        }

        /* 7 transform complex amplitudes into integer array: */
        waveform_t new_form;
        new_form.samples_per_second = testform.samples_per_second;
        new_form.data_length = new_len;
        new_form.amplitude_data = malloc(new_len*sizeof(int16_t));
        for(int i = 0; i < new_len; i++) {
            float rebased = new_amps[2*i]-min_new_amp;
            float normalized = (rebased)/(max_new_amp-min_new_amp);
            new_form.amplitude_data[i] = (int16_t) ((normalized)*max_abs_amplitude);
            printf("%f via %f and %f into %i (max %f, min %f)\n", new_amps[2*i], rebased, normalized, new_form.amplitude_data[i], max_new_amp, min_new_amp);
        }
        free(new_amps);

        /* 8. write waveform */
        write_amplitude_data(new_path, new_form);
        free(new_form.amplitude_data);

    } else {
        die("kind of operation not supported\n");
    }
}


int main(int argc, char** argv) {
    if(argc < 2) die("not enough arguments!\n");

    switch(argv[1][0]) {
        case 'a':
            if(argc < 5) die("not enough arguments!\n");
            analyze(argv[2], atoi(argv[3]), atoi(argv[4]));
        break;
        case 'm':
            if(argc < 7) die("not enough arguments!\n");
            modify(argv[2], atoi(argv[3]), atoi(argv[4]), argv[5], argv[6]);
        break;
    }
	return 0;
}
