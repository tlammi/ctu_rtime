
#include "./utils.h"

unsigned int grayToBinary(unsigned int num)
{
    unsigned int mask = num;
    while (mask != 0)
    {
        mask = mask >> 1;
        num = num ^ mask;
    }
    return num;
}

enum motorDirection findMotorDirection(struct motorSignals const * const current,
		struct motorSignals const * const previous) {
	
	int unsigned val_gray_current = (current->a << 1) + current->b;
	int unsigned val_gray_previous = (previous->a << 1) + previous->b;
	int unsigned val_binary_current = grayToBinary(val_gray_current);
	int unsigned val_binary_previous = grayToBinary(val_gray_previous);
	// printf("current: %u, prev: %u", val_binary_current, val_binary_previous);
	int val_binary_diff = (unsigned)val_binary_current - val_binary_previous;
	// Edge cases
	if (val_binary_diff == 3) {
		return DIRECTION_CW;
	}
	if (val_binary_diff == -3) {
		return DIRECTION_CCW;
	}
	
	if ( val_binary_diff == -1) {
		return DIRECTION_CW;
	} else {
		return DIRECTION_CCW;
	}
}

inline void incrementMotorPosition(int unsigned * motor_position) {
	*motor_position = (*motor_position + 1) % MOTOR_POSITION_MAX;
}

inline void decrementMotorPosition(int unsigned * motor_position) {
	if (*motor_position == 0) {
		*motor_position = MOTOR_POSITION_MAX - 1;
	} else {
		--(*motor_position);
	}
}
