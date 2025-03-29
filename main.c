#include "common.h"


void spectrogram_intensity(char* path, int chosen_channel, int chosen_reduction_power) {
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


    int reduced_length = (new_len > 4096 ? 4096 : new_len);
    int reduction_factor = new_len/reduced_length;
    printf("FACTOR: %i,%i\n", reduced_length, reduction_factor);


    float* reduced_amplitudes = malloc(reduced_length*sizeof(float));
    for(int i = 0; i < reduced_length; i++) {
        reduced_amplitudes[i] = normalized_amplitudes[i*reduction_factor];
        printf("%i: %f \n", i, reduced_amplitudes[i]);
    }



    Vector2* normalized_amplitude_graph = create_graph_from_float_array(reduced_amplitudes, reduced_length, 15.0f, 350.0f, 350.0f, 200.0f);





    float base_x = 465.0f, base_y = 150.0f, max_x = 350.0f, max_y = 400.0f;
    Vector2* normalized_freuquency_graph = malloc(new_len*sizeof(Vector2));
    for(int i = 0; i < new_len; i++) {
        float frequency_in_herz = ((float)testform.samples_per_second) * (((float)i)/((float) new_len));
        float octave = log2(frequency_in_herz/27.5);
        ///printf("%i: %f,%f\n", i, frequency_in_herz, octave);
        float x_value = octave/10;
        normalized_freuquency_graph[i].y = base_y+(1-x_value)*max_y ;
        normalized_freuquency_graph[i].x = base_x + max_x*normalized_frequencies[i];
    }






    int image_width = 100;
    int image_height = 400;
    unsigned char* greyscale_spectrogram_image = calloc(sizeof(unsigned char),image_height*image_width);
    for(int i = 0; i < image_height; i++) {
        for(int j = 0; j < image_width; j++) {
            greyscale_spectrogram_image[image_width*i+j] = i%256;
        }
    }

    /* curtesy of  https://stackoverflow.com/questions/76897194/how-to-display-an-image-from-an-array-using-raylib */
    Image img = {
        .data = greyscale_spectrogram_image,
        .width = image_width,
        .height = image_height,
        .format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
        .mipmaps = 1
    };
    //Texture2D texture = LoadTextureFromImage(img);
    (void) img;








    Vector2 a0_dash[2];
    a0_dash[1].y = base_y+max_y;
    a0_dash[1].x = 415.0f;
    a0_dash[0].y = base_y+max_y;
    a0_dash[0].x = 465.0f;
    Vector2 a1_dash[2];
    a1_dash[0].y = base_y+max_y*0.9;
    a1_dash[0].x = 465.0f;
    a1_dash[1].y = base_y+max_y*0.9;
    a1_dash[1].x = 415.0f;
    Vector2 a2_dash[2];
    a2_dash[0].y = base_y+max_y*0.8;
    a2_dash[0].x = 465.0f;
    a2_dash[1].y = base_y+max_y*0.8;
    a2_dash[1].x = 415.0f;
    Vector2 a3_dash[2];
    a3_dash[0].y = base_y+max_y*0.7;
    a3_dash[0].x = 465.0f;
    a3_dash[1].y = base_y+max_y*0.7;
    a3_dash[1].x = 415.0f;
    Vector2 a4_dash[2];
    a4_dash[0].y = base_y+max_y*0.6;
    a4_dash[0].x = 465.0f;
    a4_dash[1].y = base_y+max_y*0.6;
    a4_dash[1].x = 415.0f;
    Vector2 a5_dash[2];
    a5_dash[0].y = base_y+max_y*0.5;
    a5_dash[0].x = 465.0f;
    a5_dash[1].y = base_y+max_y*0.5;
    a5_dash[1].x = 415.0f;
    Vector2 a6_dash[2];
    a6_dash[0].y = base_y+max_y*0.4;
    a6_dash[0].x = 465.0f;
    a6_dash[1].y = base_y+max_y*0.4;
    a6_dash[1].x = 415.0f;
    Vector2 a7_dash[2];
    a7_dash[0].y = base_y+max_y*0.3;
    a7_dash[0].x = 465.0f;
    a7_dash[1].y = base_y+max_y*0.3;
    a7_dash[1].x = 415.0f;
    Vector2 a8_dash[2];
    a8_dash[0].y = base_y+max_y*0.2;
    a8_dash[0].x = 465.0f;
    a8_dash[1].y = base_y+max_y*0.2;
    a8_dash[1].x = 415.0f;
    Vector2 a9_dash[2];
    a9_dash[0].y = base_y+max_y*0.1;
    a9_dash[0].x = 465.0f;
    a9_dash[1].y = base_y+max_y*0.1;
    a9_dash[1].x = 415.0f;
    Vector2 a10_dash[2];
    a10_dash[0].y = base_y;
    a10_dash[0].x = 465.0f;
    a10_dash[1].y = base_y;
    a10_dash[1].x = 415.0f;



    /* 3. disply the graph: */
	int width = 800, height = 700;
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
            DrawSplineLinear(normalized_amplitude_graph, reduced_length, 1.0f, BLUE);
            DrawSplineLinear(normalized_freuquency_graph, new_len/2, 1.0f, BLUE);


            /*
            for(int i = 0; i < reduced_length/2; i++) {
                float value = normalized_freuquency_graph[reduction_factor*i].y;
                Color c;
                if(value < 0.5) c = ORANGE;
                else c = RED;
                (void) base_x;
                //DrawRectangle(base_x, normalized_freuquency_graph[reduction_factor*i].y, max_x, 2*i*max_y/reduced_length, c);



            }
            */

            DrawSplineLinear(a0_dash, 2, 1.0f, RED);
            DrawSplineLinear(a1_dash, 2, 1.0f, RED);
            DrawSplineLinear(a2_dash, 2, 1.0f, RED);
            DrawSplineLinear(a3_dash, 2, 1.0f, RED);
            DrawSplineLinear(a4_dash, 2, 1.0f, RED);
            DrawSplineLinear(a5_dash, 2, 1.0f, RED);
            DrawSplineLinear(a6_dash, 2, 1.0f, RED);
            DrawSplineLinear(a7_dash, 2, 1.0f, RED);
            DrawSplineLinear(a8_dash, 2, 1.0f, RED);
            DrawSplineLinear(a9_dash, 2, 1.0f, RED);
            DrawSplineLinear(a10_dash, 2, 1.0f, RED);


			DrawText("A0", floor(a0_dash[1].x - max_x*0.05), floor(a0_dash[1].y+5), 20, BLACK);
			DrawText("A1", floor(a1_dash[1].x - max_x*0.05), floor(a1_dash[1].y+5), 20, BLACK);
			DrawText("A2", floor(a2_dash[1].x - max_x*0.05), floor(a2_dash[1].y+5), 20, BLACK);
			DrawText("A3", floor(a3_dash[1].x - max_x*0.05), floor(a3_dash[1].y+5), 20, BLACK);
			DrawText("A4", floor(a4_dash[1].x - max_x*0.05), floor(a4_dash[1].y+5), 20, BLACK);
			DrawText("A5", floor(a5_dash[1].x - max_x*0.05), floor(a5_dash[1].y+5), 20, BLACK);
			DrawText("A6", floor(a6_dash[1].x - max_x*0.05), floor(a6_dash[1].y+5), 20, BLACK);
			DrawText("A7", floor(a7_dash[1].x - max_x*0.05), floor(a7_dash[1].y+5), 20, BLACK);
			DrawText("A8", floor(a8_dash[1].x - max_x*0.05), floor(a8_dash[1].y+5), 20, BLACK);
			DrawText("A9", floor(a9_dash[1].x - max_x*0.05), floor(a9_dash[1].y+5), 20, BLACK);
			DrawText("A10", floor(a10_dash[1].x - max_x*0.05), floor(a10_dash[1].y+5), 20, BLACK);
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






void spectrogram(char* path, int chosen_channel, int chosen_reduction_power) {
    /*1. loading of data and FFT */
    waveform_t testform = read_amplitude_data(path, chosen_channel);


    matrix_t formants = lpc(testform, 20);

    print_matrix(formants);


    return;


    size_t new_len = truncate_power_of_2(testform.data_length) >> chosen_reduction_power;
    float* amplitudes = transform_to_complex_array(testform.amplitude_data, new_len);
    float* frequencies = fft_power_of_two(amplitudes, new_len*2);
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


    int reduced_length = (new_len > 4096 ? 4096 : new_len);
    int reduction_factor = new_len/reduced_length;
    printf("FACTOR: %i,%i\n", reduced_length, reduction_factor);


    float* reduced_amplitudes = malloc(reduced_length*sizeof(float));
    for(int i = 0; i < reduced_length; i++) {
        reduced_amplitudes[i] = normalized_amplitudes[i*reduction_factor];
        printf("%i: %f \n", i, reduced_amplitudes[i]);
    }





    Vector2* normalized_amplitude_graph = create_graph_from_float_array(reduced_amplitudes, reduced_length, 15.0f, 350.0f, 350.0f, 200.0f);





    float base_x = 415.0f, base_y = 550.0f, max_x = 350.0f, max_y = 400.0f;
    Vector2* normalized_freuquency_graph = malloc(new_len*sizeof(Vector2));
    for(int i = 0; i < new_len; i++) {
        float frequency_in_herz = ((float)testform.samples_per_second) * (((float)i)/((float) new_len));
        float octave = log2(frequency_in_herz/27.5);
        ///printf("%i: %f,%f\n", i, frequency_in_herz, octave);
        float x_value = octave/10;
        normalized_freuquency_graph[i].x = base_x + max_x*x_value;
        normalized_freuquency_graph[i].y = base_y - max_y*normalized_frequencies[i];
    }




    Vector2 a0_dash[2];
    a0_dash[1].x = base_x;
    a0_dash[1].y = 600.0f;
    a0_dash[0].x = base_x;
    a0_dash[0].y = 550.0f;
    Vector2 a1_dash[2];
    a1_dash[0].x = base_x + max_x*0.1;
    a1_dash[0].y = 550.0f;
    a1_dash[1].x = base_x + max_x*0.1;
    a1_dash[1].y = 600.0f;
    Vector2 a2_dash[2];
    a2_dash[0].x = base_x + max_x*0.2;
    a2_dash[0].y = 550.0f;
    a2_dash[1].x = base_x + max_x*0.2;
    a2_dash[1].y = 600.0f;
    Vector2 a3_dash[2];
    a3_dash[0].x = base_x + max_x*0.3;
    a3_dash[0].y = 550.0f;
    a3_dash[1].x = base_x + max_x*0.3;
    a3_dash[1].y = 600.0f;
    Vector2 a4_dash[2];
    a4_dash[0].x = base_x + max_x*0.4;
    a4_dash[0].y = 550.0f;
    a4_dash[1].x = base_x + max_x*0.4;
    a4_dash[1].y = 600.0f;
    Vector2 a5_dash[2];
    a5_dash[0].x = base_x + max_x*0.5;
    a5_dash[0].y = 550.0f;
    a5_dash[1].x = base_x + max_x*0.5;
    a5_dash[1].y = 600.0f;
    Vector2 a6_dash[2];
    a6_dash[0].x = base_x + max_x*0.6;
    a6_dash[0].y = 550.0f;
    a6_dash[1].x = base_x + max_x*0.6;
    a6_dash[1].y = 600.0f;
    Vector2 a7_dash[2];
    a7_dash[0].x = base_x + max_x*0.7;
    a7_dash[0].y = 550.0f;
    a7_dash[1].x = base_x + max_x*0.7;
    a7_dash[1].y = 600.0f;
    Vector2 a8_dash[2];
    a8_dash[0].x = base_x + max_x*0.8;
    a8_dash[0].y = 550.0f;
    a8_dash[1].x = base_x + max_x*0.8;
    a8_dash[1].y = 600.0f;
    Vector2 a9_dash[2];
    a9_dash[0].x = base_x + max_x*0.9;
    a9_dash[0].y = 550.0f;
    a9_dash[1].x = base_x + max_x*0.9;
    a9_dash[1].y = 600.0f;
    Vector2 a10_dash[2];
    a10_dash[0].x = base_x + max_x;
    a10_dash[0].y = 550.0f;
    a10_dash[1].x = base_x + max_x;
    a10_dash[1].y = 600.0f;



    /* 3. disply the graph: */
	int width = 800, height = 700;
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
            DrawSplineLinear(normalized_amplitude_graph, reduced_length, 1.0f, BLUE);
            DrawSplineLinear(normalized_freuquency_graph, new_len/2, 1.0f, BLUE);

            DrawSplineLinear(a0_dash, 2, 1.0f, RED);
            DrawSplineLinear(a1_dash, 2, 1.0f, RED);
            DrawSplineLinear(a2_dash, 2, 1.0f, RED);
            DrawSplineLinear(a3_dash, 2, 1.0f, RED);
            DrawSplineLinear(a4_dash, 2, 1.0f, RED);
            DrawSplineLinear(a5_dash, 2, 1.0f, RED);
            DrawSplineLinear(a6_dash, 2, 1.0f, RED);
            DrawSplineLinear(a7_dash, 2, 1.0f, RED);
            DrawSplineLinear(a8_dash, 2, 1.0f, RED);
            DrawSplineLinear(a9_dash, 2, 1.0f, RED);
            DrawSplineLinear(a10_dash, 2, 1.0f, RED);


			DrawText("A0", floor(a0_dash[1].x - max_x*0.05), floor(a0_dash[1].y+5), 20, BLACK);
			DrawText("A1", floor(a1_dash[1].x - max_x*0.05), floor(a1_dash[1].y+5), 20, BLACK);
			DrawText("A2", floor(a2_dash[1].x - max_x*0.05), floor(a2_dash[1].y+5), 20, BLACK);
			DrawText("A3", floor(a3_dash[1].x - max_x*0.05), floor(a3_dash[1].y+5), 20, BLACK);
			DrawText("A4", floor(a4_dash[1].x - max_x*0.05), floor(a4_dash[1].y+5), 20, BLACK);
			DrawText("A5", floor(a5_dash[1].x - max_x*0.05), floor(a5_dash[1].y+5), 20, BLACK);
			DrawText("A6", floor(a6_dash[1].x - max_x*0.05), floor(a6_dash[1].y+5), 20, BLACK);
			DrawText("A7", floor(a7_dash[1].x - max_x*0.05), floor(a7_dash[1].y+5), 20, BLACK);
			DrawText("A8", floor(a8_dash[1].x - max_x*0.05), floor(a8_dash[1].y+5), 20, BLACK);
			DrawText("A9", floor(a9_dash[1].x - max_x*0.05), floor(a9_dash[1].y+5), 20, BLACK);
			DrawText("A10", floor(a10_dash[1].x - max_x*0.05), floor(a10_dash[1].y+5), 20, BLACK);
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

























typedef struct color_buckets_t {
    float upper_bounds[255];
} color_buckets_t;

color_buckets_t* create_color_buckets(float* array, size_t len) {
    color_buckets_t* res = malloc(sizeof(color_buckets_t));
    float* array_copy = malloc(sizeof(float)*len);
    memcpy(array_copy, array, len*sizeof(float));
    quick_sort_float(array_copy, len);


    /* now make into unique set array: */
    float* set_array = malloc(sizeof(float)*len);
    float old = array_copy[0];
    set_array[0] = old;
    int curr = 0;
    for(int i = 0; i < len; i++) {
        if(array_copy[i] != old) set_array[++curr] = array_copy[i];
    }

    printf("we have: %i curr\n", curr);

    int step_size = curr/256;
    for(int i = 1; i < 256; i++) {
        res[0].upper_bounds[i-1] = set_array[step_size*i];
    }
    free(set_array);
    free(array_copy);
    return res;
}


uint8_t get_color_bucket_by_table(float val, color_buckets_t* table) {
    for(int i = 0; i < 255; i++) {
        if(val < table[0].upper_bounds[i]) return i;
        //printf("ub %i not checked: %f >= %f\n",i, val, table[0].upper_bounds[i]);
    }
    return 255;
}

uint8_t get_color_bucket(float val) {
    if(val < 0.00001f) { return 0; }
    else if(val < 0.0005f) { return 25; }
    else if(val < 0.001f) { return 50; }
    else if(val < 0.003f) { return 80; }
    else if(val < 0.005f) { return 100; }
    else if(val < 0.01f) { return 128; }
    else if(val < 0.03f) { return 150; }
    else if(val < 0.05f) { return 170; }
    else if(val < 0.7f) { return 200; }
    else { return 255; }
}

/* sanfierro's idea: sort array first, then assort buckets by percentiles / median! */



void spectrogram_file(char* path, int chosen_channel, int chosen_reduction_power, char* kind, char* new_path) {


    waveform_t testform = read_amplitude_data(path, chosen_channel);
    size_t new_len = truncate_power_of_2(testform.data_length) >> chosen_reduction_power;
    float* amplitudes = transform_to_complex_array(testform.amplitude_data, new_len);


    int bucket_nr = 4000;
    int height = bucket_nr;
    int width = 1024;
    uint8_t* red_numbers = malloc(height*width*sizeof(uint8_t));;
    uint8_t* green_numbers = malloc(height*width*sizeof(uint8_t));;
    uint8_t* blue_numbers = malloc(height*width*sizeof(uint8_t));;
    uint8_t* alpha_numbers = malloc(height*width*sizeof(uint8_t));;

    int length_of_segment = new_len/width;

    for(int segment = 0; segment < width; segment++) {


        float* frequencies = fft_power_of_two(&amplitudes[segment*length_of_segment*2], length_of_segment*2);
        float* combined_frequencies = compute_complex_absolute_values(frequencies, length_of_segment);
        float* normalized_frequencies = normalize_float_array(combined_frequencies, length_of_segment);


        float total_frequency = 0.0f;
        for(int i = 0; i < length_of_segment; i++) {
            total_frequency += normalized_frequencies[i];
        }



        float heatmap_base[bucket_nr];
        float heatmap_max[bucket_nr];
        for(int y = 0; y < bucket_nr; y++) {
            heatmap_base[y] = 0.0f;
            heatmap_max[y] = 0.0f;
            float octave_base = ((float)y)/(((float)bucket_nr)/10.0f);
            float octave_upper = octave_base+ (10/(float)(bucket_nr));
            float real_frequency_in_herz_base = pow(2.0f,octave_base)*27.5f;
            float real_frequency_in_herz_upper = pow(2.0f,octave_upper)*27.5f;
            int base_index = floor((real_frequency_in_herz_base/((float)testform.samples_per_second)) * ((float) length_of_segment));
            int upper_index = floor((real_frequency_in_herz_upper/((float)testform.samples_per_second)) * ((float) length_of_segment));
            for(int i = base_index; i < upper_index; i++) {
                heatmap_base[y] += combined_frequencies[i];
                heatmap_max[y] = (combined_frequencies[i] > heatmap_max[y]) ? combined_frequencies[i] : heatmap_max[y];
            }
            //printf("%i: %i-%i = %f (%f)\n", y, base_index, upper_index, heatmap_base[y], heatmap_base[y]/total_frequency);
        }



        float reduced_bucket_values[bucket_nr];
        for(int y = 0; y < bucket_nr; y++) {
            reduced_bucket_values[y] = heatmap_base[y]; //log2(fabs(heatmap_base[y]/total_frequency));
            //printf("%i:%f, base: %f\n", y, reduced_bucket_values[y], heatmap_base[y]);
        }

        float* normalized_bucket_numbers = normalize_float_array(reduced_bucket_values, bucket_nr);


        color_buckets_t* table = create_color_buckets(heatmap_base, bucket_nr);


        printf("print segment: %i\n", segment);
        int x = segment;
        for(int y = 0; y < height; y++) {
            red_numbers[y*width+x] =  get_color_bucket_by_table(heatmap_base[y], table); //get_color_bucket(normalized_bucket_numbers[y]); //ceil(255.0f*normalized_bucket_numbers[y]);
            green_numbers[y*width+x] = 0;
            blue_numbers[y*width+x] = 0;
            alpha_numbers[y*width+x] = 255;
            if(x == 0 && red_numbers[y*width+x] != 0) {
                //printf("%i: %i\n", y, red_numbers[y*width+x]);
            }
            //printf("px %i, %i = %i\n", x,y,y*width+x);
        }

        free(table);

    free(normalized_bucket_numbers);
    free(combined_frequencies);
    free(normalized_frequencies);
    free(frequencies);
    }


    write_bitmap_data("test.bmp", red_numbers, green_numbers, blue_numbers, alpha_numbers, width*height, width, height, 72.0f);
    // write_bitmap_data(char* file_name, uint8_t* red_data, uint8_t* green_data, uint8_t* blue_data, uint8_t* alpha_data, size_t len, int width, int height, float DPI)


    free(testform.amplitude_data);
    free(amplitudes);
}






void spectrogram_file_dblog(char* path, int chosen_channel, int chosen_reduction_power, char* kind, char* new_path) {


    waveform_t testform = read_amplitude_data(path, chosen_channel);
    size_t new_len = truncate_power_of_2(testform.data_length) >> chosen_reduction_power;
    float* amplitudes = transform_to_complex_array(testform.amplitude_data, new_len);


    int bucket_nr = 4000;
    int height = bucket_nr;
    int width = 1024;
    uint8_t* red_numbers = malloc(height*width*sizeof(uint8_t));;
    uint8_t* green_numbers = malloc(height*width*sizeof(uint8_t));;
    uint8_t* blue_numbers = malloc(height*width*sizeof(uint8_t));;
    uint8_t* alpha_numbers = malloc(height*width*sizeof(uint8_t));;

    int length_of_segment = new_len/width;

    for(int segment = 0; segment < width; segment++) {
        float* frequencies = fft_power_of_two(&amplitudes[segment*length_of_segment*2], length_of_segment*2);
        float* combined_frequencies = compute_complex_absolute_values(frequencies, length_of_segment);
        float* normalized_frequencies = normalize_float_array(combined_frequencies, length_of_segment);

        float total_frequency = 0.0f;
        for(int i = 0; i < length_of_segment; i++) {
            total_frequency += normalized_frequencies[i];
        }

        float heatmap_base[bucket_nr];
        for(int y = 0; y < bucket_nr; y++) {
            heatmap_base[y] = 0.0f;
            float octave_base = ((float)y)/(((float)bucket_nr)/10.0f);
            float octave_upper = octave_base+ (10/(float)(bucket_nr));
            float real_frequency_in_herz_base = pow(2.0f,octave_base)*27.5f;
            float real_frequency_in_herz_upper = pow(2.0f,octave_upper)*27.5f;
            int base_index = floor((real_frequency_in_herz_base/((float)testform.samples_per_second)) * ((float) length_of_segment));
            int upper_index = floor((real_frequency_in_herz_upper/((float)testform.samples_per_second)) * ((float) length_of_segment));
            for(int i = base_index; i < upper_index; i++) {
                heatmap_base[y] += 20*log10(combined_frequencies[i]);
            }
            //printf("%i: %i-%i = %f (%f)\n", y, base_index, upper_index, heatmap_base[y], heatmap_base[y]/total_frequency);
        }



        float reduced_bucket_values[bucket_nr];
        for(int y = 0; y < bucket_nr; y++) {
            reduced_bucket_values[y] = heatmap_base[y]; //log2(fabs(heatmap_base[y]/total_frequency));
            //printf("%i:%f, base: %f\n", y, reduced_bucket_values[y], heatmap_base[y]);
        }

        float* normalized_bucket_numbers = normalize_float_array(reduced_bucket_values, bucket_nr);


        color_buckets_t* table = create_color_buckets(heatmap_base, bucket_nr);


        printf("print segment: %i\n", segment);
        int x = segment;
        for(int y = 0; y < height; y++) {
            red_numbers[y*width+x] =  get_color_bucket_by_table(heatmap_base[y], table); //get_color_bucket(normalized_bucket_numbers[y]); //ceil(255.0f*normalized_bucket_numbers[y]);
            green_numbers[y*width+x] = 0;
            blue_numbers[y*width+x] = 0;
            alpha_numbers[y*width+x] = 255;
            if(x == 0 && red_numbers[y*width+x] != 0) {
                //printf("%i: %i\n", y, red_numbers[y*width+x]);
            }
            //printf("px %i, %i = %i\n", x,y,y*width+x);
        }

        free(table);

    free(normalized_bucket_numbers);
    free(combined_frequencies);
    free(normalized_frequencies);
    free(frequencies);
    }


    write_bitmap_data("test.bmp", red_numbers, green_numbers, blue_numbers, alpha_numbers, width*height, width, height, 72.0f);
    // write_bitmap_data(char* file_name, uint8_t* red_data, uint8_t* green_data, uint8_t* blue_data, uint8_t* alpha_data, size_t len, int width, int height, float DPI)


    free(testform.amplitude_data);
    free(amplitudes);
}


void write(char* path, int chosen_channel, int chosen_reduction_power, char* kind, char* new_path) {
    waveform_t testform = read_amplitude_data(path, chosen_channel);
    size_t new_len = truncate_power_of_2(testform.data_length) >> chosen_reduction_power;
    float* amplitudes = transform_to_complex_array(testform.amplitude_data, new_len);
    float* frequencies = fft_power_of_two(amplitudes, new_len * 2);
    float* combined_frequencies = compute_complex_absolute_values(frequencies, new_len);
    float* normalized_frequencies = normalize_float_array(combined_frequencies, new_len);

    if(strcmp(kind,"basic")==0) {
        FILE* fp = fopen(new_path, "w");
        for(int i = 0; i < new_len/2; i++) {
            fprintf(fp, "%f Hz: %f dB\n", ((float)testform.samples_per_second)*(((float)i)/((float)new_len)), 20*log10(normalized_frequencies[i]));
        }
        fclose(fp);
    }
    else if(strcmp(kind,"reduced")==0) {
        FILE* fp = fopen(new_path, "w");
        for(int i = 0; i < new_len/2; i++) {
            if(normalized_frequencies[i] == 0.0f) die("Zero!");
            float value = 20*log10(normalized_frequencies[i]);
            if (value > -20.0) fprintf(fp, "%f Hz: %f dB\n", ((float)testform.samples_per_second)*(((float)i)/((float)new_len)), value);
        }
        fclose(fp);
    }
    else {
        die("kind of operation not supported\n");
    }



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
        case 's':
            if(argc < 5) die("not enough arguments!\n");
            spectrogram(argv[2], atoi(argv[3]), atoi(argv[4]));
        break;
        case 'i':
            if(argc < 5) die("not enough arguments!\n");
            spectrogram_intensity(argv[2], atoi(argv[3]), atoi(argv[4]));
        break;

        case 'r':
            if(argc < 6) die("not enough arguments!\n");
            reverse_analyze(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
        break;

        case 'm':
            if(argc < 7) die("not enough arguments!\n");
            modify(argv[2], atoi(argv[3]), atoi(argv[4]), argv[5], argv[6]);
        break;

        case 'f':
            if(argc < 7) die("not enough arguments!\n");
            spectrogram_file(argv[2], atoi(argv[3]), atoi(argv[4]), argv[5], argv[6]);
        break;

        case 'y':
            if(argc < 7) die("not enough arguments!\n");
            spectrogram_file_dblog(argv[2], atoi(argv[3]), atoi(argv[4]), argv[5], argv[6]);
        break;



        case 'c': // change
            if(argc < 5) die("not enough arguments!\n");
            recognize_change_point(argv[2], atoi(argv[3]), atoi(argv[4]));
            break;
        case 'p': //pitch
            if(argc < 8) die("not enough arguments!\n");
            create_pitch_shifted_wave(argv[2], atoi(argv[3]), atoi(argv[4]), argv[5], argv[6], argv[7]);
            break;

        case 'w':
            if(argc < 7) die("not enough arguments!\n");
            write(argv[2], atoi(argv[3]), atoi(argv[4]), argv[5], argv[6]);
        break;



        case 'x':
            float testsort[12] = {0.0f, -1.345f, 134.4f, 23.0f, 2345.0f, -1234.0f, 0.0000001f, 0.0f, -234.000001f, -100000.0f, 1.000001f, 565.0f};
            quick_sort_float(testsort, 12);
            printf("array: %f %f %f %f %f %f %f %f %f %f %f %f", testsort[0], testsort[1], testsort[2], testsort[3], testsort[4], testsort[5], testsort[6], testsort[7], testsort[8], testsort[9], testsort[10], testsort[11]);
            break;
        case 't':

            matrix_t mat = create_matrix(3,3);
            set(mat,0,0, 1.23454f);
            set(mat,0,1, 2.23454f);
            set(mat,0,2, 1.2554f);
            set(mat,1,0, 145.23454f);
            set(mat,1,1, 0.23454f);
            set(mat,1,2, 1.245222f);
            set(mat,2,0, 1.0234f);
            set(mat,2,1, -7.23454f);
            set(mat,2,2, 14.23454f);
            matrix_t v = create_matrix(1,3);
            set(v,0,0, 24.0f);
            set(v,0,1, 0.2340f);
            set(v,0,2, 2.3450f);

            print_matrix(mat);
            print_matrix(v);

            matrix_t new_mat = gaussian(mat, v);
            print_matrix(new_mat);

            destroy_matrix(mat);
            destroy_matrix(v);
            destroy_matrix(new_mat);




            return 0;
            float test[2*16] = {112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234453453.735f, 0.0f, 1.000150f, 0.0f, 0.004545f, 0.0f};
            float *testfmid = fft_power_of_two(test, 2*16);
            float *testf = ifft_power_of_two(testfmid, 2*16);
            printf("freq: %f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n\n", testfmid[0], testfmid[1], testfmid[2], testfmid[3], testfmid[4], testfmid[5], testfmid[6], testfmid[7], testfmid[8], testfmid[9], testfmid[10], testfmid[11], testfmid[12], testfmid[13], testfmid[14], testfmid[15], testfmid[16], testfmid[17], testfmid[18], testfmid[19], testfmid[20], testfmid[21], testfmid[22], testfmid[23], testfmid[24], testfmid[25], testfmid[26], testfmid[27], testfmid[28], testfmid[29], testfmid[30], testfmid[31]);
            printf("reverse: %f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n", testf[0], testf[1], testf[2], testf[3], testf[4], testf[5], testf[6], testf[7], testf[8], testf[9], testf[10], testf[11], testf[12], testf[13], testf[14], testf[15], testf[16], testf[17], testf[18], testf[19], testf[20], testf[21], testf[22], testf[23], testf[24], testf[25], testf[26], testf[27], testf[28], testf[29], testf[30], testf[31]);
            free(testf);free(testfmid);
            float test2[2*16] = {112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234453453.735f, 0.0f, 1.000150f, 0.0f, 0.004545f, 0.0f};
                //112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234.735f, 0.0f, 15.0f, 0.0f, 0.004545f, 0.0f, 112.450f, 0.0f, 234453453.735f, 0.0f, 1.000150f, 0.0f, 0.004545f, 0.0f};
            float dest[2*16];
            fft_power_of_two_inplace(test2, dest, 2*16, false);
            printf("freq: %f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n%f, %f, %f, %f, %f, %f, %f, %f\n\n", dest[0], dest[1], dest[2], dest[3], dest[4], dest[5], dest[6], dest[7], dest[8], dest[9], dest[10], dest[11], dest[12], dest[13], dest[14], dest[15], dest[16], dest[17], dest[18], dest[19], dest[20], dest[21], dest[22], dest[23], dest[24], dest[25], dest[26], dest[27], dest[28], dest[29], dest[30], dest[31]);



            //4x4 image
            uint8_t reds[16] = {
                0, 0, 128, 255,
                15, 0, 48, 27,
                0, 0, 0, 0,
                0, 0, 0, 0
            };
            uint8_t greens[16] = {
                0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 128, 255,
                15, 0, 48, 27
            };
            uint8_t blues[16] = {
                0, 0, 128, 255,
                0, 0, 0, 0,
                15, 0, 48, 27,
                0, 0, 0, 0
            };
            uint8_t alphas[16] = {
                255, 255, 255, 255,
                255, 255, 255, 255,
                255, 255, 255, 255,
                255, 255, 255, 255
            };
            write_bitmap_data("test.bmp", reds, greens, blues, alphas, 16, 4, 4, 72.0f);
            // write_bitmap_data(char* file_name, uint8_t* red_data, uint8_t* green_data, uint8_t* blue_data, uint8_t* alpha_data, size_t len, int width, int height, float DPI)

//void fft_power_of_two_inplace(float* data, float* dest, size_t len, bool reverse)
        break;
    }
	return 0;
}
