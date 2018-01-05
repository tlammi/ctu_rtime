
#include <taskLib.h>
#include <stdint.h>

// In hz
#define PWM_FREQUENCY 20*1000
// In Mhz
#define PWM_CLOCK 100*1000*1000
// Number of periods of PWM_CLOCK
#define PWM_PERIOD (PWM_CLOCK) / (PWM_FREQUENCY)

#define PMOD2_BASE_ADDRESS 0x43c30000
#define CONTROL_REGISTER_OFFSET 0x0000
#define PWM_ENABLE_BIT 6
#define PWM_PERIOD_OFFSET 0x0008
#define PWM_PERIOD_MASK 0x3FFFFFFF
#define PWM_DUTY_CYCLE_OFFSET 0x000C
#define PWM_DUTY_MASK 0x3FFFFFFF
#define PWM_DUTY_DIR_A_BIT 30
#define PWM_DUTY_DIR_B_BIT 31

int unsigned current_position;
int unsigned desired_position = 50;

void setPwmDutyCycle(int unsigned const duty_cycle) {
	volatile uint32_t * duty_cycle_address = (volatile uint32_t *)(PMOD2_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET);
	*duty_cycle_address = ((*duty_cycle_address) & (~PWM_DUTY_MASK)) | (duty_cycle & PWM_DUTY_MASK);
}

void pwmInit() {

	// Set PWM period
	*(volatile uint32_t *)(PMOD2_BASE_ADDRESS + PWM_PERIOD_OFFSET) = PWM_PERIOD & PWM_PERIOD_MASK;
	// Change direction of DUTY_DIR_B
	*(volatile uint32_t *)(PMOD2_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET) |= ((unsigned)1<<PWM_DUTY_DIR_B_BIT);
	// Set duty cycle to 0
	setPwmDutyCycle(0);
	// Enable PWM
	*(volatile uint8_t *)(PMOD2_BASE_ADDRESS + CONTROL_REGISTER_OFFSET) |= (1<<PWM_ENABLE_BIT);
}

void disablePwm() {
	// Set PWM_ENABLE to 0
	*(volatile uint8_t *)(PMOD2_BASE_ADDRESS + CONTROL_REGISTER_OFFSET) &= ~(1<<PWM_ENABLE_BIT);
}

void printStatus() {
	int unsigned pwm_enabled = ((*(volatile uint8_t *)(PMOD2_BASE_ADDRESS + CONTROL_REGISTER_OFFSET) ) & (1<<PWM_ENABLE_BIT)) >> PWM_ENABLE_BIT;
	int unsigned pwm_duty_cycle = (*(volatile uint32_t *) (PMOD2_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET)) & PWM_DUTY_MASK;
	int unsigned pwm_period = (*(volatile uint32_t *) (PMOD2_BASE_ADDRESS + PWM_PERIOD_OFFSET)) & PWM_PERIOD_MASK;
	int unsigned pwm_duty_dir_a = ((*(volatile uint32_t *) (PMOD2_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET)) & (((unsigned)1)<<PWM_DUTY_DIR_A_BIT)) >> PWM_DUTY_DIR_A_BIT;
	int unsigned pwm_duty_dir_b = ((*(volatile uint32_t *) (PMOD2_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET)) & (((unsigned)1)<<PWM_DUTY_DIR_B_BIT)) >> PWM_DUTY_DIR_B_BIT;

	printf("pwm enabled: %d\n", pwm_enabled);
	printf("pwm duty cycle: %d\n", pwm_duty_cycle);
	printf("pwm_duty_dir_a: %d\n", pwm_duty_dir_a);
	printf("pwm_duty_dir_b: %d\n", pwm_duty_dir_b);
	printf("pwm period: %d\n", pwm_period);
}

int unsigned pidController() {
	int unsigned u = 0;
	int const P = 10000;
	int const I = 1;
	int const e = desired_position - current_position;
	
	u = P * e;
	
	return u;
}

void motorWriterTask() {

	while(1) {
		taskDelay(1);
		int unsigned pid_output = pidController();
		setPwmDutyCycle(pid_output);
	}
	
}

void startMotorWriter() {
	printf("Starting motor writer task\n");
	
	pwmInit();
	printStatus();
	
	TASK_ID motor_writer_task_id;
	
    motor_writer_task_id = taskSpawn("irc_st", 100, 0, 4096, (FUNCPTR) motorWriterTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	
	taskDelay(5*sysClkRateGet());
	disablePwm();
    taskDelete(motor_writer_task_id);
	printf("Motor writer done\n");
}
