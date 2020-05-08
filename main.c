/* MN15439A simple controller code for STM8L, No PWM (Connect the SIN1 SIN2 SIN3 of the display together)
 * Coded by TinLethax 2020/05/03 +7
 */
#include <stdint.h>
#include <stm8l.h>
#include <delay.h>

/* Define your pinout (MUST BE FAST MODE COMPATIBLE !!) 
 * If Changed, You must change the bset and bres inline assembly pin 
 * For example __asm__("bset 0x5005, #3); >> __asm__("BitSet(to1) Px_ODR address, #PinNumer); Logic 1
 * __asm__("bres 0x5005, #3); >> __asm__("BitReset(to0) Px_ODR address, #PinNumer); Logic 0
 */

#define BLK 3	// Display blanking on PB3
#define LAT 4	// Latching on PB4
#define CLK 5	// Clock line on PB5
#define Mo  6	// Master out on PB6

//Dummy bytes ; Format (MSB)[a,b,c,d,e,f,0,0](LSB) 

char img1[30]={
0xAA,0xAA,0xAA,0xAA,0xAA,

0xAA,0xAA,0xAA,0xAA,0xAA,

0xAA,0xAA,0xAA,0xAA,0xAA,

0xAA,0xAA,0xAA,0xAA,0xAA,

0xAA,0xAA,0xAA,0xAA,0xAA,

0xAA,0xAA,0xAA,0xAA,0xA8};

uint8_t reOrder[2][6]={
{0,2,0,3,0,4}, /* the bit shift to convert def to the afbecd format */
{7,0,6,0,5,0} /* the bit shift to convert abc to the afbecd format */
}; 

// GPIOs initialization, Super Fast 10MHz instead 2MHz
void initGPIOs(){
	PB_DDR |= (1 << BLK) | (1 << LAT) | (1 << CLK) | (1 << Mo); // set all pin as output
	PB_CR1 |= (1 << BLK) | (1 << LAT) | (1 << CLK) | (1 << Mo); // by using Push-pull mode
	PB_CR2 |= (1 << BLK) | (1 << LAT) | (1 << CLK) | (1 << Mo); // That Super fast at 10MHz max speed
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
void LBB(uint8_t Grid,char *rowdata[30]){
// Logically thinking : Determine the Grid is Event or Odd number (Important, For the simple algoithm to convert abcdef to afbecd format).
	uint8_t EvOd = 0;
	if(Grid%2){
	EvOd = 1;// event number (event grid), Only manipulate the d, e, f Dots
	}else{	
	EvOd = 0;// odd number (odd grid), Only manipulate the a, b, c Dots
	}
// Initial phase : Start communication
	__asm__("bset 0x5005, #3"); // Display blank, Set PB3 to 1
	__asm__("bset 0x5005, #4"); // Hold Latch for little while, Set PB4 to 1
	__asm__("nop");
	__asm__("bres 0x5005, #4"); // release Latch, Set PB4 to 0
	__asm__("bres 0x5005, #3"); // blank is done, Set PB3 to 0

// Bitmap phase : send the 234 bit of bitmap data
	for (int i=0;i< 29;i++){

	for (int a=0;a< 6;a++){

	if((uintptr_t)rowdata & (1 << reOrder[EvOd][a])){ //sending the data
	__asm__("bset 0x5005, #6");// set PB5 to 1	
	}else{
	__asm__("bres 0x5005, #6");// Set PB5 to 0	
	}// if((uintptr_t)rowdata & (1 << reOrder[EvOd][a]))
	__asm__("bset 0x5005, #5"); // _/
	__asm__("bres 0x5005, #5"); // \_
	__asm__("nop");
	__asm__("nop");
	}// for (int a=0;a< 6;a++)

	*rowdata++; // move to the next data in the array
	}// for (int i=0;i< 29;i++)

	*rowdata++;// move to the next data in the array

	for (int i=0;i< 2;i++){

	if((uintptr_t)rowdata & (1 << reOrder[EvOd][i])){ // The Unused Grid will be turn off
	__asm__("bset 0x5005, #6");// set PB5 to 1	
	}else{
	__asm__("bres 0x5005, #6");// Set PB5 to 0	
	}
	__asm__("bset 0x5005, #5"); // _/
	__asm__("bres 0x5005, #5"); // \_
	__asm__("nop");
	__asm__("nop");
	}// for (int i=0;i< 2;i++)

// Grid Activation phase 1a : Check if We use the Grid 52, If yes, Grid 1 must be activated
	if (Grid == 52){

// in case that Grid is reaching the end (Grid 53) the First Grid (1) must be activated 
	__asm__("bset 0x5005, #6"); // Grid 1 must be activated 	
	__asm__("bset 0x5005, #5"); // _/
	__asm__("bres 0x5005, #5"); // \_
	__asm__("nop");
	__asm__("nop");

// Grid Activation phase 2a : Disable the rest unused Grid
	for (int i=0;i<50;i++){
	__asm__("bres 0x5005, #6"); // The Unused Grid will be turn off
	__asm__("bset 0x5005, #5"); // _/
	__asm__("bres 0x5005, #5"); // \_
	__asm__("nop");
	__asm__("nop");
	}// for (int i=0;i<50;i++)

// Grid Activation phase 3a : Enable the last Grid
	__asm__("bset 0x5005, #6"); // activate Grid 52 	
	__asm__("bset 0x5005, #5"); // _/
	__asm__("bres 0x5005, #5"); // \_
	__asm__("nop");
	__asm__("nop");

	}else{

// Grid Activation phase 1b : Disable unused grid
	for (int i=1;i<Grid;i++){
	__asm__("bres 0x5005, #6"); // The Unused Grid will be turn off
	__asm__("bset 0x5005, #5"); // _/
	__asm__("bres 0x5005, #5"); // \_
	__asm__("nop");
	__asm__("nop");
	}

// Grid Activation phase 2b : Only turn the Grid N and N+1 on by send 1 two times
	for (int i=0; i < 2; i++){
	__asm__("bset 0x5005, #6"); // two pair will be activated	
	__asm__("bset 0x5005, #5"); // _/
	__asm__("bres 0x5005, #5"); // \_
	__asm__("nop");
	__asm__("nop");
	}

// Grid Activation phase 3 : Disable the rest unused Grid
	for (int i=Grid+1;i<52;i++){
	__asm__("bres 0x5005, #6"); // The Unused Grid will be turn off
	__asm__("bset 0x5005, #5"); // _/
	__asm__("bres 0x5005, #5"); // \_
	__asm__("nop");
	__asm__("nop");
	}

	}// if (Grid == 52)
	__asm__("bres 0x5005, #6"); // The Unused Grid will be turn off
}
void main() {
	CLK_CKDIVR = 0x00; // Keep the clock at 16MHz with no clock divider
	initGPIOs(); // Init all pin that we want at Super fast 10MHz
 	while (1) {
     // Still working on it dude.
	for (int i=1;i < 53;i++){
	LBB(i,img1);	
	}
    }
}
