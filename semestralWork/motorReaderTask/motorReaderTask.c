

#include <taskLib.h>
#include <stdio.h>
#include <kernelLib.h>
#include <semLib.h>
#include <intLib.h>
#include <iv.h>

#include <xlnx_zynq7k.h>

#include "mzapoBoard/irc.h"

#include "motor/utils.h"
#include "priorities.h"
#include "fifoBuffer.h"

SEM_ID irc_sem;
volatile int irc_a, irc_b;
volatile int unsigned motor_position;
FifoHandl g_udpHandl;


void updateMotorPosition(void)
{
	struct motorSignals signals;
	struct motorSignals signals_previous;
	enum motorDirection motor_direction;
        while (1) {
                semTake(irc_sem, WAIT_FOREVER);
                // printf("a: %d, b: %d\n", irc_a, irc_b);
                signals.a = irc_a;
                signals.b = irc_b;
                motor_direction = findMotorDirection(&signals, &signals_previous);
                // printf("Motor direction: %d\n", motor_direction);
                signals_previous.a = signals.a;
                signals_previous.b = signals.b;
                
                if ( motor_direction == DIRECTION_CW ) {
                	incrementMotorPosition(&motor_position);
                } else {
                	decrementMotorPosition(&motor_position);
                }
                printf("Motor position: %d\n", motor_position);
        }
}

void irc_isr(void)
{
        int sr; /* status register */
        sr = *(volatile uint32_t *) (PMOD_BASE_ADDRESS + 0x0004);
        irc_a = (sr & 0x100) >> 8;
        irc_b = (sr & 0x200) >> 9;
        semGive(irc_sem);
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000298) = 0x4; /* reset (stat) */
}

void sendMotorPosition() {
	while(1) {
		taskDelay(sysClkRateGet() / 10);
		printf("Read data to buffer: %d\n",motor_position);
		fifo_push_nonblock(g_udpHandl, motor_position, NULL);
	}
}

void startMotorReader(FifoHandl udpHandl)
{
		g_udpHandl = udpHandl;
        TASK_ID st, send_motor_position_task_id;

        irc_init(irc_isr);
        irc_sem = semCCreate(SEM_Q_FIFO, 0);
        st = taskSpawn("irc_st", PRIORITY_MOTOR_READER, 0, 4096, (FUNCPTR) updateMotorPosition, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        send_motor_position_task_id = taskSpawn("sendMotorPositionTask", PRIORITY_MOTOR_READER, 0, 4096, (FUNCPTR) sendMotorPosition, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

        printf("Motor reader starting.\n");

        taskDelay(60*sysClkRateGet());
        printf("Out of play time.\n");

        irc_disable(irc_isr);
        taskDelete(st);
        taskDelete(send_motor_position_task_id);
        
        printf("Motor reader done\n");
}

