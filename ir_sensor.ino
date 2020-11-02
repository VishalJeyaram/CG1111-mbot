/*
 * Refer to MePort class here:
 * https://github.com/Makeblock-official/mBot/blob/master/mBot-default-program/MePort.cpp
 */

#include <MeMCore.h>

// instantiate peripheral objects
MePort ir_sensor(PORT_2)  // CHECK THAT RJ25 IR SENSORS ARE AT PORT 2

void setup() {
  // initialize serial communications at 9600 bps
  Serial.begin(9600);

  // variables to contain integer output from ir sensors
  int s1, s2;     // IF THIS CAUSES AN ERROR, CUT THIS LINE TO LINE 5 (MAKE IT GLOBAL)
}

void loop() {
  /* checks if ir_sensor works. should get integer values between 0 and 1023
   * inclusive based on https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/ */
  s1 = ir_sensor.aRead1();
  s2 = ir_sensor.aRead2();
  Serial.print("S1: ");
  Serial.print(s1);
  Serial.print("\t");
  Serial.print("S2: ");
  Serial.println(s2);

  delay(500); // to prevent serial monitor from getting too many readings
}
