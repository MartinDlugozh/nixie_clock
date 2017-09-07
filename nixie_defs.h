#ifndef NIXIE_DEFS_H_
#define NIXIE_DEFS_H_

/**
 * K155ID1 inputs
 */
#define IN_A 		13		// decoder input A
#define IN_B 		12		// decoder input B
#define IN_C 		11		// decoder input C
#define IN_D 		10		// decoder input D

/**
 * Anodes
 * (optocouples inputs)
 */
#define A_HH 		9		// first (left) hour digit tube anode
#define A_HL 		8		// second (right) hour digit tube anode
#define A_MH 		7		// first (left) minute digit tube anode
#define A_ML 		6		// second (right) minute digit tube anode
#define A_NE 		5		// blinking separator neon anode

/**
 * Buttons
 * (pull-up input is used)
 */
#define BTN_PP		16		// +1 hour button
#define BTN_MM		17		// -1 hour button
#define BTN_DT		3		// +1 minute button
#define BTN_ADJ 	4		// -1 minute button

#define BUZZER      1

/**
 * Battery monitor and charger
 */
#define BATT_VOLT	A0		// backup power battery voltage sensor pin
#define INPUT_VOLT	A1		// normal input voltage sensor pin
#define CRG_OUT		2		// backup power battery charger pin (P-MOSFET gate)

#define FULL_BATT_THRES 	8.38
#define EMPTY_BATT_THRES 	7.2
#define INPUT_VOLT_THRES 	10

/**
 * Flags
 */
#define DISP_HH 0
#define DISP_HL 1
#define DISP_MH 2
#define DISP_ML 3

#define DISP_TYPE_TIME 	0
#define DISP_TYPE_DATE 	1
#define DISP_TYPE_ALARM 2

#define BUZZ_INACTIVE 	0
#define BUZZ_ACTIVE 	1

#define ALARM_INACTIVE 	0
#define ALARM_STANDBY	1
#define ALARM_ACTIVE	2
#define ALARM_BLOCK 	3

#define ADJ_TYPE_NO 	0
#define ADJ_TYPE_MIN 	1
#define ADJ_TYPE_HR 	2

#endif /* NIXIE_DEFS_H_ */
