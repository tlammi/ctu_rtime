
#include <stdint.h>
#include <xlnx_zynq7k.h>

/*
 *  Enable IRQ
 *
 *  See TRM, 14.2.4 Interrupt Function (pg. 391, pg. 1348). Technical reference
 *  manual link is on rtime HW wiki: https://rtime.felk.cvut.cz/hw/index.php/Zynq
 */
void irc_init(void (*isr)(void))
{
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000298) = 0x4; /* reset (stat) */
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000284) = 0x0; /* set as input (dirm) */
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x0000029c) = 0x4; /* rising edge (type) */
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x000002a0) = 0x0; /* rising edge (polarity) */
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x000002a4) = 0x0; /* rising edge (any) */
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000290) = 0x4; /* enable interrupt (en) GPIO2 */

        intConnect(INUM_TO_IVEC(INT_LVL_GPIO), isr, 0);
        intEnable(INT_LVL_GPIO);
}

void irc_disable(void (*isr)(void))
{
        *(volatile uint32_t *) (ZYNQ7K_GPIO_BASE + 0x00000294) = 0x4; /* disable interrupt (dis) */

        intDisable(INT_LVL_GPIO);
        intDisconnect(INUM_TO_IVEC(INT_LVL_GPIO), isr, 0);
}


