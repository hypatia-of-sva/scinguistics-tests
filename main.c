#include "common.h"

int main(void) {




    waveform_t testform = read_amplitude_data("../open.wav", 0);

    printf("samples/second: %i \n", testform.samples_per_second);
    printf("length: %li \n", testform.data_length);
    printf("duration: %f s\n\n", ((float)testform.data_length)/((float)testform.samples_per_second));
    //printf("data:\n");
    for(int i = 0; i < testform.data_length; i++) {
        //printf("%i\n", testform.amplitude_data[i]);
    }
    //printf("[END]");





    /* transform wavefile into complex float array: */

    int16_t max_abs_amplitude = 0;
    size_t new_len = truncate_power_of_2(testform.data_length);
    float* amplitudes = malloc(new_len * 2 * sizeof(float));
    for(int i = 0; i < new_len; i++) {
        amplitudes[2*i] = (float) testform.amplitude_data[i];
        amplitudes[2*i+1] = 0.0f;
        if(abs(testform.amplitude_data[i]) > abs(max_abs_amplitude)) max_abs_amplitude = abs(testform.amplitude_data[i]);
    }
    free(testform.amplitude_data);

    printf("max ampl: %i \n", max_abs_amplitude);



    float* frequencies = fft_power_of_two_real(amplitudes, new_len * 2, false);

    printf("pointer: %lx \n\n", (long) frequencies);



    //printf("first datum: %f , %f", frequencies[0], frequencies[1]);

    float* combined_frequencies = malloc(new_len * sizeof(float));

    float max_combined_freq = 0.0f;
    for(int i = 0; i < new_len; i++) {
        combined_frequencies[i] = sqrtf(frequencies[2*i]*frequencies[2*i] + frequencies[2*i+1]*frequencies[2*i+1]);
        //printf("%f\n", combined_frequencies[i]);
        if(combined_frequencies[i] > max_combined_freq) max_combined_freq = combined_frequencies[i];
    }
    printf("max freq: %f \n", max_combined_freq);

    free(frequencies);



    float* normalized_frequencies = malloc(new_len * sizeof(float));
    float* normalized_amplitudes = malloc(new_len * sizeof(float));
    for(int i = 0; i < new_len; i++) {
        normalized_frequencies[i] = combined_frequencies[i]/max_combined_freq;
        normalized_amplitudes[i] = amplitudes[2*i]/max_abs_amplitude;
    }

/*
    printf("normalized ampl:\n");
    for(int i = 0; i < new_len; i++) {
        printf("%f\n", normalized_amplitudes[i]);
    }
    printf("normalized freq:\n");
    for(int i = 0; i < new_len; i++) {
        printf("%f\n", normalized_frequencies[i]);
    }
*/







	int width = 800, height = 600;
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(width, height, "sound wave and fft tool");
	SetTargetFPS(30);

	//InitAudioDevice(); //?? music exmpl look


    float amp_base_x = 15.0f, amp_base_y = 300.0f, amp_max_x = 350.0f, amp_max_y = 200.0f;
    Vector2* normalized_amplitude_graph = malloc(new_len*sizeof(Vector2));
    for(int i = 0; i < new_len; i++) {
        normalized_amplitude_graph[i].x = amp_base_x + (amp_max_x/new_len)*i;
        normalized_amplitude_graph[i].y = amp_base_y + amp_max_y*normalized_amplitudes[i];
    }


    float freq_base_x = 415.0f, freq_base_y = 300.0f, freq_max_x = 350.0f, freq_max_y = 200.0f;
    Vector2* normalized_freuquency_graph = malloc(new_len*sizeof(Vector2));
    for(int i = 0; i < new_len; i++) {
        normalized_freuquency_graph[i].x = freq_base_x + (freq_max_x/new_len)*i;
        normalized_freuquency_graph[i].y = freq_base_y - freq_max_y*normalized_frequencies[i];
    }

	while(!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText("Amplitudes", 50, 20, 20, BLACK);


            //Vector2 points[5] = {{3.1f, 310.2f}, {20.2f, 310.7f}, {30.3f, 310.8f}, {40.4f, 310.08f}, {50.5f, 310.0f}};
            //DrawSplineBezierCubic(points, 5, 5.0f, BLUE);
            DrawSplineBezierCubic(normalized_amplitude_graph, new_len, 1.0f, BLUE);

            DrawSplineBezierCubic(normalized_freuquency_graph, new_len, 1.0f, BLUE);

			DrawText("Frequencies", 400, 20, 20, BLACK);
			//DrawLineBezier....
		EndDrawing();
	}

	free(normalized_amplitude_graph);


	CloseWindow();




    free(combined_frequencies);
    free(amplitudes);


	return 0;
}
