
#define MOTOR_POSITION_MAX 10000

struct motorSignals {
	int a;
	int b;
};

enum motorDirection {
	DIRECTION_CW,
	DIRECTION_CCW
};

unsigned int grayToBinary(unsigned int num);

void incrementMotorPosition(int unsigned * motor_position);

void decrementMotorPosition(int unsigned * motor_position);
