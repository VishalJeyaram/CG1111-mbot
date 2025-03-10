#include <Wire.h>

#include <SoftwareSerial.h>

#include <MeMCore.h>

#include <Arduino.h>

// define pin numbers for multiple sensors
#define IR_LEFT A0
#define IR_RIGHT A1
#define BUTTON A7
#define MOTOR_SPEED 100

// number of color sample measurements to take in each channel
#define NO_SAMPLES 5

// response time for taking measurement in ms (NEED TO CHECK IF REQUIRED)
#define RESPONSE_TIME 1000

// set up threshold values for the colors
#define B_BLUE_LOWER 65
#define R_YELLOW_LOWER 90
#define R_RED_LOWER 55
#define B_PURPLE_LOWER 50
#define G_BLACK_UPPER 10

// macros to control rgb
#define RED_LED 255, 000, 000
#define GREEN_LED 000, 255, 000
#define BLUE_LED 000, 000, 255
#define OFF_LED 000, 000, 000

// initialize peripheral objects
MeDCMotor motor_left(M1);
MeDCMotor motor_right(M2);
MeLineFollower lineFinder(PORT_2);
MeLimitSwitch ir_sides(PORT_4);
MeUltrasonicSensor ultra(PORT_1);
MeLightSensor light_sensor(PORT_6);
MeRGBLed led(PORT_7);
MeBuzzer buzzer;
MePort ir_sensor(PORT_3);

// struct contains details for red, green and blue channels
struct Color {
  int red;
  int green;
  int blue;
};

// colors for calibration
Color black_color = {
  147,
  97,
  106
};

Color white_color = {
  283,
  195,
  208
};

void setup() {
  // initialize serial communications at 9600 bps
  // Serial.begin(9600);

  // initialize left and right IR side sensor pins
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  // initialize start button
  pinMode(BUTTON, INPUT);

  // perform color calibration first--***CODE IS COMMENTED OUT DURING THE RACE***
  /*
  Serial.println("Place white sample for calibration...");
  delay(5000);
  Serial.println("Starting Calibration");
  white_color = get_colors();

  Serial.println("Place black sample for calibration...");
  delay(5000);
  Serial.println("Starting Calibration");
  black_color = get_colors();
  Serial.println("Finished calibration");
  Serial.println(white_color.red);
  Serial.println(white_color.green);
  Serial.println(white_color.blue);
  Serial.println(black_color.red);
  Serial.println(black_color.green);
  Serial.println(black_color.blue);
  */

  // press then release button to begin the run 
  // Serial.println("Waiting for button press");
  while (analogRead(BUTTON) > 500) {}
  while (analogRead(BUTTON) < 500) {}
}

void loop() {
  // To test if lightsenor, ultrasound sensor, IR sensors are functional and print to serial monitor
  // read the lightSensor value:
  /*--***CODE IS COMMENTED OUT DURING THE RACE*** FOR DEBUGGING PURPOSES ONLY
  int lightsensor_value = lightSensor.read();

  // print the lightSensor value measured by ultrasonic sensor to serial monitor
  Serial.print("lightSensor value = ");
  Serial.println(lightsensor_value);

  // print the distance measured by ultrasonic sensor to serial monitor
  Serial.print("Distance : ");
  Serial.print(ultra.distanceCm() );
  Serial.println(" cm");

  // print the values measured by ir sensors to serial monitor
  int ir_left = ir_sides.aRead1();
  int ir_right = ir_sides.aRead2();
  Serial.print("ir_left value = ");
  Serial.println(ir_left);
  Serial.print("ir_right value = ");
  Serial.println(ir_right);
  */
  // stop and take color measurements to decide course of action if black line is encountered.
  // otherwise keep going on autopilot
  if (is_black_line()) {
     stop_moving();
    /* 
    COLOR CODE RETURN VALUES FOR REFERENCE
    
    RED -> return 0
    GREEN -> return 1
    YELLOW -> return 2
    PURPLE -> return 3
    BLUE -> return 4 
    BLACK -> return 5
    */
    int colour_code = get_balance_colour_code();
    // refer to colour codes below for respective assigned return values
    if (colour_code == 0) {
      // Serial.println("Red");
      red();
    } else if (colour_code == 1) {
      // Serial.println("Green");
      green();
    } else if (colour_code == 2) {
      // Serial.println("Yellow");
      yellow();
    } else if (colour_code == 3) {
      purple();
      // Serial.println("Purple");
    } else if (colour_code == 4) {
      light_blue();
      // Serial.println("Light Blue");
    } else if (colour_code == 5) {
      black();
      // Serial.println("Black");
    } else {
      // Serial.println("Wrong color code")
    }
  } else {
    autopilot();
  }
}

int get_balance_colour_code() {
  // response time
  delay(RESPONSE_TIME);
  // take raw colour readings
  Color new_color = get_colors();

  // do color balancing
  new_color.red = map(new_color.red, black_color.red, white_color.red, 0, 100);
  new_color.green = map(new_color.green, black_color.green, white_color.green, 0, 100);
  new_color.blue = map(new_color.blue, black_color.blue, white_color.blue, 0, 100);

  // print out color detected
  return (get_color_code(new_color));
}

int get_color_code(Color color) {
  // returns the corresponding color code

  int red = color.red;
  int green = color.green;
  int blue = color.blue;

  //  Serial.print(red);
  //  Serial.print("\t");
  //  Serial.print(green);
  //  Serial.print("\t");
  //  Serial.println(blue);

  /* 
   COLOR CODE RETURN VALUES
   
   RED -> return 0
   GREEN -> return 1
   YELLOW -> return 2
   PURPLE -> return 3
   BLUE -> return 4 
   BLACK -> return 5
  */

  // distinguishing colours based on distinct characteristics of colours
  if (blue > B_BLUE_LOWER) {
    // blue
    // Serial.println("blue");
    return 4;
  } 
  else if (red > R_YELLOW_LOWER) {
    // yellow
    // Serial.println("yellow");
    return 2;
  }    
  else if (red > R_RED_LOWER) {
    // red
    // Serial.println("red");
    return 0;
  }    
  else if (blue > B_PURPLE_LOWER) {
    // purple
    // Serial.println("purple");
    return 3;
  }    
  else if (green < G_BLACK_UPPER) {
    // black
    // Serial.println("black");
    return 5;
  }    
  else {
    // green
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

  // off RGB LED
  led.setColor(OFF_LED);
  led.show();
  
  return color;
}

int get_single_color() {
  // returns the average color value for single channel
  int sum = 0;

  for (int j = 0; j < NO_SAMPLES; j++) {
    sum += light_sensor.read();
  }

  return sum / NO_SAMPLES;
}

// adds a pause for specific no. of beats
void music_delay(float seconds) {
  long endTime = millis() + seconds * 1000;
  while (millis() < endTime) {};
}

void music() {
  // plays a specific note for no. of beats * milliseconds
  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  // pauses for 0.5 beats
  music_delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1245, 1.1 * 1000);

  buzzer.tone(1397, 1.3 * 1000);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1245, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1397, 0.25 * 1000);

  buzzer.tone(1397, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(932, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 1.25 * 1000);
  music_delay(1);

  buzzer.tone(1047, 0.5 * 1000);

  buzzer.tone(1047, 0.5 * 1000);
  music_delay(0.25);

  buzzer.tone(1568, 0.3 * 1000);

  buzzer.tone(1397, 1.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1245, 1.1 * 1000);

  buzzer.tone(1175, 0.5 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 1.25 * 1000);
  music_delay(1);

  buzzer.tone(1047, 0.5 * 1000);

  buzzer.tone(1047, 0.5 * 1000);
  music_delay(0.5);

  buzzer.tone(1568, 0.5 * 1000);

  buzzer.tone(1760, 1.4 * 1000);
  music_delay(0.25);

  buzzer.tone(1397, 0.5 * 1000);

  buzzer.tone(1568, 0.5 * 1000);
  music_delay(1);

  buzzer.tone(1047, 1.25 * 1000);
  music_delay(1);

  buzzer.tone(1047, 0.5 * 1000);

  buzzer.tone(1047, 0.5 * 1000);
  music_delay(0.25);

  buzzer.tone(1568, 0.3 * 1000);

  buzzer.tone(1397, 1.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1245, 1.1 * 1000);

  buzzer.tone(1175, 0.5 * 1000);
  music_delay(0.5);

  buzzer.tone(932, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(932, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(932, 0.25 * 1000);

  buzzer.tone(1047, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 0.25 * 1000);

  buzzer.tone(1175, 0.25 * 1000);
  music_delay(0.5);

  buzzer.tone(1047, 2 * 1000);
}

// if a black strip is encountered, returns 1, otherwise 0
bool is_black_line() {
  return lineFinder.readSensors() == S1_IN_S2_IN;
}

// function to make mbot stop moving for 1 second
void stop_moving() {
  motor_left.run(0);
  motor_right.run(0);
  delay(1000);
}

// function to make mbot move straight after completing a turn
void moving_straight(float multiplication_factor) {
  motor_left.run(-multiplication_factor * MOTOR_SPEED);
  motor_right.run(multiplication_factor * MOTOR_SPEED);
}

// function for mbot to make a left turn
int red() {
  motor_left.run(MOTOR_SPEED);
  motor_right.run(MOTOR_SPEED);
  delay(670);
}

// function for mbot to make a left turn for the second time (during a consecutive successive turn)
int red_second() {
  motor_left.run(MOTOR_SPEED);
  motor_right.run(MOTOR_SPEED);
  delay(600);
}

// function for mbot to make a right turn
int green() {
  motor_left.run(-MOTOR_SPEED);
  motor_right.run(-MOTOR_SPEED);
  delay(630);
}

// function for mbot to make a right turn for the second time (during a consecutive successive turn)
int green_second() {
  motor_left.run(-MOTOR_SPEED);
  motor_right.run(-MOTOR_SPEED);
  delay(650);
}

// function for mbot to make 2 consecutive left turns (to turn 180 degrees within same grid)
int yellow() {
  red();
  red_second();
}

// function for mbot to make 2 consecutive left turns (after travelling certain distance in between)
int purple() {
  red();
  double frontDistance = 100;
  while (frontDistance > 10.00) {
    frontDistance = ultra.distanceCm();
    //Serial.print("Distance : ");
    //Serial.print(frontDistance);
    //Serial.println(" cm");
    moving_straight(1.5);
  }
  red_second();
}

// function for mbot to make 2 consecutive right turns (after travelling certain distance in between)
int light_blue() {
  green();
  double frontDistance = 100;
  while (frontDistance > 10.00) {
    frontDistance = ultra.distanceCm();
    //Serial.print("Distance : ");
    //Serial.print(frontDistance);
    //Serial.println(" cm");
    moving_straight(1.5);
  }
  green_second();
}

// function for mbot to play victory music and stop moving once at end of the maze
int black() {
  music();
  while (true) {
    stop_moving();
  }
}

// function for mbot to auto correct itself using IR sensors & to move as straight as possible
void autopilot() {

  int ir_left = ir_sensor.aRead1();
  int ir_right = ir_sensor.aRead2();
  // Serial.print("ir_left: ");
  //Serial.print(ir_left);
  //Serial.print("\t");
  //Serial.print("ir_right: ");
  //Serial.println(ir_right);

  if (ir_left < 415) {
    // turn right to avoid bumping into the left wall
    motor_left.run(-2 * 1.5 * MOTOR_SPEED);
    motor_right.run(1.5 * MOTOR_SPEED);
  } else if (ir_right < 370) {
    // turn left to avoid bumping into the right wall
    motor_left.run(-1.5 * MOTOR_SPEED);
    motor_right.run(2 * 1.5 * MOTOR_SPEED);
  } else {
    // else just keep moving straight
    moving_straight(1.0);
  }
}
