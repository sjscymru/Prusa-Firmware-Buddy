#include <device/peripherals.h>
#include "random.h"

bool random32bit(uint32_t *random) {

    if (HAL_RNG_GenerateRandomNumber(&hrng, random) != HAL_OK) {
        return false;
    }
    return true;
}
