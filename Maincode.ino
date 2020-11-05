#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeMCore.h>
#include <Arduino.h>

// define pin numbers for multiple sensors
#define IR_LEFT A0
#define IR_RIGHT A1
#define BUTTON A7
#define MOTOR_SPEED 100
#define LINE
// number of color sample measurements to take in each channel
#define NO_SAMPLES 5

#define BUTTON A7

// response time for taking measurement in ms (NEED TO CHECK IF REQUIRED)
#define RESPONSE_TIME 1000

// set up threshold values the colors
#define R_YELLOW_LOWER 75
#define R_RED_LOWER 55
#define B_BLUE_LOWER 65
#define R_PURPLE_LOWER 35
#define R_BLACK_UPPER 10


// macros to control rgb
#define RED_LED 255,000,000
#define GREEN_LED 000,255,000
#define BLUE_LED 000,000,255

// initialize peripheral objects
MeDCMotor motor_left(M1);
MeDCMotor motor_right(M2);
MeLineFollower lineFinder(PORT_2);
MeLimitSwitch ir_sides(PORT_4);
MeUltrasonicSensor ultra(PORT_1);
MeLightSensor light_sensor(PORT_6);
MeRGBLed led(PORT_7);  // the internal connection is handled by port 7, with default slot number 2
MeBuzzer buzzer;
MePort ir_sensor(PORT_3);

// contains details for red, green and blue channels
struct Color {
  int red;
  int green;
  int blue;
};

// colors for calibration
Color black;
Color white;

void setup() {
  // initialize serial communications at 9600 bps
  Serial.begin(9600);

  // initialize left and right IR side sensor pins
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  // initialize start button
  // press then release button to begin the run
  pinMode(BUTTON, INPUT);

  //  // turn off the LED
  //  rgb_led.setColor(0, 0, 0);
  //  rgb_led.show();
  //
  //  // turn on the LED
  //  rgb_led.setColor(0, 255, 255);
  //  rgb_led.show();
  // perform color calibration first
  Serial.println("Place white sample for calibration...");
  delay(5000);
  Serial.println("Starting Calibration");
  white = get_colors();

  Serial.println("Place black sample for calibration...");
  delay(5000);
  Serial.println("Starting Calibration");
  black = get_colors();
  Serial.println("Finished calibration");

  // initialize start button
  // press then release button to begin the run 
  Serial.println("Waiting for button press");
  while (analogRead(BUTTON) > 500) {}
  while (analogRead(BUTTON) < 500) {}
 
}

void loop() {
  // read the lightSensor value:
  //int value = lightSensor.read();

  // print the lightSensor value measured by ultrasonic sensor to serial monitor
  //  Serial.print("lightSensor value = ");
  //Serial.println(value);

  // print the distance measured by ultrasonic sensor to serial monitor
  // Serial.print("Distance : ");
  // Serial.print(ultra.distanceCm() );
  // Serial.println(" cm");

  // print the values measured by ir sensors to serial monitor
  // int val1 = ir_sides.aRead1();
  //int val2 = ir_sides.aRead2();
  // Serial.print("irSensor1 value = ");
  // Serial.println(val1);
  // Serial.print("irSensor2 value = ");
  // Serial.println(val2);

  // wait 100 milliseconds before the next loop
  moveFront();
}

int get_balance_colour_code() {
  delay(5000);
  // take color measurement
  Color new_color = get_colors();

  // do color balancing
  new_color.red = map(new_color.red, black.red, white.red, 0, 100);
  new_color.green = map(new_color.green, black.green, white.green, 0, 100);
  new_color.blue = map(new_color.blue, black.blue, white.blue, 0, 100);

  // print out color detected
  return (get_color_code(new_color));    
}

int get_color_code(Color color) {
  // returns the corresponding color code

  int red = color.red;
  int green = color.green;
  int blue = color.blue;
  Serial.print(red);
  Serial.print("\t");
  Serial.print(green);
  Serial.print("\t");
  Serial.println(blue);

  if (red > R_YELLOW_LOWER)
  {
    // yellow
    Serial.println("yellow");
    return 2;
  } else if (red > R_RED_LOWER)
  {
    // red
    Serial.println("red");
    return 0;
  } else if (blue > B_BLUE_LOWER)
  {
    // blue
    Serial.println("blue");
    return 4;
  } else if (red > R_PURPLE_LOWER)
  {
    // purple
    Serial.println("purple");
    return 3;
  } else if (red < R_BLACK_UPPER)
  {
    // black
    Serial.println("black");
    return 6;
  } else {
    // green
    Serial.println("green");
    return 1;
  }
}

Color get_colors() {
  // takes NO_SAMPLES samples and returns the average for each channel
  Color color;

  // set RGB LED to RED
  led.setColor(RED_LED);
  led.show();
  color.red = get_single_color();

  // set RGB LED to GREEN
  led.setColor(GREEN_LED);
  led.show();
  color.green = get_single_color();

  // set RGB LED to BLUE
  led.setColor(BLUE_LED);
  led.show();
  color.blue = get_single_color();
  return color;
}

int get_single_color() {
  // returns the average color value for single channel
  int sum = 0;

  delay(RESPONSE_TIME);

  for (int j = 0; j < NO_SAMPLES; j++){
    sum += light_sensor.read();
  }

  return sum / NO_SAMPLES;
}


void _loop() {
}

void _delay(float seconds) {
  long endTime = millis() + seconds * 1000;
  while (millis() < endTime) _loop();
}

void music()
{
  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1245, 1.1 * 1000);

  buzzer.tone(1397, 1.3 * 1000);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(932, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 1.25 * 1000);
  _delay(1);

  buzzer.tone(1047, 0.5 * 1000);

  buzzer.tone(1047, 0.5 * 1000);
  _delay(0.25);

  buzzer.tone(1568, 0.3 * 1000);

  buzzer.tone(1397, 1.25 * 1000);
  _delay(0.5);

  buzzer.tone(1245, 1.1 * 1000);

  buzzer.tone(1175, 0.5 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 1.25 * 1000);
  _delay(1);

  buzzer.tone(1047, 0.5 * 1000);

  buzzer.tone(1047, 0.5 * 1000);
  _delay(0.5);

  buzzer.tone(1568, 0.5 * 1000);

  buzzer.tone(1760, 1.4 * 1000);
  _delay(0.25);

  buzzer.tone(1397, 0.5 * 1000);

  buzzer.tone(1568, 0.5 * 1000);
  _delay(1);

  buzzer.tone(1047, 1.25 * 1000);
  _delay(1);

  buzzer.tone(1047, 0.5 * 1000);

  buzzer.tone(1047, 0.5 * 1000);
  _delay(0.25);

  buzzer.tone(1568, 0.3 * 1000);

  buzzer.tone(1397, 1.25 * 1000);
  _delay(0.5);

  buzzer.tone(1245, 1.1 * 1000);

  buzzer.tone(1175, 0.5 * 1000);
  _delay(0.5);

  buzzer.tone(932, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(932, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(932, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);
  _delay(0.5);

  buzzer.tone(1047, 2 * 1000);
}








int clear_path()
{
  int line_value = lineFinder.readSensors();
  if (line_value == S1_OUT_S2_OUT) {
    return 1;
  }
  return 0;
}

int obstacle()
{
  int line_value = lineFinder.readSensors();
  if (line_value == S1_IN_S2_IN) {
    return 1;
  }
  return 0;
}

void stop_moving()
{
  motor_left.run(0);
  motor_right.run(0);
  delay(1000);
}



int red()
{
  motor_left.run(MOTOR_SPEED);
  motor_right.run(MOTOR_SPEED);
  delay(800);
}

int green() {
  motor_left.run(-MOTOR_SPEED);
  motor_right.run(-MOTOR_SPEED);
  delay(800);
}

int yellow() {
  green();
  stop_moving();
  green();
  stop_moving();
}

int purple() {
  red();
  stop_moving();
  double frontDistance = 100;
  while ( frontDistance > 10.00 ) {
    frontDistance = ultra.distanceCm();
    Serial.print("Distance : ");
    Serial.print(frontDistance);
    Serial.println(" cm");
    autopilot();
  }
  stop_moving();
  red();
  stop_moving();
}


int light_blue () {
  green();
  stop_moving();
  double frontDistance = 100;
  while ( frontDistance > 10.00 ) {
    frontDistance = ultra.distanceCm();
    Serial.print("Distance : ");
    Serial.print(frontDistance);
    Serial.println(" cm");
    autopilot();
  }
  stop_moving();
  green();
  stop_moving();
}

int _black()
{
  music();
}

void moving_straight() 
{   
  motor_left.run(-MOTOR_SPEED);
  motor_right.run(MOTOR_SPEED);
}

void autopilot() {
  int s1, s2;
  
  s1 = ir_sensor.aRead1();
  s2 = ir_sensor.aRead2();
  int difference = s1 - s2;
  Serial.print("S1: ");
  Serial.print(s1);
  Serial.print("\t");
  Serial.print("S2: ");
  Serial.println(s2);
  if ( abs(difference) < 10 ) {
    moving_straight();
  } else if (difference > 300 ) {
    motor_left.run(-MOTOR_SPEED);
    motor_right.run(5 * MOTOR_SPEED);
  } else if ( difference < -300 ) {
    motor_left.run(-5 * MOTOR_SPEED);
    motor_right.run(MOTOR_SPEED);
  } else if ( ( s1 > 500 ) || ( s2 > 500 ) ) {
    moving_straight(); 
  }
}



void moveFront() 
{  
  if ( obstacle() == 1 ) {
    stop_moving();

  int colour_code = get_balance_colour_code();
    
    if ( colour_code == 0 ) {
      red();
    } else if ( colour_code == 1 ) {
      green();
    } else if ( colour_code == 2 ) {
      Serial.println("yellow is cool");
      yellow();
    } else if ( colour_code == 3 ) {
      purple();
    } else if ( colour_code == 4 ) {
      light_blue();
    } else if ( colour_code == 6 ) {
      _black();
    } else {
      moveFront();
    }
  } else {
    autopilot();
  }
}













/* COLOR CODE FOR REFERENCE DURING CALIBRATION
 * RED -> return 0
   GREEN -> return 1
   YELLOW -> return 2
   PURPLE -> return 3
   BLUE -> return 4 
   WHITE -> return 5
   BLACK -> return 6
   OTHERS -> return -1 */



