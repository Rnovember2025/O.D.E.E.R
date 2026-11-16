#include <stdint.h> 
#include <stddef.h>
#include "HotspotDetection.h"
#include "Thermal_Camera_Main.h"

void Detect_Hotspot(Frame *frame, Coordinate *hotspot_location) {
    uint16_t hotspot = 0;

    for (int i = 0; i < 62; i++) {
        for (int j = 0; j < 80; j++) {
            uint16_t temp = (*frame)[i][j];

            //identify hotspot                 
            if (temp > TARGET_K) {
                if (temp > hotspot){
                    hotspot = temp;
                    hotspot_location->row = j;
                    hotspot_location->column = i;
                } 
            }
        }
        
    }
}

