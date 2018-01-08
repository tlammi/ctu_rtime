
#include <stdint.h>


// In Mhz
#define PWM_CLOCK 100*1000*1000
// Number of periods of PWM_CLOCK

// #define PMOD_BASE_ADDRESS 0x43c30000
#define CONTROL_REGISTER_OFFSET 0x0000
#define PWM_ENABLE_BIT 6
#define PWM_PERIOD_OFFSET 0x0008
#define PWM_PERIOD_MASK 0x3FFFFFFF
#define PWM_DUTY_CYCLE_OFFSET 0x000C
#define PWM_DUTY_MASK 0x3FFFFFFF
#define PWM_DUTY_DIR_A_BIT 30
#define PWM_DUTY_DIR_B_BIT 31
#define PWM_DUTY_DIR_A_MASK ((1 << PWM_DUTY_DIR_A_BIT))
#define PWM_DUTY_DIR_B_MASK ((1 << PWM_DUTY_DIR_B_BIT))

void pwm_setDutyCycle(int unsigned const duty_cycle) {
	volatile uint32_t * duty_cycle_address = (volatile uint32_t *)(PMOD_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET);
	*duty_cycle_address = ((*duty_cycle_address) & (~PWM_DUTY_MASK)) | (duty_cycle & PWM_DUTY_MASK);
}

void setDirABit(int unsigned value) {
	volatile uint32_t * reg_address = (volatile uint32_t *)(PMOD_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET);
	uint32_t reg_value = *(volatile uint32_t *)(PMOD_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET);
	uint32_t new_reg_value = (reg_value & (~PWM_DUTY_DIR_A_MASK)) | (value << PWM_DUTY_DIR_A_BIT);
	*reg_address = new_reg_value;
}

void setDirBBit(int unsigned value) {
	volatile uint32_t * reg_address = (volatile uint32_t *)(PMOD_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET);
	uint32_t reg_value = *(volatile uint32_t *)(PMOD_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET);
	uint32_t new_reg_value = (reg_value & (~PWM_DUTY_DIR_B_MASK)) | (value << PWM_DUTY_DIR_B_BIT);
	*reg_address = new_reg_value;
}

void pwm_init(int unsigned pwm_frequency) {
	int unsigned pwm_period = PWM_CLOCK / pwm_frequency;
	// Set PWM period
	*(volatile uint32_t *)(PMOD_BASE_ADDRESS + PWM_PERIOD_OFFSET) = pwm_period & PWM_PERIOD_MASK;
	// Change direction of DUTY_DIR_B
	// *(volatile uint32_t *)(PMOD_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET) |= ((unsigned)1<<PWM_DUTY_DIR_B_BIT);
	setDirABit(1);
	setDirBBit(0);
	// Set duty cycle to 0
	pwm_setDutyCycle(0);
	// Enable PWM
	*(volatile uint8_t *)(PMOD_BASE_ADDRESS + CONTROL_REGISTER_OFFSET) |= (1<<PWM_ENABLE_BIT);
}

void pwm_disable() {
	// Set PWM_ENABLE to 0
	*(volatile uint8_t *)(PMOD_BASE_ADDRESS + CONTROL_REGISTER_OFFSET) &= ~(1<<PWM_ENABLE_BIT);
}

void pwm_setPolarity(int polarity) {
	if(polarity > 0) {
		setDirABit(1);
		setDirBBit(0);
	} else {
		setDirABit(0);
		setDirBBit(1);
	}
}

void pwm_printStatus() {
	int unsigned pwm_enabled = ((*(volatile uint8_t *)(PMOD_BASE_ADDRESS + CONTROL_REGISTER_OFFSET) ) & (1<<PWM_ENABLE_BIT)) >> PWM_ENABLE_BIT;
	int unsigned pwm_duty_cycle = (*(volatile uint32_t *) (PMOD_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET)) & PWM_DUTY_MASK;
	int unsigned pwm_period = (*(volatile uint32_t *) (PMOD_BASE_ADDRESS + PWM_PERIOD_OFFSET)) & PWM_PERIOD_MASK;
	int unsigned pwm_duty_dir_a = ((*(volatile uint32_t *) (PMOD_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET)) & (((unsigned)1)<<PWM_DUTY_DIR_A_BIT)) >> PWM_DUTY_DIR_A_BIT;
	int unsigned pwm_duty_dir_b = ((*(volatile uint32_t *) (PMOD_BASE_ADDRESS + PWM_DUTY_CYCLE_OFFSET)) & (((unsigned)1)<<PWM_DUTY_DIR_B_BIT)) >> PWM_DUTY_DIR_B_BIT;

	printf("pwm enabled: %d\n", pwm_enabled);
	printf("pwm duty cycle: %d\n", pwm_duty_cycle);
	printf("pwm_duty_dir_a: %d\n", pwm_duty_dir_a);
	printf("pwm_duty_dir_b: %d\n", pwm_duty_dir_b);
	printf("pwm period: %d\n", pwm_period);
}
