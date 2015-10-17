#include "Main.h"
#include "API.h"
#include <stdint.h>

/* Sensor pin definitions */
#define LEFT_SENSOR_PIN  1
#define RIGHT_SENSOR_PIN 2
#define BLOCK_DETECT_PIN 3
#define GRAB_DETECT_PIN  4

/* Motor pin definitions */
#define LEFT_MOTOR_PIN  1
#define RIGHT_MOTOR_PIN 2
#define LIFT_MOTOR_PIN  3
#define GRAB_MOTOR_PIN  4

/* If sensor > DarkThreshold, then sensor is on tape */
int32_t LeftDarkThreshold  = 0;
int32_t RightDarkThreshold = 0;

/* Motor speed parameters */
/* Motor functions will limit speeds to these values */
/* All values below are ABSOLUTE VALUE */
int32_t MaxMoveSpeed = 0;
int32_t MaxLiftSpeed = 0;
int32_t MaxGrabSpeed = 0;
int32_t MinMoveSpeed = 0;
int32_t MinLiftSpeed = 0;
int32_t MinGrabSpeed = 0;

/* PID gain values */
float GainP = 0;
float GainI = 0;
float GainD = 0;
float IntegralErrorMax = 0;

/* PID error values */
float PIDCurrentError = 0;
float PIDPreviousError = 0;

/* Raw analog sensor values */
volatile int32_t LeftAnalogRaw;
volatile int32_t RightAnalogRaw;

/* Sensor states */
volatile int32_t LeftDetectsTape;
volatile int32_t RightDetectsTape;
volatile int32_t BlockDetected;    /* HIGH = microswitch pressed */
volatile int32_t BlockGrabbed;     /* HIGH = microswitch pressed */

/* Error > 0: Too left */
/* Error < 0: Too right */
volatile int32_t CurrentTapeError;
volatile int32_t PreviousTapeError;

/* Interrupt calls this function to update sensors */

/* Speed control of left and right motors) */
void LeftSpeed(int32_t speed);
void RightSpeed(int32_t speed);

/* Raise or lower the grabbing arm */
/* speed: How quickly to raise/lower arm. Positive speed = RAISE. Negative speed = LOWER */
/* duration_ms: Duration to move the arm in milliseconds */
void Raise(int32_t speed, int32_t duration_ms);

/* Pull or unwind the string */
/* speed: How quickly to raise/lower arm. Positive speed = WIND. Negative speed = UNWIND */
/* duration_ms: Duration to move the arm in milliseconds */
void Grab(int32_t speed, int32_t duration_ms);

/* Single increment of tape following at the given speed */
void TapeFollow(int32_t nominal_speed);

void code_setup(void) 
{
}

void code_loop(void) 
{
}

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


