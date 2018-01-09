
#include <taskLib.h>
#include <stdint.h>

#include <xlnx_zynq7k.h>

#include "motor/utils.h"
#include "mzapoBoard/pwm.h"
#include "mzapoBoard/irc.h"
#include "priorities.h"
#include "fifoBuffer/fifoBuffer.h"

volatile int unsigned current_position;
volatile int unsigned desired_position = 250;
volatile int unsigned pwm_duty_cycle;
FifoHandl g_motorWriterHandl;
FifoHandl g_tcpHandl;


volatile int irc_a, irc_b;

// In hz
#define PWM_FREQUENCY (20*1000)
#define PID_BUF_SIZE 10

SEM_ID sem_update_motor_position;

/*
int unsigned abs(int a) {
	return a > 0 ? a : -a;
}
*/

int calculatePositionDifference(int desired_position, int current_position) {
	int val1 = desired_position - (current_position - MOTOR_POSITION_MAX);
	int val2 = desired_position - current_position;
	if (abs(val1) > abs(val2)){
		return val2;
	} else {
		return val1;
	}
}

int unsigned pidController() {
	// static int current_index;
	// static int e_array[PID_BUF_SIZE];
	int u = 0;
	int const P = 110;
	int const I = 5;
	int const I_div = 1;
	// int const e = (int)desired_position - (int)current_position;
	int const e = calculatePositionDifference(desired_position, current_position);
	static int e_int;
	e_int += (I * e)/I_div;
	// printf("error: %d\n", e);
	
	if ( e < 0) {
		pwm_setPolarity(-1);
	} else {
		pwm_setPolarity(1);
	}
	
	// e_array[current_index] = e;
	// current_index = (current_index + 1) % PID_BUF_SIZE;
	u = P * e + e_int;
	u = (u > 0) ? u : -u;
	
	if ( abs(e) < 10 ) {
		u = 0;
	}
	
	/*
	int i;
	for (i = 0; i < PID_BUF_SIZE; ++i) {
		u += I * e_array[i];
	}
	*/
	/*
	printf("e: %d\n", e);
	printf("Current position: %u\n", current_position);
	printf("u: %u\n",u );
	*/
	printf("%u\n", current_position);
	return u / 100;
}

void motorWriterTask() {

	while(1) {
		taskDelay(5);
		int unsigned pid_output = pidController();
		// printf("set duty cycle: %u\n", pid_output);
		pwm_setDutyCycle(pid_output);
	}
	
}

void irc_isr(void)
{
        int sr; /* status register */
        sr = *(volatile uint32_t *) (PMOD_BASE_ADDRESS + 0x0004);
        irc_a = (sr & 0x100) >> 8;
        irc_b = (sr & 0x200) >> 9;
        semGive(sem_update_motor_position);
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000298) = 0x4; /* reset (stat) */
}

void updateMotorPosition() {
	struct motorSignals signals;
	struct motorSignals signals_previous;
	enum motorDirection motor_direction;
	printf("updateMotorPosition started\n");
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
			incrementMotorPosition(&current_position);
		} else {
			decrementMotorPosition(&current_position);
		}
		// printf("Motor position: %u\n", current_position);
		// int unsigned pid_output = pidController();
		// pwm_setDutyCycle(pid_output);
	}
}

void updateDesiredPosition() {
	desired_position = fifo_pop(g_motorWriterHandl);
	printf("Writer read %d from buffer\n",desired_position);
}

void sendDataToTcpBuffer() {
	taskDelay(sysClkRateGet() / 2);
	fifo_push_nonblock(g_tcpHandl, current_position, NULL);
	fifo_push_nonblock(g_tcpHandl, desired_position, NULL);
	fifo_push_nonblock(g_tcpHandl, pwm_duty_cycle, NULL);
}

void startMotorWriter(FifoHandl motorWriterHandl, FifoHandl tcpHandl) {
	
	g_motorWriterHandl = motorWriterHandl;
	g_tcpHandl = tcpHandl;
	pwm_init(PWM_FREQUENCY);
	pwm_printStatus();
	
	irc_init(irc_isr);
	
	TASK_ID motor_writer_task_id;
    TASK_ID update_motor_position_task_id;
    TASK_ID update_desired_position_task_id;
    TASK_ID send_data_to_tcp_buffer_task_id;
    sem_update_motor_position = semCCreate(SEM_Q_FIFO, 0);
	
    motor_writer_task_id = taskSpawn("motorWriterTask", PRIORITY_MOTOR_WRITER, 0, 4096, (FUNCPTR) motorWriterTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    update_motor_position_task_id = taskSpawn("updateMotorPositionTask", PRIORITY_MOTOR_WRITER, 0, 4096, (FUNCPTR) updateMotorPosition, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    update_desired_position_task_id = taskSpawn("updateDesiredPositionTask", PRIORITY_MOTOR_WRITER, 0, 4096, (FUNCPTR) updateDesiredPosition, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	send_data_to_tcp_buffer_task_id = taskSpawn("sendDataToTcpBufferTask", PRIORITY_MOTOR_WRITER, 0, 4096, (FUNCPTR) sendDataToTcpBuffer, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    printf("Starting motor writer task\n");

	
	taskDelay(60*sysClkRateGet());
	irc_disable(irc_isr);
	pwm_disable();
    taskDelete(motor_writer_task_id);
    taskDelete(update_motor_position_task_id);
    taskDelete(update_desired_position_task_id);
	printf("Motor writer done\n");
}

