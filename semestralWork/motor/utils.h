
#define MOTOR_POSITION_MAX 100

struct motorSignals {
	int a;
	int b;
};

enum motorDirection {
	DIRECTION_CW,
	DIRECTION_CCW
};

unsigned int grayToBinary(unsigned int num);
