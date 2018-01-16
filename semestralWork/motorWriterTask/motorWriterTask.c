
#include <taskLib.h>
#include <stdint.h>

#include <xlnx_zynq7k.h>

#include "motor/utils.h"
#include "mzapoBoard/pwm.h"
#include "mzapoBoard/irc.h"
#include "priorities.h"
#include "fifoBuffer.h"
#include "TCPInterface.h" //!< For updating values in webpage

#define ABS(A) (((A) > 0) ? (A) : (-(A)))

volatile int unsigned g_current_position = MOTOR_POSITION_MAX / 2;
volatile int unsigned g_desired_position = MOTOR_POSITION_MAX / 2;
volatile int unsigned g_pwm_duty_cycle;
volatile int64_t g_e;
FifoHandl g_motorWriterHandl;
FifoHandl g_tcpHandl;


volatile int g_irc_a, g_irc_b, g_irc_a_prev, g_irc_b_prev;

// In hz
#define PWM_FREQUENCY (20*1000)
#define PID_BUF_SIZE 10

SEM_ID sem_update_motor_position;

/*
int unsigned abs(int a) {
	return a > 0 ? a : -a;
}
*/	

int64_t calculatePositionDifference(int desired_position, int current_position) {
	/*
	int val1 = desired_position - (current_position - MOTOR_POSITION_MAX);
	int val2 = desired_position - current_position;
	if (abs(val1) > abs(val2)){
		return val2;
	} else {
		return val1;
	}
	*/
	int64_t e = desired_position - current_position;
	if ( e > MOTOR_POSITION_MAX/2 ) {
		e = e - MOTOR_POSITION_MAX;
	} else if ( e < -MOTOR_POSITION_MAX/2 ) {
		e = e + MOTOR_POSITION_MAX;
	}
	return e;
}

int unsigned pidController() {
	int64_t u = 0;
	int const P = 0;
	int const I = 1;
	int const I_div = 10;
	int64_t const e = calculatePositionDifference(g_desired_position, g_current_position);
	g_e = e;
	static int64_t e_int;
	e_int += (I * e);
	
	if ( e < 0LL ) {
		pwm_setPolarity(1);
	} else {
		pwm_setPolarity(-1);
	}
	
	if ( ABS(e) < 10 ) {
		u = 0;
		e_int = 0;
	} else {
		u = P * e + e_int / I_div;
		// Absolute value of u
		u = u > 0 ? u : -u;
		// Scale to avoid too high pwm
		u /= 1;
		// Set a cap on pwm speed to avoid problems
		int unsigned const u_max = 500;
		if (u > u_max) {
			u = u_max;
		}
	}
	return (int unsigned)u;
}

void motorWriterTask() {

	while(1) {
		taskDelay(sysClkRateGet() / 500);
		int unsigned pid_output = pidController();
		// printf("set duty cycle: %u\n", pid_output);
		g_pwm_duty_cycle = pid_output; // Update pwn duty cycle for TCP server
		pwm_setDutyCycle(pid_output);
		
		/*
		printf("Desired position: %d\n", g_desired_position);
		printf("Motor position: %d\n", g_current_position);
		printf("e: %d\n", g_e);
		printf("pid_output: %d\n", pid_output);
		*/
	}
	
}

void irc_isr(void)
{
        int sr; /* status register */
        sr = *(volatile uint32_t *) (PMOD_BASE_ADDRESS + 0x0004);
        g_irc_a = (sr & 0x100) >> 8;
        g_irc_b = (sr & 0x200) >> 9;
        
        // motor_updatePosition(&g_current_position, g_irc_a, g_irc_b, g_irc_a_prev, g_irc_b_prev);
        
        int unsigned motor_direction = (g_irc_a << 3) +
        		(g_irc_b << 2) +
        		(g_irc_a_prev << 1) +
        		(g_irc_b_prev << 0);
        
        switch (motor_direction) {
        	case 1:
        	case 7:
        	case 8:
        	case 14:
        		--g_current_position;
        		break;
        	case 2:
        	case 4:
        	case 11:
        	case 13:
        		++g_current_position;
        		break;
        }
        /*
    	if ((g_irc_b && !g_irc_a_prev) ||
    		(!g_irc_a && !g_irc_b_prev) ||
    		(!g_irc_b && g_irc_a_prev) ||
    		(g_irc_a && g_irc_b_prev)
    	) {
    		++g_current_position;
    	} else {
    		--g_current_position;
    	}
        */
        g_irc_a_prev = g_irc_a;
        g_irc_b_prev = g_irc_b;
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000298) = 0x4; /* reset (stat) */
}

/*
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
			incrementMotorPosition(&g_current_position);
		} else {
			decrementMotorPosition(&g_current_position);
		}
		// printf("Motor position: %u\n", current_position);
		// int unsigned pid_output = pidController();
		// pwm_setDutyCycle(pid_output);
		 
        // motor_updatePosition(&motor_position, a, b);
        // printf("Motor position: %d\n", motor_position);
	}
}
*/

void updateDesiredPosition() {
	while(1){
		// This function call blocks until data available
		g_desired_position = fifo_pop(g_motorWriterHandl);
		//printf("Writer read %d from buffer\n",g_desired_position);
	}
}

void sendDataToTcpBuffer() {
	while(1){
		taskDelay(sysClkRateGet()/2);
		printf("exec\n ");
		/*
		printf("Motor position:\t%u\n", g_current_position);
		printf("Desired position:\t%u\n", g_desired_position);
		printf("PWM duty cycle:\t%u\n", g_pwm_duty_cycle);
		
		printf("e: %lld\n", g_e);
		pwm_printStatus();
		*/
		/*
		fifo_push_nonblock(g_tcpHandl, g_current_position, NULL);
		fifo_push_nonblock(g_tcpHandl, g_desired_position, NULL);
		fifo_push_nonblock(g_tcpHandl, g_pwm_duty_cycle, NULL);
		*/
		GraphData data;
		data.actPos = g_current_position;
		data.reqPos = g_desired_position;
		data.pwmDuty = g_pwm_duty_cycle;
		
		TCP_pushGraphData(data);
	}
}

void startMotorWriter(FifoHandl motorWriterHandl, FifoHandl tcpHandl) {
	
	g_motorWriterHandl = motorWriterHandl;
	g_tcpHandl = tcpHandl;
	pwm_init(PWM_FREQUENCY);
	pwm_printStatus();
	
	irc_init(irc_isr);
	
	TASK_ID motor_writer_task_id;
    // TASK_ID update_motor_position_task_id;
    TASK_ID update_desired_position_task_id;
    TASK_ID send_data_to_tcp_buffer_task_id;
    sem_update_motor_position = semCCreate(SEM_Q_FIFO, 0);
	
    motor_writer_task_id = taskSpawn("motorWriterTask", PRIORITY_MOTOR_WRITER-1, 0, 4096, (FUNCPTR) motorWriterTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    // update_motor_position_task_id = taskSpawn("updateMotorPositionTask", PRIORITY_MOTOR_WRITER, 0, 4096, (FUNCPTR) updateMotorPosition, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    update_desired_position_task_id = taskSpawn("updateDesiredPositionTask", PRIORITY_MOTOR_WRITER, 0, 4096, (FUNCPTR) updateDesiredPosition, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	send_data_to_tcp_buffer_task_id = taskSpawn("sendDataToTcpBufferTask", PRIORITY_MOTOR_WRITER, 0, 4096, (FUNCPTR) sendDataToTcpBuffer, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    printf("Starting motor writer task\n");

	/*
	taskDelay(60*sysClkRateGet());
	irc_disable(irc_isr);
	pwm_disable();
    taskDelete(motor_writer_task_id);
    taskDelete(update_motor_position_task_id);
    taskDelete(update_desired_position_task_id);
	printf("Motor writer done\n");*/
}

