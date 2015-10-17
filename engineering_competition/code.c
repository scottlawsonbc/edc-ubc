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

// MANEUVER STATES
#define START_STATE 0
#define ACQUIRE_FIRST_CUBE 1
#define DELIVER_FIRST_CUBE 2
#define ACQUIRE_THIRD_CUBE 3
#define DELIVER_THIRDACQUIRE__CUBE 4
#define FINISHED_STATE 5

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
volatile int32_t PreviousLeftDetectsTape;
volatile int32_t RightDetectsTape;
volatile int32_t PreviousRightDetectsTape;
volatile int32_t BlockDetected;    /* HIGH = microswitch pressed */
volatile int32_t BlockGrabbed;     /* HIGH = microswitch pressed */
volatile int32_t IntersectionDetected;

/* Global Variables */
int32_t mState = 0;
int32_t blockAcquired = 0;
uint32_t initialSpeed = 100;
uint32_t nominalSpeed = 120;
uint32_t getOutOfCrossTime = 500;
uint32_t startDeadReckonTime = 500;
uint32_t windTime = 3500;
uint32_t lowerTime = 2000;

uint32_t intersectionNumberFirstCube = 0;
uint32_t intersectionNumberDeliverFirstCube = 0;
uint32_t intersectionNumberThirdCube = 0;
uint32_t intersectionNumberDeliverThirdCube = 0;



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
void Wind(int32_t speed, int32_t duration_ms);

/* Single increment of tape following at the given speed */
void TapeFollow(int32_t nominal_speed);

void Start();
void GetFirstCube();
void DeliverFirstCube();
void GetThirdCube();
void DeliverThirdCube();
void moveForward(int32_t MoveTime);
void moveBackward(int32_t MoveTime);
void stopMoving();
void lowerAndGrab();
void release();

void code_setup(void) 
{
}

void code_loop(void) {
	switch(mState) 
	{
		case START_STATE:
		Start();
		break;
		case ACQUIRE_FIRST_CUBE:
		GetFirstCube();
		break;
		case DELIVER_FIRST_CUBE:
		DeliverFirstCube();
		break;
		case ACQUIRE_THIRD_CUBE:
		GetThirdCube();
		break;
		case DELIVER_THIRD_CUBE:
		DeliverThirdCube();
		break;
		case FINISHED_STATE:

		break;
	}

}

void Start() {
	if(!IntersectionDetected) {
		LeftSpeed(initialSpeed);
		RightSpeed(initialSpeed);
	} else {
		LeftSpeed(0);
		RightSpeed(0);
		PrintToScreen("Found starting intersection \n");
		mState = FIRST_CUBE;
		return;
	}
}

void GetFirstCube() {
	
	// At cross in the box
	if(intersectionNumberFirstCube == 0) {
		// Get out of box if at intersection
		if(IntersectionDetected) 
		{
			moveForward(getOutOfCrossTime);
			PrintToScreen("Get Cube 1 Intersection %d", intersectionNumberFirstCube);
		}
		// Tape Follow Till next intersection
		else 
		{
			TapeFollow(nominalSpeed);
			if(IntersectionDetected) {
				intersectionNumberFirstCube++;
				PrintToScreen("Get Cube 1 Intersection %d", intersectionNumberFirstCube);		
			}
		}		
	} 
	else if(intersectionNumberFirstCube == 1) 
	{
		// turn right on first intersection
		// TODO: TurnRight, don't return till turned Right
		TurnRight();
		// TODO: do we need to move out of cross?
		// moveForward(getOutOfCrossTime);
		TapeFollow(nominalSpeed);
		if(IntersectionDetected) {
			intersectionNumberFirstCube++;
			PrintToScreen("Get Cube 1 Intersection %d", intersectionNumberFirstCube);
		}
	} 
	else if (intersectionNumberFirstCube == 2)
	{
		if(!BlockDetected || !blockAcquired) {
			TapeFollow(nominalSpeed/2);
		} else {
			stopMoving();
			// TODO: lowerAndGrab();
			lowerAndGrab();
			if(blockAcquired) {
				mState = DELIVER_FIRST_CUBE;
			}
		}			
	} 
	else
	{
		PrintToScreen("Cube 1: Intersections crossed is 0 or more than 2\n");
	}
}

void DeliverFirstCube() {
	// Starting from cube acquired, and on tape
	if(IntersectionDetected) {
			intersectionNumberDeliverFirstCube++;
	}

	if(intersectionNumberDeliverFirstCube == 0) 
	{
		// goback
		TurnRight();
	}
	else if(intersectionNumberDeliverFirstCube == 1) 
	{
		// TODO: TurnLeft, don't return till turned left
		TurnRight();
		TapeFollow(nominalSpeed);
	}
	else if(interectionNumberDeliverFirstCube == 2) {
		moveForward(getOutOfCrossTime/2);
		TapeFollow(nominalSpeed);
	}
	else if(interectionNumberDeliverFirstCube == 3) {
		TurnLeft();
		TapeFollow(nominalSpeed);
	}
	else if(interectionNumberDeliverFirstCube == 4) {
		TurnRight();
		TapeFollow(nominalSpeed);
	}
	else if(interectionNumberDeliverFirstCube == 5) {
		stopMoving();
		release();
		mState = ACQUIRE_THIRD_CUBE;
	}
}

void GetThirdCube() {

	if(intersectionNumberThirdCube == 0)
	{
		moveBackward(getOutOfCrossTime/2);
		TurnRight();
		TapeFollow(nominalSpeed);
		if(IntersectionDetected) {
			intersectionNumberThirdCube++;
			PrintToScreen("Get Cube 3 Intersection %d", intersectionNumberThirdCube);
		}
	}
	else if(intersectionNumberThirdCube == 1) 
	{
		TurnLeft();
		TapeFollow(nominalSpeed);
		if(IntersectionDetected) {
			intersectionNumberThirdCube++;
			PrintToScreen("Get Cube 3 Intersection %d", intersectionNumberThirdCube);
		}
	}
	else if(intersectionNumberThirdCube == 2)
	{
		// TODO: to tape follow for a set time or not
		if(!BlockDetected || !blockAcquired) {
			moveForward(250);
		} else {
			stopMoving();
			// TODO: lowerAndGrab();
			lowerAndGrab();
			if(blockAcquired) {
				mState = DELIVER_THIRD_CUBE;
			}
		}
	}
}

void DeliverThirdCube() {
	if(IntersectionDetected) {
			intersectionNumberDeliverThirdCube++;
	}

	if(intersectionNumberDeliverThirdCube == 0)
	{
		moveBackward(250);
		// TODO: figure out how long to move before tape follow back
	}
	else if(intersectionNumberDeliverThirdCube == 1)
	{
		// turn Around
		TurnLeft();
		TapeFollow(nominalSpeed);
	}
	else if(intersectionNumberDeliverThirdCube == 2)
	{
		TurnRight();
		TapeFollow(nominalSpeed);
	}
	else if(intersectionNumberDeliverThirdCube == 3)
	{
		stopMoving();
		release();
		mState = FINISHED_STATE;
	}
}

void moveForward(int32_t MoveTime) {
	LeftSpeed(initialSpeed);
	RightSpeed(initialSpeed);
	Wait(MoveTime);
	LeftSpeed(0);
	RightSpeed(0);
}

void moveBackward(int32_t MoveTime) {
	LeftSpeed(-initialSpeed);
	RightSpeed(-initialSpeed);
	Wait(MoveTime);
	LeftSpeed(0);
	RightSpeed(0);
}

void stopMoving() {
	LeftSpeed(0);
	RightSpeed(0);
}


void release() {
	Wind(-nominalSpeed, windTime);
	blockAcquired = 0;
}

void lowerAndGrab() {
	// run till crane detects block and winding is finished
	while(BlockGrabbed != 1) {
		Raise(-nominalSpeed, 250);
	}
	Wind(nominalSpeed, windTime);
	blockAcquired = 1;
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
