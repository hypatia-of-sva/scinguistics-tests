#include "common.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

size_t filesize(FILE* fp) {
    struct stat buf;
    int fd = fileno(fp);
    fstat(fd, &buf);
    off_t size = buf.st_size;
    return size;
}

void die(char* str) {
	fprintf(stderr, str);
	exit(EXIT_FAILURE);
}

bool is_power_of_2(uint32_t x) {
    return x > 0 && !(x & (x-1));
}

uint32_t truncate_power_of_2(uint32_t x) {
    for(int bit = 31; bit > 0; bit--) {
        uint32_t pow = (1 << bit);
        if(x>=pow) return pow;
    }
    return 0;
}
