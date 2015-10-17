#include "Main.h"
#include "API.h"
#include <stdint.h>

void setup(void) {
    
    // define pins
    uint_32 left_tape_pin = 0;
    uint_32 right_tape_pin = 1;
    uint_32 block_pin = 1;
}

void loop(void) {

// Read from inputs
uint_32 left_tape = readAnalogInput(left_tape_pin);
uint_32 right_tape = readAnalogInput(right_tape_pin);
boolean isAcquired = readDigitalInput(block_pin);

// states

// start


// ------ first block --------

// RL corner --> move straight
// right corner --> turn right
// left corner --> half speed
// block detected --> grab
// back up till left corner --> turn left
// first right corner --> pass
// RL corner --> turn left
// right corner --> turn right
// RL corner --> release block


// ------- second block -------

// back up till right corner
// turn right
// right corner --> turn right
// left corner --> turn left
// no tape --> move 1/4 speed
// block detected
// back up
// RL corner -- turn left
// move
// RL corner --> turn left
// right corner --> turn right
// RL corner --> release block

// ------- third block -------

// back up till right corner
// turn right
// right corner --> go straight
// no tape --> 1/4 speed
// block detected --> grab
// back up 
// right corner --> pass
// back up
// left corner --> turn left
// RL corner --> release block
}

// functions
void move(uint_32 left, uint_32 right);
void turn_right(uint_32 left, uint_32 right);
void turn_left(uint_32 left, uint_32 right);
void back(uint_32 left, uint_32 right)



