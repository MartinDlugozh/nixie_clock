#include "nixie_defs.h"
#include "Arduino.h"
#include "K155ID1.h"
#include "Time.h"

#if(DEBUG == 1)
const char comma = ',';
#endif

/**
 * Main source headers
 */
void loop_100Hz(void);
void loop_50Hz(void);
void loop_2Hz(void);
void loop_1Hz(void);

/**
 * Main loop timers
 */
struct {
	volatile uint32_t loop_100Hz;		// anode switching loop
	volatile uint32_t loop_50Hz;		// buttons reading loop
#if(DEBUG == 1)
	volatile uint32_t loop_2Hz;			// debugging loop
#endif
	volatile uint32_t loop_1Hz;			// time updating loop
	volatile uint32_t btn_block;
}timer;

uint8_t hours 	= 0;	// hour now
uint8_t minutes = 0;	// minute now
uint8_t hh 		= 0;	// first (left) hour digit
uint8_t hl 		= 0;	// second (right) hour digit
uint8_t mh 		= 0;	// first (left) minute digit
uint8_t ml		= 0;	// second (right) minute digit

uint8_t anodes[4] = { 0, 0, 0, 0 }; // anodes states

float batt_voltage 	= 0; 	// backup power battary voltage
float input_voltage = 0;	// normal input voltage

struct {
	uint8_t disp_loop;		// anode switching loop flag
	bool crg;				// battery charging flag
	bool btn_block;
}flags = { 0, 0, 0 };

K155ID1 tube_drv;

#define CRG_ON() 	{ digitalWrite(CRG_OUT, 0); flags.crg = 1; }
#define CRG_OFF() 	{ digitalWrite(CRG_OUT, 1); flags.crg = 0; }
#define BLOCK_BTN() { flags.btn_block = 1; timer.btn_block = millis(); }

void expand(uint8_t n, uint8_t &h, uint8_t &l)
{
	h = (uint8_t)(n/10);
	l = (uint8_t)(n - (h*10));
}

void update_time(void)
{
	hours = hour();
	minutes = minute();
	expand(hours, hh, hl);
	expand(minutes, mh, ml);
}

void set_anodes(uint8_t* anodes)
{
	for(uint8_t i = 0; i < 4; i++)
	{
		digitalWrite((A_HH - i), anodes[i]);
	}
}

void setup()
{
	pinMode(A_HH, OUTPUT);
	pinMode(A_HL, OUTPUT);
	pinMode(A_MH, OUTPUT);
	pinMode(A_ML, OUTPUT);
	tube_drv.init(IN_D, IN_C, IN_B, IN_A);

	pinMode(CRG_OUT, OUTPUT);
	CRG_OFF();
	flags.crg = 0;

	pinMode(BTN_HP, INPUT);
	pinMode(BTN_HM, INPUT);
	pinMode(BTN_MP, INPUT);
	pinMode(BTN_MM, INPUT);
	pinMode(BATT_VOLT, INPUT);
	pinMode(INPUT_VOLT, INPUT);

#if(DEBUG == 1)
	Serial.begin(38400);
#endif

	delay(1000);

	timer.loop_100Hz = millis();
	timer.loop_50Hz = millis();
#if(DEBUG == 1)
	timer.loop_2Hz = millis();
#endif
	timer.loop_1Hz = millis();
}

void loop()
{
	if((millis() - timer.loop_100Hz) >= 10)
	{
		loop_100Hz();
		timer.loop_100Hz = millis();
	}

	if((millis() - timer.loop_50Hz) >= 20)
	{
		loop_50Hz();
		timer.loop_50Hz = millis();
	}

#if(DEBUG == 1)
	if((millis() - timer.loop_2Hz) >= 500)
	{
		loop_2Hz();
		timer.loop_2Hz = millis();
	}
#endif

	if((millis() - timer.loop_1Hz) >= 1000)
	{
		loop_1Hz();
		timer.loop_1Hz = millis();
	}
}

void loop_100Hz()
{
	switch(flags.disp_loop)
	{
	case DISP_HH:
	{
		anodes[0] = 1;	anodes[1] = 0;	anodes[2] = 0;	anodes[3] = 0;
		set_anodes(anodes);
		tube_drv.set_digit(hh);
		flags.disp_loop = DISP_HL;
		break;
	}
	case DISP_HL:
	{
		anodes[0] = 0;	anodes[1] = 1;	anodes[2] = 0;	anodes[3] = 0;
		set_anodes(anodes);
		tube_drv.set_digit(hl);
		flags.disp_loop = DISP_MH;
		break;
	}
	case DISP_MH:
	{
		anodes[0] = 0;	anodes[1] = 0;	anodes[2] = 1;	anodes[3] = 0;
		set_anodes(anodes);
		tube_drv.set_digit(mh);
		flags.disp_loop = DISP_ML;
		break;
	}
	case DISP_ML:
	{
		anodes[0] = 0;	anodes[1] = 0;	anodes[2] = 0;	anodes[3] = 1;
		set_anodes(anodes);
		tube_drv.set_digit(ml);
		flags.disp_loop = DISP_HH;
		break;
	}
	default:
		break;
	}
}

void loop_50Hz()
{
#if (DEBUG == 0)
	if(flags.btn_block == 0)
	{
		if(digitalRead(BTN_HP) == 0)
		{
			adjustTime(SECS_PER_HOUR);
			update_time();
			BLOCK_BTN();
		}
		if(digitalRead(BTN_HM) == 0)
		{
			adjustTime(-SECS_PER_HOUR);
			update_time();
			BLOCK_BTN();
		}
		if(digitalRead(BTN_MP) == 0)
		{
			adjustTime(SECS_PER_MIN);
			update_time();
			BLOCK_BTN();
		}
		if(digitalRead(BTN_HP) == 0)
		{
			adjustTime(-SECS_PER_MIN);
			update_time();
			BLOCK_BTN();
		}
	}else if((flags.btn_block == 1) && ((millis() - timer.btn_block) > 200))
	{
		flags.btn_block = 0;
	}
#endif
}

void loop_2Hz()
{
	Serial.print(hh); Serial.print(hl); Serial.print(comma);
	Serial.print(mh); Serial.print(ml); Serial.println();
}
void loop_1Hz()
{
	/**
	 * Battery monitor and charger
	 */
	batt_voltage = map(analogRead(BATT_VOLT), 0, 255, 0, 10);
	input_voltage = map(analogRead(INPUT_VOLT), 0, 255, 0, 15);

	if(batt_voltage < 7.2)
	{
		CRG_ON();
	}

	if(flags.crg == 1)
	{
		if(batt_voltage >= 8.4)
		{
			CRG_OFF();
		}
	} /** END Battery monitor and charger **/

	digitalWrite(A_NE, !digitalRead(A_NE)); 	// separator blink

	/**
	 * Time update
	 */
	update_time();
	/** END Time update **/
}
