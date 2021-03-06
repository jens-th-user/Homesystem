
#include <avr/io.h>
#include <avr/interrupt.h>


#include "Types.h"

#include "Io.h"
#include "Spi.h"



/* USI port and pin definitions.
 */
//#define USI_OUT_REG	PORTB	//!< USI port output register.
//#define USI_IN_REG	PINB	//!< USI port input register.
//#define USI_DIR_REG	DDRB	//!< USI port direction register.
//#define USI_CLOCK_PIN	PB2	//!< USI clock I/O pin.
//#define USI_DATAIN_PIN	PB0	//!< USI data input pin.
//#define USI_DATAOUT_PIN	PB1	//!< USI data output pin.




/*! \brief  Data input register buffer.
 *
 *  Incoming bytes are stored in this byte until the next transfer is complete.
 *  This byte can be used the same way as the SPI data register in the native
 *  SPI module, which means that the byte must be read before the next transfer
 *  completes and overwrites the current value.
 */

static byte byReceivedUSIDR = 0;



/*! \brief  Initialize USI as SPI slave.
 *
 *  This function sets up all pin directions and module configurations.
 *  Use this function initially or when changing from master to slave mode.
 *  Note that the stored USIDR value is cleared.
 *
 *  \param spi_mode  Required SPI mode, must be 0 or 1.
 */

void Spi_SlaveInit( byte bySpiMode )
{
	// Configure port directions.
	//USI_DIR_REG |= (1<<USI_DATAOUT_PIN);                      // Outputs.
	//USI_DIR_REG &= ~(1<<USI_DATAIN_PIN) | (1<<USI_CLOCK_PIN); // Inputs.
	//USI_OUT_REG |= (1<<USI_DATAIN_PIN) | (1<<USI_CLOCK_PIN);  // Pull-ups.

	// Configure USI to 3-wire slave mode with overflow interrupt.
	USICR = (1<<USIOIE) | (1<<USIWM0) | (1<<USICS1) | (bySpiMode<<USICS0);
	
	byReceivedUSIDR = 0;
}


/*! \brief  Put one byte on bus.
 *
 *  Use this function like you would write to the SPDR register in the native SPI module.
 *  Calling this function in master mode starts a transfer, while in slave mode, a
 *  byte will be prepared for the next transfer initiated by the master device.
 *  If a transfer is in progress, this function will set the write collision flag
 *  and return without altering the data registers.
 *
 *  \returns  0 if a write collision occurred, 1 otherwise.
 */

byte Spi_Put( byte byVal )
{
	byte byRet = 1;
   
   // Put data in USI data register.
   USIDR = byVal;
   // Update flags and clear USI counter
   USISR = (1<<USIOIF);

	return byRet;
}


/*! \brief  Get one byte from bus.
 *
 *  This function only returns the previous stored USIDR value.
 *  The transfer complete flag is not checked. Use this function
 *  like you would read from the SPDR register in the native SPI module.
 */
byte Spi_Get( void )
{
	return byReceivedUSIDR;
}


/*! \brief  USI Timer Overflow Interrupt handler.
 *
 *  This handler disables the compare match interrupt if in master mode.
 *  When the USI counter overflows, a byte has been transferred, and we
 *  have to stop the timer tick.
 *  For all modes the USIDR contents are stored and flags are updated.
 */
ISR( USI_OVF_vect )
{
   // Update flags and clear USI counter
	USISR = (1<<USIOIF);
	// Copy USIDR to buffer to prevent overwrite on next transfer.
	byReceivedUSIDR = USIDR;
   
}
