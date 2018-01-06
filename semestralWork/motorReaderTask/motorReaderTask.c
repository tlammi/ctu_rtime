

#include <taskLib.h>
#include <stdio.h>
#include <kernelLib.h>
#include <semLib.h>
#include <intLib.h>
#include <iv.h>

#include <xlnx_zynq7k.h>

#include "mzapoBoard/irc.h"

#include "motor/utils.h"

SEM_ID irc_sem;
volatile int irc_a, irc_b;
int unsigned motor_position;

void incrementMotorPosition() {
	motor_position = (motor_position + 1) % MOTOR_POSITION_MAX;
}

void decrementMotorPosition() {
	if (motor_position == 0) {
		motor_position = 99;
	} else {
		--motor_position;
	}
}

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
                	incrementMotorPosition();
                } else {
                	decrementMotorPosition();
                }
                printf("Motor position: %d\n", motor_position);
        }
}

void irc_isr(void)
{
        int sr; /* status register */
        sr = *(volatile uint32_t *) (0x43c20000 + 0x0004);
        irc_a = (sr & 0x100) >> 8;
        irc_b = (sr & 0x200) >> 9;
        semGive(irc_sem);
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000298) = 0x4; /* reset (stat) */
}


void startMotorReader(void)
{
        TASK_ID st;

        irc_init(irc_isr);
        irc_sem = semCCreate(SEM_Q_FIFO, 0);
        st = taskSpawn("irc_st", 100, 0, 4096, (FUNCPTR) updateMotorPosition, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        printf("All is ready.\n");

        taskDelay(1000);
        printf("Out of play time.\n");

        irc_disable(irc_isr);
        taskDelete(st);
        
        printf("Motor reader done\n");
}

