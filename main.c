/* MN15439A simple controller code for STM8L, No PWM (Connect the SIN1 SIN2 SIN3 of the display together)
 * Coded by TinLethax 2020/05/03 +7
 */
#include <stdint.h>
#include <stm8l.h>
#include <delay.h>

/* Define your pinout (MUST BE FAST MODE COMPATIBLE !!) */

#define BLK 3	// Display blanking on PB3
#define LAT 4	// Latching on PB4
#define CLK 5	// Clock line on PB5
#define Mo  6	// Master out on PB6


// GPIOs initialization, Super Fast 10MHz instead 2MHz
void initGPIOs(){
	PB_DDR = (1 << BLK) | (1 << LAT) | (1 << CLK) | (1 << Mo); // set all pin as output
	PB_CR1 = (1 << BLK) | (1 << LAT) | (1 << CLK) | (1 << Mo); // by using Push-pull mode
	PB_CR2 = (1 << BLK) | (1 << LAT) | (1 << CLK) | (1 << Mo); // That Super fast at 10MHz max speed
}

//Very Long 286 bit bitbanging 

/* first 234 bit is divided into 39 part, each part contain 6 bit of dot data 
 * [1:1a,1f,1b,1e,1c,1d] [2:2a,2f,2b,2e,2c,2d] ..... [39:39a,39f,39b,39e,39c,39d] ; [Row Number:Column a, f, b, e, c, d]
 * The rest 52 bit (238 to 286) is the grid control bits 
 * [G1:235] [G2:236] ..... [G52:286] ; [Grid number : bit position]
 */

/* uint8_t Grid is the current Grid number for displaying certain region of the display, The active grid will be in the form N and N+1 by N is between 1 and 53
 * char *rowdata[] is the array that pointed to the actuall memory parts contain the Display bitmap
 */    
void LBB(uint8_t Grid,char *rowdat[]){
// Initial phase : Start communication
	PB_ODR = (1 << BLK); // Display blank
	PB_ODR = (1 << LAT); // Hold Latch for little while
	PB_ODR = (0 << BLK); // blank is done
	PB_ODR = (0 << LAT); // release Latch

// Bitmap phase : send the 234 bit of bitmap data
	for (int i=0;i< 29;i++){
	for (int a=7;a> -1;a--){
	PB_ODR = (((uintptr_t)rowdata & (1 << a)) << Mo); // The Unused Grid will be turn off
	PB_ODR = (1 << CLK); // _/
	PB_ODR = (0 << CLK); // \_
	}
	*rowdata++;
	}
	*rowdata++;
	for (int i=7;i > 5;i--){
	PB_ODR = (((uintptr_t)rowdata & (1 << i)) << Mo); // The Unused Grid will be turn off
	PB_ODR = (1 << CLK); // _/
	PB_ODR = (0 << CLK); // \_
	}

// Grid Activation phase 1 : Disable unused grid
	for (int i=1;i<Grid;i++){
	PB_ODR = (0 << Mo); // The Unused Grid will be turn off
	PB_ODR = (1 << CLK); // _/
	PB_ODR = (0 << CLK); // \_
	}
// Grid Activation phase 2 : Only turn the Grid N and N+1 on
	for (int i=0; i < 2; i++){
	PB_ODR = (1 << Mo); // two pair will be activated	
	PB_ODR = (1 << CLK); // _/
	PB_ODR = (0 << CLK); // \_
	}

// Grid Activation phase 3 : Disable the rest unused Grid
	for (int i=Grid;i<53;i++){
	PB_ODR = (0 << Mo); // The Unused Grid will be turn off
	PB_ODR = (1 << CLK); // _/
	PB_ODR = (0 << CLK); // \_
	}
	if (Grid == 53){// in case that Grid is reaching the end (Grid 53) the First Grid (1) must be activated 
	PB_ODR = (1 << Mo); // Grid 1 must be activated 	
	PB_ODR = (1 << CLK); // _/
	PB_ODR = (0 << CLK); // \_
	}
}


void main() {
	CLK_CKDIVR = 0x00; // Keep the clock at 16MHz with no clock divider
	initGPIOs(); // Init all pin that we want at Super fast 10MHz
    while (1) {
     // Still working on it dude.
    }
}
