
#include <taskLib.h>
#include <stdint.h>

#include <xlnx_zynq7k.h>

#include "motor/utils.h"

int unsigned current_position;
int unsigned desired_position = 50;

volatile int irc_a, irc_b;

// In hz
#define PWM_FREQUENCY 20*1000

SEM_ID sem_update_motor_position;

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
		pwm_setDutyCycle(pid_output);
	}
	
}

void irc_isr(void)
{
        int sr; /* status register */
        sr = *(volatile uint32_t *) (0x43c30000 + 0x0004);
        irc_a = (sr & 0x100) >> 8;
        irc_b = (sr & 0x200) >> 9;
        semGive(sem_update_motor_position);
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000298) = 0x4; /* reset (stat) */
}

void updateMotorPosition() {
	struct motorSignals signals;
	struct motorSignals signals_previous;
	enum motorDirection motor_direction;
        while (1) {
                semTake(sem_update_motor_position, WAIT_FOREVER);
                // printf("a: %d, b: %d\n", irc_a, irc_b);
                signals.a = irc_a;
                signals.b = irc_b;
                motor_direction = findMotorDirection(&signals, &signals_previous);
                // printf("Motor direction: %d\n", motor_direction);
                signals_previous.a = signals.a;
                signals_previous.b = signals.b;
                
                if ( motor_direction == DIRECTION_CW ) {
                	incrementMotorPosition();
                } else {
                	decrementMotorPosition();
                }
                printf("Motor position: %d\n", current_position);
        }
}

void startMotorWriter() {
	printf("Starting motor writer task\n");
	
	pwmInit();
	// pwm_printStatus();
	
	irc_init(irc_isr);
	
	TASK_ID motor_writer_task_id;
    TASK_ID update_motor_position_task_id;
    sem_update_motor_position = semCCreate(SEM_Q_FIFO, 0);
	
    motor_writer_task_id = taskSpawn("motorWriterTask", 100, 0, 4096, (FUNCPTR) motorWriterTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    update_motor_position_task_id = taskSpawn("updateMotorPositionTask", 100, 0, 4096, (FUNCPTR) updateMotorPosition, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);


	
	taskDelay(5*sysClkRateGet());
	irc_disable(irc_isr);
	disablePwm();
    taskDelete(motor_writer_task_id);
    taskDelete(update_motor_position_task_id);
	printf("Motor writer done\n");
}

