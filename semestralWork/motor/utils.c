
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

inline enum motorDirection findDirection(int a, int b, int a_prev, int b_prev) {
	if ((b && !a_prev) ||
		(!a && !b_prev) ||
		(!b && a_prev) ||
		(a && b_prev)
	) {
		return DIRECTION_CCW;
	} else {
		return DIRECTION_CW;
	}
}

inline void motor_updatePosition(int * motor_position, int a, int b, int a_prev, int b_prev){
	struct motorSignals signals;
	struct motorSignals signals_previous;
	enum motorDirection motor_direction;
    // while (1) {
                // semTake(irc_sem, WAIT_FOREVER);
                // printf("a: %d, b: %d\n", irc_a, irc_b);
                /*
                signals.a = irc_a;
                signals.b = irc_b;
                motor_direction = findMotorDirection(&signals, &signals_previous);
                // printf("Motor direction: %d\n", motor_direction);
                signals_previous.a = signals.a;
                signals_previous.b = signals.b;
                */
                motor_direction = findDirection(a, b, a_prev, b_prev);
                // a_prev = a;
                // b_prev = b;
                if ( motor_direction == DIRECTION_CW ) {
                	incrementMotorPosition(motor_position);
                } else {
                	decrementMotorPosition(motor_position);
                }
                // printf("Motor position: %d\n", motor_position);
        // }
}

/*
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
*/
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
