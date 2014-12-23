#include "cereal.h"
#include <avr/io.h>

#define set_high( port, pin ) (port |=  (_BV(pin)))
#define set_low( port, pin )  (port &= ~(_BV(pin)))
#define set_alt( port, pin )  (port ^=  (_BV(pin)))
#define read_pin( port, pin ) ((port & (_BV(pin))) == 0 )

void delay_ms( uint16_t ms ){
	uint16_t time = 4000;
	volatile uint16_t i;

	while ( ms ){
		for ( i = 0; i < time; i++ );
		ms--;
	}
}

int main( void ){
	uint16_t i;
	unsigned state = 0;

	DDRB  |= _BV( PB1 );
	PORTB |= _BV( PB1 );

	INIT_DEBUG( );
	DEBUGF( "[%s] Hello, world! I'm at 0x%x\r\n", __func__, main );

	for ( i = 0;; i++ ){
		if ( state ){
			DEBUGF( "[%s] Turning LED off... %d iterations\r\n", __func__, i );
			set_low( PORTB, PB1 );

		} else {
			DEBUGF( "[%s] Turning LED on...  %d iterations\r\n", __func__, i );
			set_high( PORTB, PB1 );
		}

		state = !state;

#if ENABLE_INPUT
		DEBUGF( "[%s] character? > ", __func__ );
		DEBUGF( "got character \'0x%x\'\r\n", get_cereal_char( ));
#else
		delay_ms( 100 );
#endif

	}

	return 0;
}
