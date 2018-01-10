
#ifndef _MOTOR_UTILS_H_
#define _MOTOR_UTILS_H_
#include "stdint.h"
#define MOTOR_POSITION_MAX 0xFFFFFFFFU
// #define MOTOR_POSITION_MAX 500

struct motorSignals {
	int a;
	int b;
};

enum motorDirection {
	DIRECTION_CW,
	DIRECTION_CCW
};

unsigned int grayToBinary(unsigned int num);

inline void incrementMotorPosition(int unsigned * motor_position);

inline void decrementMotorPosition(int unsigned * motor_position);

inline void motor_updatePosition(int * motor_position, int a, int b, int a_prev, int b_prev);


#endif // _MOTOR_UTILS_H_
