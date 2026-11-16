#ifndef HotspotDetection_h
#define HotspotDetection_h
#define TARGET_K 3000

#include "Thermal_Camera_Main.h"

typedef struct {
    uint8_t row;     
    uint8_t column; 
} Coordinate;

void Detect_Hotspot(Frame *frame, Coordinate *hotspot_location);
#endif