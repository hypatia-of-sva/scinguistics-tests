#include "common.h"


int* calculate_peaks(float* data, size_t len, const char* filename) {
    assert(len > 4);

    FILE* fd = fopen(filename, "wb");
    bool rising = data[0] < data[1];
    for(int i = 2; i < len; i++) {
        if(rising && (data[i] < data[i-1])) {
            rising = false;
            fprintf(fd, "local maximum at %i: %f\n", i-1, data[i-1]);
        } else if(!rising && (data[i] > data[i-1])) {
            rising = true;
            fprintf(fd, "local minimum at %i: %f\n", i-1, data[i-1]);
        }
    }
    fclose(fd);


    return NULL;
}



int* calculate_peaks_dbl(double* data, size_t len, const char* filename) {
    assert(len > 4);

    FILE* fd = fopen(filename, "wb");
    bool rising = data[0] < data[1];
    for(int i = 2; i < len; i++) {
        if(rising && (data[i] < data[i-1])) {
            rising = false;
            fprintf(fd, "local maximum at %i: %f\n", i-1, data[i-1]);
        } else if(!rising && (data[i] > data[i-1])) {
            rising = true;
            fprintf(fd, "local minimum at %i: %f\n", i-1, data[i-1]);
        }
    }
    fclose(fd);


    return NULL;
}


