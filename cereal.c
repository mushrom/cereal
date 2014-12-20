#include "cereal.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdarg.h>

// compile things only if debugging is enabled
#if DEBUG

#define set_high( port, pin ) (port |=  (_BV(pin)))
#define set_low( port, pin )  (port &= ~(_BV(pin)))
#define set_alt( port, pin )  (port ^=  (_BV(pin)))
#define read_pin( port, pin ) ((port & (_BV(pin))) == 0 )

volatile uint16_t cur_char = 0;
volatile uint8_t bitpos = 0;

void init_cereal( void ){
	cli( );

	// Set prescaler to 64
	TCCR0B |= _BV(CS01) | _BV(CS00);

	/*
	 * Set timer frequency. Exploits the fact that uart can tolerate slight
	 * deviations in frequency.
	 *
	 * rough frequency translations (that work):
	 * 103 -> 1201.92 hz -> 1200 baud, the most reliable
	 * 51  -> 2403.84 hz -> 2400 baud
	 * 25  -> 4807.69 hz -> 4800 baud
	 * 12  -> 9615.38 hz -> 9600 baud, sometimes works, but choose a lower rate if it prints garbage.
	 *                                 Seems to depend on optimization settings.
	 */
	OCR0A = BAUD_RATE;

	// enable CTC mode
	TCCR0B |= _BV(WGM02);

	// enable timer0a interrupt
	TIMSK  |= _BV(OCIE0A);

	// Set up output pin
	DDRB  |= _BV( CEREAL_PIN ) ;
	PORTB |= _BV( CEREAL_PIN );
	set_high( PORTB, CEREAL_PIN );

	sei( );
}

ISR( TIMER0_COMPA_vect ){
	if ( bitpos ){
		// handle any pending output character
		if ( cur_char & 1 )
			set_high( PORTB, CEREAL_PIN );
		else
			set_low( PORTB, CEREAL_PIN );

		cur_char >>= 1;
		bitpos--;

	} else {
		// line is set to high as the idle state
		set_high( PORTB, CEREAL_PIN );
	}

	TCNT0 = 0;
}

void put_cereal_char( char c ){
	// wait for current char to finish
	while ( bitpos );

	cur_char = (1<<9) | (((uint16_t)c) << 1);

	// 8 bits per char + 1 start bit + 1 stop bit
	bitpos = 10;
}

void put_cereal_str( char *s ){
	for ( ; *s; s++ )
		put_cereal_char( *s );
}

void put_cereal_int( uint16_t n ){
	char buf[5] = {0};
	int i = 0;

	do {
		buf[i++] = '0' + (n % 10);
		n /= 10;
	} while ( n );

	while(i){
		put_cereal_char( buf[--i] );
	}
}

void put_cereal_hex( uint16_t n ){
	char buf[4] = {0};
	int i = 0;

	do {
		buf[i++] = "0123456789abcdef"[n & 0xf];
		n >>= 4;
	} while ( n );

	while(i){
		put_cereal_char( buf[--i] );
	}
}

void cereal_printf( char *fmt, ... ){
	va_list args;
	uint16_t i;
	union {
		char *s;
		uint16_t n;
	} buf;

	va_start( args, fmt );

	for ( i = 0; fmt[i]; i++ ){
		if ( fmt[i] == '%' && fmt[i+1]){
			i++;
			switch( fmt[i] ){
				case 's':
					buf.s = va_arg( args, char * );
					put_cereal_str( buf.s );
					break;

				case 'd':
				case 'u':
					buf.n = va_arg( args, uint16_t );
					put_cereal_int( buf.n );
					break;

				case 'x':
					buf.n = va_arg( args, uint16_t );
					put_cereal_hex( buf.n );
					break;

				case 'c':
					buf.n = va_arg( args, int );
					put_cereal_char( buf.n );
					break;

				default:
					put_cereal_char( fmt[i] );
					break;
			}

		} else {
			put_cereal_char( fmt[i] );
		}
	}

	va_end( args );
}

#endif /* DEBUG == 1 */
