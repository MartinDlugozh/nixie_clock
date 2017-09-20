#ifndef K155ID1_H_
#define K155ID1_H_

#include "Arduino.h"

#define DD_INPUTS 4
#define DD_DIGITS 10

/**
 * Dedimal decoder input masks
 * states: D, C, B, A
 */
#define _0 { 0, 0, 0, 0 }
#define _1 { 0, 0, 0, 1 }
#define _2 { 0, 0, 1, 0 }
#define _3 { 0, 0, 1, 1 }
#define _4 { 0, 1, 0, 0 }
#define _5 { 0, 1, 0, 1 }
#define _6 { 0, 1, 1, 0 }
#define _7 { 0, 1, 1, 1 }
#define _8 { 1, 0, 0, 0 }
#define _9 { 1, 0, 0, 1 }

#define _STATES { _0, _1, _2, _3, _4, _5, _6, _7, _8, _9 }

class K155ID1
{
public:
	K155ID1();

	void init(uint8_t in_d, uint8_t in_c, uint8_t in_b, uint8_t in_a);
	void set_digit(uint8_t digit);
	void reset(void);

private:
	uint8_t _in[DD_INPUTS];
	uint8_t _st[DD_DIGITS][DD_INPUTS] = _STATES;
};

/**
 * Constructor
 */
K155ID1::K155ID1(){}

/**
 * K155ID1 initialization
 * Configures uint8_t in_d, uint8_t in_c, uint8_t in_b, uint8_t in_a as
 * K155ID1 decimal decoder inputs
 */
void K155ID1::init(uint8_t in_d, uint8_t in_c, uint8_t in_b, uint8_t in_a)
{
	_in[0] = in_d;
	_in[1] = in_c;
	_in[2] = in_b;
	_in[3] = in_a;

	for(uint8_t i = 0; i < DD_INPUTS; i++)
	{
		pinMode(_in[i], OUTPUT);
	}
}

/**
 * Set K155ID1 decimal decoder output digit
 */
void K155ID1::set_digit(uint8_t digit)
{
	uint8_t cat = 0;

	if(digit <= DD_DIGITS)
	{
		switch(digit)
		{
		case 0:
		{
			cat = 2;
			break;
		}
		case 1:
		{
			cat = 9;
			break;
		}
		case 2:
		{
			cat = 8;
			break;
		}
		case 3:
		{
			cat = 4;
			break;
		}
		case 4:
		{
			cat = 1;
			break;
		}
		case 5:
		{
			cat = 6;
			break;
		}
		case 6:
		{
			cat = 3;
			break;
		}
		case 7:
		{
			cat = 7;
			break;
		}
		case 8:
		{
			cat = 5;
			break;
		}
		case 9:
		{
			cat = 0;
			break;
		}
		default:
			break;
		}

		for(uint8_t i = 0; i < DD_INPUTS; i++)
		{
			digitalWrite(_in[i], _st[cat][i]);
		}
	}
}

void K155ID1::reset(void)
{
	for(uint8_t i = 0; i < DD_INPUTS; i++)
	{
		digitalWrite(_in[i], 1);
	}
}

#endif /* K155ID1_H_ */
