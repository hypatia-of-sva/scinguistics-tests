#include "common.h"

int main(int argc, char** argv) {
    if(argc < 2) die("not enough arguments!\n");

    /* 1. load wave file data: */
    char* path = argv[1];
    int chosen_channel = atoi(argv[2]);
    waveform_t testform = read_amplitude_data(path, chosen_channel);

    /* 2. calculate play duration: */
    float duration = ((float)testform.data_length)/((float)testform.samples_per_second);
    char duration_string[22];
    snprintf(duration_string, 22, "Duration: %.6fs", duration);

    /* 3. transform wavefile into complex float array: */
    int16_t max_abs_amplitude = 0;
    size_t new_len = truncate_power_of_2(testform.data_length);
    float* amplitudes = malloc(new_len * 2 * sizeof(float));
    for(int i = 0; i < new_len; i++) {
        amplitudes[2*i] = (float) testform.amplitude_data[i];
        amplitudes[2*i+1] = 0.0f;
        if(abs(testform.amplitude_data[i]) > abs(max_abs_amplitude)) max_abs_amplitude = abs(testform.amplitude_data[i]);
    }
    free(testform.amplitude_data);

    /* 4. apply FFT: */
    float* frequencies = fft_power_of_two_real(amplitudes, new_len * 2, false);

    /* 5. combine sine and cosine frequencies: */
    float* combined_frequencies = malloc(new_len * sizeof(float));
    float max_combined_freq = 0.0f;
    for(int i = 0; i < new_len; i++) {
        combined_frequencies[i] = sqrtf(frequencies[2*i]*frequencies[2*i] + frequencies[2*i+1]*frequencies[2*i+1]);
        if(combined_frequencies[i] > max_combined_freq) max_combined_freq = combined_frequencies[i];
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

    /* 7. create point graphs for amplitude and frequency diagrams: */
    float amp_base_x = 15.0f, amp_base_y = 350.0f, amp_max_x = 350.0f, amp_max_y = 200.0f;
    Vector2* normalized_amplitude_graph = malloc(new_len*sizeof(Vector2));
    for(int i = 0; i < new_len; i++) {
        normalized_amplitude_graph[i].x = amp_base_x + (amp_max_x/new_len)*i;
        normalized_amplitude_graph[i].y = amp_base_y + amp_max_y*normalized_amplitudes[i];
    }
    free(normalized_amplitudes);

    float freq_base_x = 415.0f, freq_base_y = 350.0f, freq_max_x = 350.0f, freq_max_y = 200.0f;
    Vector2* normalized_freuquency_graph = malloc(new_len*sizeof(Vector2));
    for(int i = 0; i < new_len; i++) {
        normalized_freuquency_graph[i].x = freq_base_x + (freq_max_x/new_len)*i;
        normalized_freuquency_graph[i].y = freq_base_y - freq_max_y*normalized_frequencies[i];
    }
    free(normalized_frequencies);


    /* 8. disply the graph: */
	int width = 800, height = 600;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(width, height, "sound wave and fft tool");
	SetTargetFPS(30);
	while(!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(RAYWHITE);

			DrawText("Analyzing File: ", 15, 20, 20, BLACK);
			DrawText(argv[1], 200, 20, 20, BLACK);
			DrawText(duration_string, 415, 20, 20, BLACK);


			DrawText("Amplitudes", 15, 70, 20, BLACK);
			DrawText("Frequencies", 415, 70, 20, BLACK);
            DrawSplineBezierCubic(normalized_amplitude_graph, new_len, 1.0f, BLUE);
            DrawSplineBezierCubic(normalized_freuquency_graph, new_len, 1.0f, BLUE);
		EndDrawing();
	}
	CloseWindow();

    /* 9. cleanup: */
	free(normalized_amplitude_graph);
	free(normalized_freuquency_graph);

	return 0;
}
