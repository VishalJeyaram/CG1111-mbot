#include <MeMCore.h>

// macros to control rgb
#define RED_LED 255,000,000
#define GREEN_LED 000,255,000
#define BLUE_LED 000,000,255

// instantiate peripheral objects
MeRGBLed led(PORT_7);  // the internal connection 
// is handled by port 7, with default slot number 2

void setup() {
  // nothing to be done here
  Serial.begin(9600);
}

void loop() {
  // loops over the 3 colors, staying in each color for 1 second
  Serial.println("red");
  led.setColor(RED_LED);
  led.show();
  delay(1000);

  Serial.println("green");
  led.setColor(GREEN_LED);
  led.show();
  delay(1000);

  Serial.println("blue");
  led.setColor(BLUE_LED);
  led.show();
  delay(1000);
}
