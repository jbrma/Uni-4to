#include <s3c44b0x.h>
#include <leds.h>

void leds_init(void) {
	PDATB |= ((1 << 9) | (1 << 10));
}

void led_on(uint8 led) {

	if (led == LEFT_LED) {
		PDATB &= ~(1 << 9);
	} else if (led == RIGHT_LED) {
		PDATB &= ~(1 << 10);
	}
}

void led_off(uint8 led) {
	if (led == LEFT_LED) {
		PDATB |= (1 << 9);
	} else if (led == RIGHT_LED) {
		PDATB |= (1 << 10);
	}
}

void led_toggle(uint8 led) {
	if (led == LEFT_LED) {
		PDATB ^= (1 << 9);
	} else if (led == RIGHT_LED) {
		PDATB ^= (1 << 10);
	}
}

uint8 led_status(uint8 led) {

	switch(led){
	case RIGHT_LED:
		if((PDATB & (1 << 10))){
			return 0;
		}
		else{
			return 1;
		}
	case LEFT_LED:
		if((PDATB & (1 << 9))){
			return 0;
		}
		else{
			return 1;
		}
	}

	return 1;
}
