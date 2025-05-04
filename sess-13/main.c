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
        dash[0].x = dia.base_diagram.base_x + dia.base_diagram.max_x*x_param;
        dash[0].y = dia.base_diagram.base_y;
        dash[1].x = dia.base_diagram.base_x + dia.base_diagram.max_x*x_param;
        dash[1].y = dia.base_diagram.base_y + 50.0f;
        DrawSplineLinear(dash, 2, 1.0f, BLUE);
        if(i != 0) {
            float mid_freq = (dia.formants[i-1] + dia.formants[i])/2;
            float x_param2 = log2(mid_freq/27.5)/10;
            dash[0].x = dia.base_diagram.base_x + dia.base_diagram.max_x*x_param2;
            dash[0].y = dia.base_diagram.base_y - dia.base_diagram.max_y - 50.0f;
            dash[1].x = dia.base_diagram.base_x + dia.base_diagram.max_x*x_param2;
            dash[1].y = dia.base_diagram.base_y;
            DrawSplineLinear(dash, 2, 1.0f, GREEN);
        }
    }

    for(int i = 0; i < 11; i++) {
        dash[0].x = dia.base_diagram.base_x + dia.base_diagram.max_x*(i*0.1);
        dash[0].y = dia.base_diagram.base_y;
        dash[1].x = dia.base_diagram.base_x + dia.base_diagram.max_x*(i*0.1);
        dash[1].y = dia.base_diagram.base_y + 50.0f;
        DrawSplineLinear(dash, 2, 1.0f, RED);
        char label[4];
        snprintf(label, 4, "A%i", i);
        DrawText(label, floor(dash[1].x - dia.base_diagram.max_x*0.05), floor(dash[1].y+5), 20, BLACK);
    }

}

void draw_string(display_string_t s) {
    DrawText(s.text, s.x, s.y, s.size, s.color);
}

void render(window_state_t state) {
    ClearBackground(RAYWHITE);
    for(int i = 0; i < state.nr_strings; i++) {
        draw_string(state.strings[i]);
    }
    draw_diagram(state.amplitude);
    draw_formant_diagram(state.log_frequency);
    draw_formant_diagram(state.linear_frequency);
}

window_state_t create_state(waveform_t snippet, int frequency_display_reduction_power, int chosen_reduction_power, char* snippet_name) {
    size_t len = truncate_power_of_2(snippet.data_length) >> chosen_reduction_power;
    float freq = ((float)snippet.samples_per_second);

    if(!is_power_of_2(len)) die("Data size collected not power of two!");

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
    size_t nr_formants = 2+(snippet.samples_per_second/1000);
    find_formants(snippet, nr_formants, &values, &bandwidths, true);

    float* formant_frequencies = calloc(nr_formants, sizeof(float));
    for(int i = 0; i < nr_formants; i++) {
        formant_frequencies[i] = get(values, 0, i);
    }
    quick_sort_float(formant_frequencies, nr_formants);

    size_t* search_indices = calloc(nr_formants, sizeof(size_t));
    for(int i = 0; i < nr_formants; i++) {
        search_indices[i] =  floor(((float)len)*(formant_frequencies[i]/freq));
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

    size_t actual_nr_formants = 0;
    float* formant_array = calloc(nr_formants, sizeof(float));
    for(int i = 0; i < nr_formants; i++) {
        if(should_be_drawn[i]) {
            formant_array[actual_nr_formants] = formant_frequencies[i];
            actual_nr_formants++;
        }
    }



    Vector2* normalized_amplitude_graph = create_graph_from_float_array(reduced_amplitudes, reduced_length, 15.0f, 350.0f, 350.0f, 200.0f);
    diagram_t amplitude = {15.0f, 350.0f, 350.0f, 200.0f, reduced_length, normalized_amplitude_graph, 1.0f, BLUE};

    float base_x = 415.0f, base_y = 550.0f, max_x = 350.0f, max_y = 400.0f;
    Vector2* normalized_frequency_graph = malloc(len*sizeof(Vector2));
    for(int i = 0; i < len; i++) {
        float frequency_in_herz = freq * (((float)i)/((float) len));
        float octave = log2(frequency_in_herz/27.5);
        float x_value = octave/10;
        normalized_frequency_graph[i].x = base_x + max_x*x_value;
        normalized_frequency_graph[i].y = base_y - max_y*normalized_frequencies[i];
    }
    formant_diagram_t log_freq = {{base_x, base_y, max_x, max_y, len/2, normalized_frequency_graph, 1.0f, BLUE},
        actual_nr_formants, formant_array};



    /* second linar graph */
    float lin_base_x = 850.0f;
    Vector2* normalized_linear_frequency_graph = create_graph_from_float_array(normalized_frequencies, (len >> frequency_display_reduction_power), lin_base_x, base_y, max_x, max_y);
    formant_diagram_t lin_freq = {{lin_base_x, base_y, max_x, max_y, (len >> frequency_display_reduction_power),
        normalized_linear_frequency_graph, 1.0f, BLUE}, actual_nr_formants, formant_array};


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



    state.amplitude = amplitude;
    state.log_frequency = log_freq;
    state.linear_frequency = lin_freq;


    return state;
}


#define CAPTURE_LEN 1024*8
bool capture_wave(ALCdevice* capture, waveform_t* form) {
static int16_t amp_buf[CAPTURE_LEN], tmp_buf[CAPTURE_LEN*2];
    form[0].samples_per_second = 44100;
    form[0].data_length = CAPTURE_LEN;
    form[0].amplitude_data = amp_buf;



    alcCaptureSamples(capture, tmp_buf, CAPTURE_LEN);
    if(alcGetError(capture) != ALC_NO_ERROR) return false;
    for(int i = 0; i < CAPTURE_LEN; i++) {
        form[0].amplitude_data[i] = tmp_buf[2*i];
    }

    return true;
}

int main(int argc, char** argv) {
    if(argc < 5) die("not enough arguments!\n");

    char* path = argv[1];
    int chosen_channel = atoi(argv[2]);
    int chosen_reduction_power = atoi(argv[3]);
    int frequency_display_reduction_power = atoi(argv[4]);

    init_formant_params();
    aladLoadAL();
    ALCdevice* capture = alcCaptureOpenDevice(NULL, 44100, AL_FORMAT_STEREO16, 1024*1024);
    if(capture == NULL) die("no mic found");

    bool recording = false;

    /*1. loading of data and FFT */
    waveform_t testform = read_amplitude_data(path, chosen_channel);

    window_state_t state = create_state(testform, frequency_display_reduction_power, chosen_reduction_power, path);



    int _width = 1500, _height = 700;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(_width, _height, "sound wave and fft tool");
    SetTargetFPS(30);

    while(!WindowShouldClose()) {
        if(IsKeyDown(KEY_Q) && !recording) {
            alcCaptureStart(capture);
            recording = true;
        }
        else if(IsKeyDown(KEY_W) && recording) {
            alcCaptureStop(capture);
            recording = false;
        }
        else if(IsKeyDown(KEY_E) && recording) {
            int16_t* buf = calloc(2*1024, sizeof(int16_t));
            alcCaptureSamples(capture, buf, 1024);
            for(int i = 0; i < 2048; i++) {
                printf("%i,", buf[i]);
            }
            free(buf);
        }
        else if(IsKeyDown(KEY_D) && recording) {
            waveform_t new_wave;
            if(capture_wave(capture, &new_wave)) {
                state = create_state(new_wave, 1, 0, "New sample");
            }
        }
        else if(IsKeyDown(KEY_A)) {
            alcCaptureStart(capture);
            sleep_us(CAPTURE_LEN*23);
            waveform_t new_wave;
            if(capture_wave(capture, &new_wave)) {
                state = create_state(new_wave, 1, 0, "New sample");
            }
            alcCaptureStop(capture);
        }

        else if(IsKeyDown(KEY_T) && recording) {
            g_params.filter_strength += 0.01f;
            printf("current strength = %f\n", g_params.filter_strength);
        }
        else if(IsKeyDown(KEY_Z) && recording) {
            g_params.filter_strength -= 0.01f;
            printf("current strength = %f\n", g_params.filter_strength);

        }

        BeginDrawing();
        render(state);
        EndDrawing();
    }
    CloseWindow();

    alcCaptureCloseDevice(capture);
    aladTerminate();

    return 0;
}
