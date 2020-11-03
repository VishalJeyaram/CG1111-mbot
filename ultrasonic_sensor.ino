/*
 * Refer to MeUltrasonic class here: 
 * https://github.com/Makeblock-official/mBot/blob/master/mBot-default-program/MeUltrasonic.cpp
 */

#include <MeMCore.h>

// instantiate peripheral objects
MeUltrasonicSensor ultra(PORT_1);  // CHECK THAT THE ULTRASONIC SENSOR IS AT PORT 1

// variable to contain integer output from ultrasonic sensor
int distance;

void setup() {
  // initialize serial communications at 9600 bps
  Serial.begin(9600);
}

void loop() {
  // find the distance of an object in front of the ultrasonic sensor and
  // print that to serial monitor. max distance capped at 400 cm, but this can
  // be changed by passing the max range as an argument to the .distance method
  distance = ultra.distanceCm();
  Serial.print("D = ");
  Serial.println(distance);

  delay(500); // to prevent serial monitor from getting too many readings
}
