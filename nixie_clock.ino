/**
 * Nixie clock test program
 *
 * First prototype firmware
 * Dinamic indication is used
 */

/**
 * Includes
 */
#include "nixie_defs.h"
#include "Arduino.h"
#include <avr/wdt.h>
#include "K155ID1.h"
#include "Time.h"
#include "DS3231.h"

/**
 * Main source headers
 */
void loop_1000Hz(void);		// nixie digits update loop
void loop_50Hz(void);		// menu loop (buttons reading, etc)
void loop_1Hz(void);		// time update loop

/**
 * Main loop timers
 */
struct {
	volatile uint32_t loop_1000Hz;		// anode switching loop (1ms period)
	volatile uint32_t loop_50Hz;		// buttons reading loop	(20 ms period)
	volatile uint32_t loop_1Hz;			// time updating loop (1000ms period)
	volatile uint32_t btn_block;		// buttons blocking timer (200ms period)
	volatile uint32_t buzzer;			// alarm buzzer update timer
	volatile uint32_t adjust;			// SET-mode blinking timer
}timer;

/**
 * Clock variables and RTC obj
 */
DS3231  rtc(SDA, SCL);
Time  t;
uint8_t months 	= 0;	// month now
uint8_t days 	= 0;	// date now
uint8_t hours 	= 0;	// hour now
uint8_t minutes = 0;	// minute now
uint8_t hh 		= 0;	// first (left) hour digit
uint8_t hl 		= 0;	// second (right) hour digit
uint8_t mh 		= 0;	// first (left) minute digit
uint8_t ml		= 0;	// second (right) minute digit
uint8_t seppnt  = 0;	// blinking separator state
uint8_t al_hours   = 0;	// alarm hours
uint8_t al_minutes = 0; // alarm minutes

/**
 * Buttons states
 */
struct{
	uint8_t mp:1;
	uint8_t mm:1;
	uint8_t dt:1;
	uint8_t al:1;
}btn;

/**
 * Tubes
 */
K155ID1 tube_drv;
uint8_t anodes[4] = { 0, 0, 0, 0 }; // anodes states (can be represented as bit field)

/**
 * Backup battery monitor
 */
float batt_voltage 	= 0; 	// backup power battary voltage
float input_voltage = 0;	// normal input voltage

/**
 * Flags
 */
struct {
	uint8_t disp_type:4;		// TIME/DATE/ALARM
	uint8_t disp_loop:4;		// anode switching loop flag
	bool crg:1;					// battery charging flag
	bool btn_block:1;			// button blocking flag
	bool al_buzzer:1;			// buzzer state (off/on)
	uint8_t al_buzz_cnt:4;		// alarm buzzing counter
	uint8_t alarm:4;			// alarm state/mode counter
	uint8_t adj_type:4;			// adjustion type (when in SET-mode)
	uint8_t blink:1;			// blinking state (when in SET-mode)
}flags;

/**
 * Common macros
 */
#define CRG_ON() 	{ digitalWrite(CRG_OUT, 1); flags.crg = 1; }
#define CRG_OFF() 	{ digitalWrite(CRG_OUT, 0); flags.crg = 0; }
#define BLOCK_BTN() { flags.btn_block = 1; timer.btn_block = millis(); }

/**
 * Split
 * Split a decimal number into separate digits
 */
void split(uint8_t n, uint8_t &h, uint8_t &l)
{
	h = (uint8_t)(n/10);
	l = (uint8_t)(n%10);
}

void update_time(void)
{
	t = rtc.getTime();
	if(t.min != minutes)
	{
		setTime(t.hour, t.min, t.sec, t.date, t.mon, t.year);
	}

	hours = hour();
	minutes = minute();
	split(hours, hh, hl);
	split(minutes, mh, ml);
}

void update_date(void)
{
	t = rtc.getTime();
	if(t.min != minutes)
	{
		setTime(t.hour, t.min, t.sec, t.date, t.mon, t.year);
	}

	months = month();
	days = day();
	split(days, hh, hl);
	split(months, mh, ml);
}

void update_alarm(void)
{
	split(al_hours, hh, hl);
	split(al_minutes, mh, ml);
}

void set_anodes(uint8_t* anodes)
{
	for(uint8_t i = 0; i < 4; i++)
	{
		digitalWrite((A_HH - i), anodes[i]);
	}
}

void do_test()
{
	set_anodes(anodes);
	for(uint8_t i = 0; i < 10; i++)
	{
		tube_drv.set_digit(i);
		delay(100);
	}
}

void adj_update()
{
	if((flags.adj_type != ADJ_TYPE_NO) && ((millis() - timer.adjust) > 250))
	{
		if(flags.blink == 0)
		{
			flags.blink = 1;
		}else if(flags.blink == 1)
		{
			flags.blink = 0;
		}
		timer.adjust = millis();
	}
}

void buzzer_update()
{
	if((flags.alarm == ALARM_ACTIVE) && (flags.al_buzz_cnt > 0))
	{
		if((millis() - timer.buzzer) > 75)
		{
			if(flags.al_buzzer == BUZZ_INACTIVE)
			{
				flags.al_buzzer = BUZZ_ACTIVE;
			}else if(flags.al_buzzer == BUZZ_ACTIVE)
			{
				flags.al_buzzer = BUZZ_INACTIVE;
			}
			flags.al_buzz_cnt--;
			timer.buzzer = millis();
		}
	}

	if(((flags.alarm == ALARM_INACTIVE) || (flags.alarm == ALARM_BLOCK)) && (flags.al_buzzer != 0))
	{
		flags.al_buzzer = 0;
	}

	digitalWrite(BUZZER, flags.al_buzzer);
}

void setup()
{
	pinMode(A_HH, OUTPUT);
	pinMode(A_HL, OUTPUT);
	pinMode(A_MH, OUTPUT);
	pinMode(A_ML, OUTPUT);
	tube_drv.init(IN_D, IN_C, IN_B, IN_A);

	pinMode(CRG_OUT, OUTPUT);
	pinMode(BTN_PP, INPUT);
	pinMode(BTN_MM, INPUT);
	pinMode(BTN_DT, INPUT);
	pinMode(BTN_ADJ, INPUT);
	pinMode(BATT_VOLT, INPUT);
	pinMode(INPUT_VOLT, INPUT);
	pinMode(BUZZER, OUTPUT);

	CRG_OFF();
	flags.crg = 0;
	flags.disp_type = DISP_TYPE_TIME;
	flags.disp_loop = DISP_HH;
	flags.al_buzzer = BUZZ_INACTIVE;
	flags.alarm = ALARM_INACTIVE;
	flags.al_buzz_cnt = 0;
	flags.adj_type = ADJ_TYPE_NO;
	flags.btn_block = 0;

	rtc.begin();
	t = rtc.getTime();
	// set current date when u use new RTC
//	rtc.setDate(20, 8, 2017);
	setTime(t.hour, t.min, t.sec, t.date, t.mon, t.year);

	delay(1000);

	anodes[0] = 1;	anodes[1] = 0;	anodes[2] = 0;	anodes[3] = 0; do_test();
	anodes[0] = 0;	anodes[1] = 1;	anodes[2] = 0;	anodes[3] = 0; do_test();
	anodes[0] = 0;	anodes[1] = 0;	anodes[2] = 1;	anodes[3] = 0; do_test();
	anodes[0] = 0;	anodes[1] = 0;	anodes[2] = 0;	anodes[3] = 1; do_test();

	// reset main loop timers
	timer.loop_1000Hz = millis();
	timer.loop_50Hz = millis();
	timer.loop_1Hz = millis();

	wdt_enable(WDTO_4S);
}

void loop()
{
	wdt_reset();
	/**
	 * Main timer controlled loops (pseudo-processes)
	 */
	if((millis() - timer.loop_1000Hz) >= 1)
	{
		loop_1000Hz();
		timer.loop_1000Hz = millis();
	}

	if((millis() - timer.loop_50Hz) >= 20)
	{
		loop_50Hz();
		timer.loop_50Hz = millis();
	}

	if((millis() - timer.loop_1Hz) >= 1000)
	{
		loop_1Hz();
		timer.loop_1Hz = millis();
	}

	/**
	 * Loops with independent internal timers
	 */
	adj_update(); 		// reset timer and toggle blink flag, if u need to make digits blinking (when in SET-mode)
	buzzer_update();	// make some tone, if alarm is ON (active)
}

void loop_1000Hz()
{
	anodes[0] = 0;	anodes[1] = 0;	anodes[2] = 0;	anodes[3] = 0;
	set_anodes(anodes);
	delayMicroseconds(400);		// DO NOT DELETE THIS DELAY!

	switch(flags.disp_loop)
	{
	case DISP_HH:
	{
		if((flags.adj_type == ADJ_TYPE_HR) && (flags.blink == 0)) 			// if SET-mode is active and blink flag is low
		{
			tube_drv.reset();
		}else{ 																// else display digit as usual
			anodes[0] = 1;	anodes[1] = 0;	anodes[2] = 0;	anodes[3] = 0;
			set_anodes(anodes);
			tube_drv.set_digit(hh);
		}
		flags.disp_loop = DISP_HL;
		break;
	}
	case DISP_HL:
	{
		if((flags.adj_type == ADJ_TYPE_HR) && (flags.blink == 0))
		{
			tube_drv.reset();
		}else{
			anodes[0] = 0;	anodes[1] = 1;	anodes[2] = 0;	anodes[3] = 0;
			set_anodes(anodes);
			tube_drv.set_digit(hl);
		}
		flags.disp_loop = DISP_MH;
		break;
	}
	case DISP_MH:
	{
		if((flags.adj_type == ADJ_TYPE_MIN) && (flags.blink == 0))
		{
			tube_drv.reset();
		}else{
			anodes[0] = 0;	anodes[1] = 0;	anodes[2] = 1;	anodes[3] = 0;
			set_anodes(anodes);
			tube_drv.set_digit(mh);
		}
		flags.disp_loop = DISP_ML;
		break;
	}
	case DISP_ML:
	{
		if((flags.adj_type == ADJ_TYPE_MIN) && (flags.blink == 0))
		{
			tube_drv.reset();
		}else{
			anodes[0] = 0;	anodes[1] = 0;	anodes[2] = 0;	anodes[3] = 1;
			set_anodes(anodes);
			tube_drv.set_digit(ml);
		}
		flags.disp_loop = DISP_HH;
		break;
	}
	default:
		break;
	}
}

void loop_50Hz()
{
	if(flags.btn_block == 0)
	{
		btn.mp = digitalRead(BTN_PP);
		if(btn.mp == 1)
		{
			if(flags.disp_type == DISP_TYPE_TIME)
			{
				if(flags.adj_type == ADJ_TYPE_MIN)
				{
					adjustTime(SECS_PER_MIN);
				}else if(flags.adj_type == ADJ_TYPE_HR)
				{
					adjustTime(SECS_PER_HOUR);
				}else if(flags.adj_type == ADJ_TYPE_NO)
				{
					if(flags.alarm == ALARM_ACTIVE)
					{
						flags.alarm = ALARM_BLOCK;
					}else{
						adjustTime(1);
					}
				}
				rtc.setTime(hour(), minute(), second());
				update_time();
			}else if(flags.disp_type == DISP_TYPE_DATE)
			{
				if(flags.adj_type == ADJ_TYPE_MIN)
				{
					adjustTime((SECS_PER_DAY * 30));
				}else if(flags.adj_type == ADJ_TYPE_HR)
				{
					adjustTime(SECS_PER_DAY);
				}else if(flags.adj_type == ADJ_TYPE_NO)
				{
					if(flags.alarm == ALARM_ACTIVE)
					{
						flags.alarm = ALARM_BLOCK;
					}
				}
				rtc.setDate(day(), month(), year());
				update_date();
			}else if(flags.disp_type == DISP_TYPE_ALARM)
			{
				if(flags.adj_type == ADJ_TYPE_MIN)
				{
					if(al_minutes <= 59)
					{
						al_minutes++;
					}
					if(al_minutes >= 60)
					{
						al_minutes = 0;
						if(al_hours <= 23)
						{
							al_hours++;
						}
						if(al_hours >= 24)
						{
							al_hours = 0;
						}
					}
				}else if(flags.adj_type == ADJ_TYPE_HR)
				{
					if(al_hours <= 23)
					{
						al_hours++;
					}
					if(al_hours >= 24)
					{
						al_hours = 0;
					}
				}else if(flags.adj_type == ADJ_TYPE_NO)
				{
					flags.alarm = ALARM_STANDBY;
					seppnt = 1;
				}
				update_alarm();
			}

			for(uint8_t i = 0; i<4; i++)
			{
				loop_1000Hz();
			}
			BLOCK_BTN();
			return;
		}

		btn.mm = digitalRead(BTN_MM);
		if(btn.mm == 1)
		{
			if(flags.disp_type == DISP_TYPE_TIME)
			{
				if(flags.adj_type == ADJ_TYPE_MIN)
				{
					adjustTime(-SECS_PER_MIN);
				}else if(flags.adj_type == ADJ_TYPE_HR)
				{
					adjustTime(-SECS_PER_HOUR);
				}else if(flags.adj_type == ADJ_TYPE_NO)
				{
					if(flags.alarm == ALARM_ACTIVE)
					{
						flags.alarm = ALARM_BLOCK;
					}else{
						adjustTime(-1);
					}
				}
				rtc.setTime(hour(), minute(), second());
				update_time();
			}else if(flags.disp_type == DISP_TYPE_DATE)
			{
				if(flags.adj_type == ADJ_TYPE_MIN)
				{
					adjustTime(-(SECS_PER_DAY * 30));
				}else if(flags.adj_type == ADJ_TYPE_HR)
				{
					adjustTime(-SECS_PER_DAY);
				}else if(flags.adj_type == ADJ_TYPE_NO)
				{
					if(flags.alarm == ALARM_ACTIVE)
					{
						flags.alarm = ALARM_BLOCK;
					}
				}
				rtc.setDate(day(), month(), year());
				update_date();
			}else if(flags.disp_type == DISP_TYPE_ALARM)
			{
				if(flags.adj_type == ADJ_TYPE_MIN)
				{
					if((al_minutes >= 0) && (al_minutes <= 59))
					{
						al_minutes--;
					}
					if(al_minutes > 59)
					{
						al_minutes = 59;
						if((al_hours >= 0) && (al_hours <= 23))
						{
							al_hours--;
						}
						if(al_hours > 23)
						{
							al_hours = 23;
						}
					}
				}else if(flags.adj_type == ADJ_TYPE_HR)
				{
					if((al_hours >= 0) && (al_hours <= 23))
					{
						al_hours--;
					}
					if(al_hours > 23)
					{
						al_hours = 23;
					}
				}else if(flags.adj_type == ADJ_TYPE_NO)
				{
					flags.alarm = ALARM_INACTIVE;
					seppnt = 0;
				}
				update_alarm();
			}

			for(uint8_t i = 0; i<4; i++)
			{
				loop_1000Hz();
			}
			BLOCK_BTN();
			return;
		}

		btn.dt = digitalRead(BTN_DT);
		if(btn.dt == 1)
		{
			if(flags.disp_type == DISP_TYPE_TIME)
			{
				flags.disp_type = DISP_TYPE_DATE;
				update_date();
				for(uint8_t i = 0; i<4; i++)
				{
					loop_1000Hz();
				}
			}else if(flags.disp_type == DISP_TYPE_DATE)
			{
				flags.disp_type = DISP_TYPE_ALARM;
				update_alarm();
				for(uint8_t i = 0; i<4; i++)
				{
					loop_1000Hz();
				}
			}else if(flags.disp_type == DISP_TYPE_ALARM)
			{
				flags.disp_type = DISP_TYPE_TIME;
				update_time();
				for(uint8_t i = 0; i<4; i++)
				{
					loop_1000Hz();
				}
			}

			if(flags.alarm == ALARM_ACTIVE)
			{
				flags.alarm = ALARM_BLOCK;
			}

			BLOCK_BTN();
			return;
		}

		btn.al = digitalRead(BTN_ADJ);
		if(btn.al == 1)
		{
			if(flags.adj_type == ADJ_TYPE_NO)
			{
				flags.adj_type = ADJ_TYPE_MIN;
			}else if(flags.adj_type == ADJ_TYPE_MIN)
			{
				flags.adj_type = ADJ_TYPE_HR;
			}else if(flags.adj_type == ADJ_TYPE_HR)
			{
				flags.adj_type = ADJ_TYPE_NO;
			}

			if(flags.alarm == ALARM_ACTIVE)
			{
				flags.alarm = ALARM_BLOCK;
			}

			BLOCK_BTN();
			return;
		}
	}else if((flags.btn_block == 1) && ((millis() - timer.btn_block) > 200))
	{
		flags.btn_block = 0;
	}
}

void loop_1Hz()
{
	/**
	 * Battery monitor and charger
	 */
	uint16_t batt_val = analogRead(BATT_VOLT);
	uint16_t in_val = analogRead(INPUT_VOLT);
	batt_voltage = (float)(batt_val/102.3);
	input_voltage = (float)(in_val/63.9375);

	if(batt_voltage < EMPTY_BATT_THRES)
	{
		if(input_voltage >= INPUT_VOLT_THRES)
		{
			CRG_ON();
		}
	}

	if(flags.crg == 1)
	{
		if(batt_voltage >= FULL_BATT_THRES)
		{
			CRG_OFF();
		}
	} /** END Battery monitor and charger **/

	if((flags.alarm == ALARM_STANDBY) || (flags.alarm == ALARM_ACTIVE) || (flags.alarm == ALARM_BLOCK))
	{
		if((hour() == al_hours) && (minute() == al_minutes) && (flags.alarm != ALARM_BLOCK))
		{
			flags.alarm = ALARM_ACTIVE;
			flags.al_buzz_cnt = 12;
		}else if((minute() != al_minutes) && (flags.alarm == ALARM_BLOCK))
		{
			flags.alarm = ALARM_STANDBY;
		}
	}

	if(flags.disp_type != DISP_TYPE_ALARM)
	{
		if(seppnt == 0)
		{
			seppnt = 1;
		}else{
			seppnt = 0;
		}
	}else{
		if((flags.alarm == ALARM_STANDBY) || (flags.alarm == ALARM_ACTIVE))
		{
			seppnt = 1;
		}else{
			seppnt = 0;
		}
	}
	digitalWrite(A_NE, seppnt); 	// separator blink

	/**
	 * Time update
	 */
	if(flags.disp_type == DISP_TYPE_TIME)
	{
		update_time();
	}else if(flags.disp_type == DISP_TYPE_DATE)
	{
		update_date();
	}else if(flags.disp_type == DISP_TYPE_ALARM)
	{
		update_alarm();
	}
	/** END Time update **/
}
