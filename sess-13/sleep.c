#include <unistd.h>
#include <stdint.h>

void sleep_us(uint32_t us) {
    usleep(us);
}
