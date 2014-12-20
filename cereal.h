#ifndef _cereal_h
#define _cereal_h 1
#include <avr/io.h>
#include <stdint.h>

// config options
#define CEREAL_PIN PB0
#define BAUD_RATE  BAUD_9600

#ifndef DEBUG
#define DEBUG 1
#endif

// sorted from most to least reliable
#define BAUD_1200   103
#define BAUD_2400   51
#define BAUD_4800   25
#define BAUD_9600   12

// macros, so that debugging statements can be easily stripped out
#if DEBUG
#define INIT_DEBUG( )          init_cereal( );
#define DEBUGF( fmt, ... )     cereal_printf( fmt, __VA_ARGS__ )
#define DEBUGF_ISR( fmt, ... ) {sei(); cereal_printf( fmt, __VA_ARGS__ ); cli( );}
#define DEBUG_PUTS( str )      put_cereal_str( str )
#define DEBUG_PUTS_ISR( str )  put_cereal_str( str )
#define DEBUG_ASSERT(cond)\
	((cond)? 0 :\
	         DEBUGF( "[%s] Assertion at %s:%d failed\r\n", __func__, __FILE__, __LINE__))

void cereal_printf( char *fmt, ... );
void put_cereal_char( char c );
void put_cereal_str( char *s );
void put_cereal_int( uint16_t n );
void put_cereal_hex( uint16_t n );
void init_cereal( void );

#else
#define INIT_DEBUG( )          /* init_cereal( ); */
#define DEBUGF( fmt, ... )     /* fmt __VA_ARGS__ */
#define DEBUGF_ISR( fmt, ... ) /* fmt __VA_ARGS__ */
#define DEBUG_PUTS( str )      /* put_cereal_str( str ) */
#define DEBUG_PUTS_ISR( str )  /* put_cereal_str( str ) */
#define DEBUG_ASSERT(cond)     /* assert cond */
#endif

#endif
