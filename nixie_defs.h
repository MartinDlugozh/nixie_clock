#ifndef NIXIE_DEFS_H_
#define NIXIE_DEFS_H_

#define DEBUG 		1

/**
 * K155ID1 Inputs
 */
#define IN_A 		13		// decoder input A
#define IN_B 		12		// decoder input B
#define IN_C 		11		// decoder input C
#define IN_D 		10		// decoder input D

/**
 * Anodes
 */
#define A_HH 		9		// first (left) hour digit tube anode
#define A_HL 		8		// second (right) hour digit tube anode
#define A_MH 		7		// first (left) minute digit tube anode
#define A_ML 		6		// second (right) minute digit tube anode
#define A_NE 		5		// blinking separator neon anode

/**
 * Buttons
 */
#define BTN_HP		16		// +1 hour button
#define BTN_HM		17		// -1 hour button
#define BTN_MP		18		// +1 minute button
#define BTN_MM 		19		// -1 minute button

/**
 * Battery monitor and charger
 */
#define BATT_VOLT	A0		// backup power battery voltage sensor pin
#define INPUT_VOLT	A1		// normal input voltage sensor pin
#define CRG_OUT		4		// backup power battery charger pin (P-MOSFET gate)

/**
 * Flags
 */
#define DISP_HH 0
#define DISP_HL 1
#define DISP_MH 2
#define DISP_ML 3


#endif /* NIXIE_DEFS_H_ */
