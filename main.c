/**************************************************************************
 *
 * xmega sound player
 * for xmega A1 xplained board
 * derived from app note AVR1520
 * plays a PCM sound 
 * On button press, DMA one shot is triggered
 * Transfers the sound buffer once per button press
 *
 ************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "dac_driver.h"
#include "dma_driver.h"
#include "sounds.h"
#include "board.h"

#define DMA_CHANNEL_0  &DMA.CH0

#define DMA_REPEAT_FOREVER 0

/* 2MHz clock internal clock */
#define TIMER_C0_PERIOD 300 //200

ISR(TCC0_OVF_vect) {
}

void DMA_Setup( DMA_CH_t *dmaChannel,
		const void *src,
		void *dest,
		int16_t blockSize,
		uint8_t repeatCount )
{
	DMA_Enable();

	DMA_SetupBlock(
			dmaChannel,
			src,
			DMA_CH_SRCRELOAD_BLOCK_gc,
			DMA_CH_SRCDIR_INC_gc,
			dest,
			DMA_CH_DESTRELOAD_BURST_gc,
			DMA_CH_DESTDIR_INC_gc,
			blockSize,
			DMA_CH_BURSTLEN_2BYTE_gc,
			repeatCount,
			false//no repeat
			);

	/* Timer Overflow will trigger DMA */
	DMA_SetTriggerSource( dmaChannel, 0x40 );
	DMA_EnableSingleShot( dmaChannel );
}

int main( void )
{
	/* First we have to enable the audio amplifier by setting PQ3 high. */
	PORTQ.PIN3CTRL = (PORTQ.PIN3CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	/* Configure switches */
	SWITCHPORTL.DIRCLR = 0x3F; /* Set port as input */

	/* Enable pull up to get a defined level on all switches */
	PORTCFG.MPCMASK = 0x3F;
	SWITCHPORTL.PIN0CTRL
		= (SWITCHPORTL.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;  
	                                                            
	PORTCFG.MPCMASK = 0x3F;
	SWITCHPORTL.PIN0CTRL |= PORT_INVEN_bm;  /* Inverted keys.. pressed = 1 */

	/* Set direction as output for LEDs */
	LEDPORT.DIR = 0xFF;

	/* Invert all pins on LED_PORT */
	PORTCFG.MPCMASK = 0xFF;
	LEDPORT.PIN0CTRL |= PORT_INVEN_bm;

	/* Enable overflow interrupt */
	TCC0.INTCTRLA
		= (TCC0.INTCTRLA & TC0_OVFINTLVL_gm) | TC_OVFINTLVL_MED_gc;

	DMA_Setup(
			DMA_CHANNEL_0,
			blaster,
			(void *)&DACB.CH0DATA,
			(sizeof(blaster) / sizeof(blaster[0])),
			DMA_REPEAT_FOREVER
			);

	DAC_SingleChannel_Enable(
			&DACB,
			DAC_REFSEL_AVCC_gc,
			true //left adust for 8b data
			);


	/* Enable medium interrupt level in PMIC and enable global interrupts. */
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;
	sei();

	TCC0.CTRLA = (TCC0.CTRLA & ~TC0_CLKSEL_gm) | TC_CLKSEL_DIV1_gc;
	TCC0.PER = TIMER_C0_PERIOD;

	while (1) {
		if (SWITCHPORTL.IN == 0x00) {

		} else {

			if (SWITCHPORTL.IN != 0x00) {

				DMA_EnableChannel( DMA_CHANNEL_0 );

				while (SWITCHPORTL.IN != 0x00) {
					LEDPORT.OUTSET = 0xFF;
				}
			}

			LEDPORT.OUT = 0x00;
		}
	}
}
