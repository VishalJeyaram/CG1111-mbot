#include <MeMCore.h>

// macros to control rgb
#define RED_LED 255,000,000
#define GREEN_LED 000,255,000
#define BLUE_LED 000,000,255

// instantiate peripheral objects
MeRGBLed led(PORT_7, 2);  // the internal connection 
// is handled by port 7, with default slot number 2

void setup() {
  // nothing to be done here
}

void loop() {
  // loops over the 3 colors, staying in each color for 1 second
  led.setColorAt(0, RED_LED);
  led.setColorAt(1, RED_LED);

  led.setColorAt(0, GREEN_LED);
  led.setColorAt(1, GREEN_LED);

  led.setColorAt(0, BLUE_LED);
  led.setColorAt(1, BLUE_LED);
}
