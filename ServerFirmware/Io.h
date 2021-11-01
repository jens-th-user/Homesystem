#ifndef _IO_H
#define _IO_H

// Output
#define TP1_ON()     (PORTA |= 0x08)
#define TP1_OFF()    (PORTA &= (~0x08))
#define TP2_ON()     (PORTA |= 0x04)
#define TP2_OFF()    (PORTA &= (~0x04))
#define TP3_ON()     (PORTB |= 0x40)
#define TP3_OFF()    (PORTB &= (~0x40))

// Input
#define CS0()     (PINB & 0x08)

// Config

#endif
