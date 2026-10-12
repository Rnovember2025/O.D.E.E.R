#include <stdint.h> 
#include <stddef.h>

#define ROWS 80 
#define COLUMNS 62 
#define TARGET_K 312 
#define TOLERANCE_K 2  

typedef struct {
    uint8_t row;     
    uint8_t column; 
} Coordinate;  

static uint16_t temperatures[ROWS][COLUMNS]; 
static Coordinate matches[ROWS * COLUMNS];  
int main(void) {     

    while (1) {         // Update temperatures here by reading from your sensor.          
        size_t match_count = 0;          
        for (int row = 0; row < ROWS; row++) {             
            for (int column = 0; column < COLUMNS; column++) {                 
                uint16_t value = temperatures[row][column];                 
                uint16_t difference = value > TARGET_K                     
                    ? value - TARGET_K : TARGET_K - value;                  
                if (difference <= TOLERANCE_K) {                     
                    matches[match_count++] = (Coordinate){row, column};                 
                }             
            }         
        }          
        // matches[0] through matches[match_count - 1] are the coordinates         
        // found in this scan.          
        sleep_ms(50);  // Wait before scanning again.     
        } 
    }