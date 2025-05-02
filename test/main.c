#include "common.h"


/* sanfierro's idea: sort array first, then assort buckets by percentiles / median! */


typedef Vector2 pair_t[2];


typedef struct {
   float base_x, base_y, max_x, max_y;
   size_t nr_of_points;
   Vector2* data;
   float thickness;
   Color color;
} diagram_t;


typedef struct {
   diagram_t base_diagram;
   size_t nr_formants;
   float* formants;
   bool* should_be_drawn;
} formant_diagram_t;

typedef struct {
	char* text;
   int x, y, size;
   Color color;
} display_string_t;

typedef struct {
	size_t nr_strings;
	display_string_t* strings;

   diagram_t amplitude;
   formant_diagram_t log_frequency;
   formant_diagram_t linear_frequency;
} window_state_t;



void draw_diagram(diagram_t dia) {
   DrawSplineLinear(dia.data, dia.nr_of_points, dia.thickness, dia.color);
}
void draw_formant_diagram(formant_diagram_t dia) {
   pair_t dash;
   draw_diagram(dia.base_diagram);

   for(int i = 0; i < dia.nr_formants; i++) {
   	float x_param = log2(dia.formants[i]/27.5)/10;
      dash[0].x = base_x + max_x*x_param;
      dash[0].y = base_y;
      dash[1].x = base_x + max_x*x_param;
      dash[1].y = base_y + 50.0f;
      DrawSplineLinear(dash, 2, 1.0f, BLUE);
      if(i != 0) {
           float mid_freq = (dia.formants[i-1] + dia.formants[i])/2;
           float x_param2 = log2(mid_freq/27.5)/10;
           dash[i][0].x = base_x + max_x*x_param2;
           dash[i][0].y = base_y - max_y - 50.0f;
           dash[i][1].x = base_x + max_x*x_param2;
           dash[i][1].y = base_y;
			  DrawSplineLinear(dash, 2, 1.0f, GREEN);
   	}
   }

   for(int i = 0; i < 11; i++) {
       dashes[0].x = base_x + max_x*(i*0.1);
       dashes[0].y = base_y;
       dashes[1].x = base_x + max_x*(i*0.1);
       dashes[1].y = base_y + 50.0f;
       DrawSplineLinear(dash, 2, 1.0f, RED);
       char label[4];
       snprintf(label, 4, "A%i", i);
       DrawText(label, floor(dash[1].x - max_x*0.05), floor(dash[1].y+5), 20, BLACK);
   }

}

void draw_string(display_string_t s) {
   DrawText(s.text, s.x, s.y, s.size, s.color);
} 

void render(window_state_t state) {
   BeginDrawing();
       ClearBackground(RAYWHITE);
		for(int i = 0; i < state.nr_strings; i++) {
         draw_string(state.strings[i]);
      }
		draw_diagram(state.amplitude);
      draw_formant_diagram(state.log_frequency);
      draw_formant_diagram(state.linear_frequency);
   EndDrawing();
}

window_state_t create_state(waveform_t snippet, int frequency_display_reduction_power, const char* snippet_name) {
   size_t len = snippet.data_length;
   float freq = ((float)snippet.samples_per_second);
   
   if(!power_pf_two(len)) die("Data size collected not power of two!");
   
   float* amplitudes = transform_to_complex_array(snippet.amplitude_data, len);
   float* frequencies = fft_power_of_two(amplitudes, len*2);
   float* combined_frequencies = compute_complex_absolute_values(frequencies, len);
   float* normalized_amplitudes = normalize_int_array(snippet.amplitude_data, len);
   float* normalized_frequencies = normalize_float_array(combined_frequencies, len);

   int16_t max_abs_amplitude = 0;
   int max_amp_index = 0;
   maximum_abs_value_and_position_int_array(snippet.amplitude_data, len, &max_amp_index, &max_abs_amplitude);
   char* max_amplitude_string = malloc(80);
   snprintf(max_amplitude_string, 80, "Maximum Amplitude: %.6f \n(index %i)", (float) max_abs_amplitude, max_amp_index);

   float max_combined_freq = 0.0f;
   int max_freq_index = 0;
   maximum_abs_value_and_position_float_array(frequencies, len, &max_freq_index, &max_combined_freq);
   char* max_frequency_string = malloc(80);
   snprintf(max_frequency_string, 80, "Maximum Frequency: %.6f \n(index %i)", max_combined_freq, max_freq_index);

   char* duration_string = malloc(40);
   float actual_play_duration = duration(snippet);
   snprintf(duration_string, 40, "Duration: %.6fs\nSamples/s: %i", actual_play_duration, snippet.samples_per_second);

   int reduced_length = (len > 4096 ? 4096 : len);
   int reduction_factor = len/reduced_length;

   float* reduced_amplitudes = malloc(reduced_length*sizeof(float));
   for(int i = 0; i < reduced_length; i++) {
       reduced_amplitudes[i] = normalized_amplitudes[i*reduction_factor];
   }

   matrix_t values, bandwidths;
   size_t nr_formants = 2+(testform.samples_per_second/1000);
   find_formants(testform, nr_formants, &values, &bandwidths, true);

   Vector2* normalized_amplitude_graph = create_graph_from_float_array(reduced_amplitudes, reduced_length, 15.0f, 350.0f, 350.0f, 200.0f);

   float base_x = 415.0f, base_y = 550.0f, max_x = 350.0f, max_y = 400.0f;
   Vector2* normalized_freuquency_graph = malloc(len*sizeof(Vector2));
   for(int i = 0; i < len; i++) {
       float frequency_in_herz = freq * (((float)i)/((float) len));
       float octave = log2(frequency_in_herz/27.5);
       float x_value = octave/10;
       normalized_freuquency_graph[i].x = base_x + max_x*x_value;
       normalized_freuquency_graph[i].y = base_y - max_y*normalized_frequencies[i];
   }

   float* formant_frequencies = calloc(nr_formants, sizeof(float));
   for(int i = 0; i < nr_formants; i++) {
       formant_frequencies[i] = get(values, 0, i);
   }
   quick_sort_float(formant_frequencies, nr_formants);

   size_t* search_indices = calloc(nr_formants, sizeof(size_t));
   for(int i = 0; i < nr_formants; i++) {
       search_indices[i] =  floor(((float)len)*(formant_frequencies[i]/freq));
   }

   bool* should_be_drawn = calloc(nr_formants, sizeof(bool));
   for(int i = 0; i < nr_formants; i++) {
       size_t min_index = (i == 0 ? 0 : search_indices[i-1]), max_index = (i == nr_formants-1 ? len/2 : search_indices[i+1]);
       size_t nr_of_indices = 0;
       float total = 0.0f;
       for(size_t index = min_index; index < max_index; index++) {
           nr_of_indices++;
           total += normalized_frequencies[index];
       }
       float average = total / nr_of_indices;
       should_be_drawn[i] = 1 + normalized_frequencies[search_indices[i]] > average;
       //printf("Is the %i-th formant %f > the average (%f): %i\n", i, formant_frequencies[i], average, shoud_be_drawn[i]);
   }


   /* second linar graph */
   float lin_base_x = 850.0f;
   Vector2* normalized_linear_freuquency_graph = create_graph_from_float_array(normalized_frequencies, (len >> frequency_display_reduction_power), lin_base_x, base_y, max_x, max_y);



	window_state_t state;
   state.nr_strings = 8;
   state.strings = calloc(8,sizeof(display_string_t));
	display_string_t stack_strings[8] = {   
           {"Analyzing Snippet: ", 15, 20, 20, BLACK},
           {snippet_name, 200, 20, 20, BLACK},
           {duration_string, 415, 20, 20, BLACK},
           {"Amplitudes", 15, 70, 20, BLACK},
           {"Frequencies", 415, 70, 20, BLACK},
           {max_amplitude_string, 15, 90, 20, BLACK},
           {max_frequency_string, 415, 90, 20, BLACK},
           {"Linear Frequencies", 865, 70, 20, BLACK}
   };
   memmove(state.strings, stack_strings, sizeof(stack_strings));
   
   
}

int main(int argc, char** argv) {

   if(argc < 5) die("not enough arguments!\n");

   char* path = argv[1];
   int chosen_channel = atoi(argv[2]);
   int chosen_reduction_power = atoi(argv[3]);
   int frequency_display_reduction_power = atoi(argv[4]);




   /*1. loading of data and FFT */
   waveform_t testform = read_amplitude_data(path, chosen_channel);

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


   printf("float-formants \n");
   matrix_t values, bandwidths;
   size_t nr_formants = 2+(testform.samples_per_second/1000);
   find_formants(testform, nr_formants, &values, &bandwidths, true);







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



   float* formant_frequencies = calloc(nr_formants, sizeof(float));
   for(int i = 0; i < nr_formants; i++) {
       formant_frequencies[i] = get(values, 0, i);

       size_t search_index = floor(
           ((float)new_len)*
           (formant_frequencies[i]/
               (
                   ((float)testform.samples_per_second)
               )
           )
       );
       if(formant_frequencies[i] != 0.0)
       printf("formant %i at %f Hz; absolute FFT value = %f %%\n", i, formant_frequencies[i], normalized_frequencies[search_index]*100);
   }

   quick_sort_float(formant_frequencies, nr_formants);

   size_t* search_indices = calloc(nr_formants, sizeof(size_t));
   for(int i = 0; i < nr_formants; i++) {
       search_indices[i] =  floor(
           ((float)new_len)*
           (formant_frequencies[i]/
               (
                   ((float)testform.samples_per_second)
               )
           )
       );
   }

   pair_t* formant_dashes = calloc(nr_formants, sizeof(pair_t));
   pair_t* mid_formant_dashes = calloc(nr_formants, sizeof(pair_t));
   bool* shoud_be_drawn = calloc(nr_formants, sizeof(bool));
   for(int i = 0; i < nr_formants; i++) {
       float x_param = log2(formant_frequencies[i]/27.5)/10;
       formant_dashes[i][0].x = base_x + max_x*x_param;
       formant_dashes[i][0].y = 550.0f;
       formant_dashes[i][1].x = base_x + max_x*x_param;
       formant_dashes[i][1].y = 600.0f;

       if(i < nr_formants-1) {
           float mid_freq = (formant_frequencies[i] + formant_frequencies[i+1])/2;
           float x_param2 = log2(mid_freq/27.5)/10;
           mid_formant_dashes[i][0].x = base_x + max_x*x_param2;
           mid_formant_dashes[i][0].y = 100.0f;
           mid_formant_dashes[i][1].x = base_x + max_x*x_param2;
           mid_formant_dashes[i][1].y = 550.0f;
       }

       size_t min_index = (i == 0 ? 0 : search_indices[i-1]), max_index = (i == nr_formants-1 ? new_len/2 : search_indices[i+1]);
       printf("chosen to search between %li and %li;", min_index, max_index);
       size_t nr_of_indices = 0;
       float total = 0.0f;
       for(size_t index = min_index; index < max_index; index++) {
           nr_of_indices++;
           total += normalized_frequencies[index];
       }
       float average = total / nr_of_indices;
       shoud_be_drawn[i] = 1 + normalized_frequencies[search_indices[i]] > average;
       printf("Is the %i-th formant %f > the average (%f): %i\n", i, formant_frequencies[i], average, shoud_be_drawn[i]);
   }




   pair_t a_dashes[11];
   for(int i = 0; i < 11; i++) {
       a_dashes[i][0].x = base_x + max_x*(i*0.1);
       a_dashes[i][0].y = 550.0f;
       a_dashes[i][1].x = base_x + max_x*(i*0.1);
       a_dashes[i][1].y = 600.0f;
   }








   /* second linar graph */
   float lin_base_x = 850.0f;
   printf("Reduced Length: %li\n\n\n\n", (new_len >> frequency_display_reduction_power));
   Vector2* normalized_linear_freuquency_graph = create_graph_from_float_array(normalized_frequencies, (new_len >> frequency_display_reduction_power), lin_base_x, base_y, max_x, max_y);


   pair_t* lin_formant_dashes = calloc(nr_formants, sizeof(pair_t));
   for(int i = 0; i < nr_formants; i++) {
       float formant_in_hz = get(values, 0, i);
       float x_param = (formant_in_hz * (float)(1<<frequency_display_reduction_power)) / ((float)testform.samples_per_second);
       //float x_param = (formant_in_hz - 27.6)/((1<<10)*27.5);
       lin_formant_dashes[i][0].x = lin_base_x + max_x*x_param;
       lin_formant_dashes[i][0].y = 550.0f;
       lin_formant_dashes[i][1].x = lin_base_x + max_x*x_param;
       lin_formant_dashes[i][1].y = 600.0f;
   }

   pair_t lin_a_dashes[11];
   for(int i = 0; i < 11; i++) {
       float a_freq = 27.5*(1<<i);
       float x_param = (a_freq * (float)(1<<frequency_display_reduction_power)) / ((float)testform.samples_per_second);
       lin_a_dashes[i][0].x = lin_base_x + max_x*x_param;
       lin_a_dashes[i][0].y = 550.0f;
       lin_a_dashes[i][1].x = lin_base_x + max_x*x_param;
       lin_a_dashes[i][1].y = 600.0f;
   }







   /* 3. disply the graph: */
   int width = 1500, height = 700;
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


           for(int i = 0; i < nr_formants; i++) {
               if(shoud_be_drawn[i])
                   DrawSplineLinear(formant_dashes[i], 2, 1.0f, BLUE);
           }
           for(int i = 0; i < nr_formants-1; i++) {
               if(shoud_be_drawn[i])
                   DrawSplineLinear(mid_formant_dashes[i], 2, 1.0f, GREEN);
           }

           for(int i = 0; i < 11; i++) {
               DrawSplineLinear(a_dashes[i], 2, 1.0f, RED);
               char label[100];
               snprintf(label, 100, "A%i", i);
               DrawText(label, floor(a_dashes[i][1].x - max_x*0.05), floor(a_dashes[i][1].y+5), 20, BLACK);
           }



           DrawSplineLinear(normalized_linear_freuquency_graph, (new_len >> frequency_display_reduction_power), 1.0f, BLUE);


           DrawText("Linear Frequencies", 865, 70, 20, BLACK);
           for(int i = 0; i < nr_formants; i++) {
               if(shoud_be_drawn[i] == 1)
               DrawSplineLinear(lin_formant_dashes[i], 2, 1.0f, BLUE);
           }

           for(int i = 0; i < 11; i++) {
               DrawSplineLinear(lin_a_dashes[i], 2, 1.0f, RED);
               char label[100];
               snprintf(label, 100, "A%i", i);
               DrawText(label, floor(lin_a_dashes[i][1].x - max_x*0.05), floor(lin_a_dashes[i][1].y+5), 20, BLACK);
           }

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
   return 0;
}