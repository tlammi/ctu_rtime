

#include <taskLib.h>
#include <stdio.h>
#include <kernelLib.h>
#include <semLib.h>
#include <intLib.h>
#include <iv.h>

#include <xlnx_zynq7k.h>

SEM_ID irc_sem;
volatile int irc_a, irc_b;

#define MOTOR_POSITION_MAX 100
int unsigned motor_position;

struct motorSignals {
	int a;
	int b;
};

enum motorDirection {
	DIRECTION_CW,
	DIRECTION_CCW
};

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

/*
 *  Enable IRQ
 *
 *  See TRM, 14.2.4 Interrupt Function (pg. 391, pg. 1348). Technical reference
 *  manual link is on rtime HW wiki: https://rtime.felk.cvut.cz/hw/index.php/Zynq
 */
void irc_init(void)
{
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000298) = 0x4; /* reset (stat) */
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000284) = 0x0; /* set as input (dirm) */
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x0000029c) = 0x4; /* rising edge (type) */
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x000002a0) = 0x0; /* rising edge (polarity) */
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x000002a4) = 0x0; /* rising edge (any) */
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000290) = 0x4; /* enable interrupt (en) GPIO2 */

        intConnect(INUM_TO_IVEC(INT_LVL_GPIO), irc_isr, 0);
        intEnable(INT_LVL_GPIO);
}

void irc_disable(void)
{
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000294) = 0x4; /* disable interrupt (dis) */

        intDisable(INT_LVL_GPIO);
        intDisconnect(INUM_TO_IVEC(INT_LVL_GPIO), irc_isr, 0);
}

/*
 * Entry point for DKM.
 */

void startMotorReader(void)
{
        TASK_ID st;

        irc_init();
        irc_sem = semCCreate(SEM_Q_FIFO, 0);
        st = taskSpawn("irc_st", 100, 0, 4096, (FUNCPTR) updateMotorPosition, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        printf("All is ready.\n");

        taskDelay(1000);
        printf("Out of play time.\n");

        irc_disable();
        taskDelete(st);
        
        printf("Motor reader done\n");
}

