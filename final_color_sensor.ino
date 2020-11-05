#include <MeMCore.h>
#include <Arduino.h>

// number of color sample measurements to take in each channel
#define NO_SAMPLES 5

#define BUTTON A7

// response time for taking measurement in ms
#define RESPONSE_TIME 100

// set up threshold values the colors
#define R_YELLOW_LOWER 75
#define R_RED_LOWER 55
#define B_BLUE_LOWER 65
#define R_PURPLE_LOWER 35
#define R_BLACK_UPPER 10


// macros to control RGB LED
#define RED_LED 255,000,000
#define GREEN_LED 000,255,000
#define BLUE_LED 000,000,255
#define OFF_LED 000,000,000

// contains details for red, green and blue channels
struct Color {
  int red;
  int green;
  int blue;
};

// initialise peripheral objects
MeLightSensor light_sensor(PORT_6);
MeRGBLed led(PORT_7); 

/* COLOR CODE FOR REFERENCE DURING CALIBRATION
 * RED -> return 0
   GREEN -> return 1
   YELLOW -> return 2
   PURPLE -> return 3
   BLUE -> return 4 
   WHITE -> return 5
   BLACK -> return 6
   OTHERS -> return -1 */

// colors for calibration
Color blackColor;
Color whiteColor;

void setup() {
  init_light_sensor();
}

void loop() {
  // print out color detected for debugging only
  Serial.println(use_light_sensor());
}

void init_light_sensor() {
  // initialises light sensor and does the required calibration
  
  // initialize serial communication at 9600 bps
  Serial.begin(9600);

  // perform color calibration first
  Serial.println("Place white sample for calibration...");
  delay(5000);
  whiteColor = get_colors();

  Serial.println("Place black sample for calibration...");
  delay(5000);
  blackColor = get_colors();
  Serial.println("Finished calibration");

  // prints out white and black values for calibration
  Serial.print("b_red\t");
  Serial.print(blackColor.red);
  Serial.print("\tb_green\t");
  Serial.print(blackColor.green);
  Serial.print("\tb_blue\t");
  Serial.println(blackColor.blue);
  
  Serial.print("w_red\t");
  Serial.print(whiteColor.red);
  Serial.print("\tw_green\t");
  Serial.print(whiteColor.green);
  Serial.print("\tw_blue\t");
  Serial.println(whiteColor.blue);

  // press then release button to begin the run
  pinMode(BUTTON, INPUT);

  while (analogRead(BUTTON) > 500) {}
  while (analogRead(BUTTON) < 500) {}
  
  // wait for button press to continue with the for loop
  Serial.println("Waiting for button press");
}

int use_light_sensor() {
  /* returns the color code based on measurement from the color sensor */
  
  // for some strange reason, we need a delay so the results are stable
  delay(5000);
  
  // take color measurement
  Color new_color = get_colors();

  // do color balancing. first constrain the new_color measurements so they don't
  // go out of bounds, then scale them
  new_color.red = constrain(new_color.red, blackColor.red, whiteColor.red);
  new_color.green = constrain(new_color.green, blackColor.green, whiteColor.green);
  new_color.blue = constrain(new_color.blue, blackColor.blue, whiteColor.blue);
  
  new_color.red = map(new_color.red, blackColor.red, whiteColor.red, 0, 100);
  new_color.green = map(new_color.green, blackColor.green, whiteColor.green, 0, 100);
  new_color.blue = map(new_color.blue, blackColor.blue, whiteColor.blue, 0, 100);
  
  // return the corresponding code for that color
  return get_color_code(new_color);
}

int get_color_code(Color color) {
  /* returns the corresponding color code */ 
  
  // variables used just to simplify the no of times I need to type color.red, color.green & color.blue
  int red = color.red;
  int green = color.green;
  int blue = color.blue;
  
//   // below used for debugging only
//   Serial.print(red);
//   Serial.print("\t");
//   Serial.print(green);
//   Serial.print("\t");
//   Serial.println(blue);

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
  
  // turn off RGB LED
  led.setColor(OFF_LED);
  led.show();
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
