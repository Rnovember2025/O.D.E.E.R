#ifndef Main1_H
#define Main1_H

#include <stdio.h>
#include "MI48xx.h"
#include "pico/time.h"
#include "pico/multicore.h"

void thermal_camera_main(void);

typedef uint16_t Frame[62][80];
#endif