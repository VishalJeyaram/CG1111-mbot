#include <MeMCore.h>
#include <Arduino.h>

// number of color sample measurements to take in each channel
#define NO_SAMPLES 10

#define BUTTON A7

// response time for taking measurement in ms
#define RESPONSE_TIME 100

// set up threshold values for the colors. ENSURE THESE HAVE BEEN UPDATED!
 #define R_RED_UPPER 0
 #define R_RED_LOWER 0
 #define G_RED_UPPER 0
 #define G_RED_LOWER 0
 #define B_RED_UPPER 0
 #define B_RED_LOWER 0

 #define R_GREEN_UPPER 0
 #define R_GREEN_LOWER 0
 #define G_GREEN_UPPER 0
 #define G_GREEN_LOWER 0
 #define B_GREEN_UPPER 0
 #define B_GREEN_LOWER 0

 #define R_YELLOW_UPPER 0
 #define R_YELLOW_LOWER 0
 #define G_YELLOW_UPPER 0
 #define G_YELLOW_LOWER 0
 #define B_YELLOW_UPPER 0
 #define B_YELLOW_LOWER 0

 #define R_PURPLE_UPPER 0
 #define R_PURPLE_LOWER 0
 #define G_PURPLE_UPPER 0
 #define G_PURPLE_LOWER 0
 #define B_PURPLE_UPPER 0
 #define B_PURPLE_LOWER 0

 #define R_BLUE_UPPER 0
 #define R_BLUE_LOWER 0
 #define G_BLUE_UPPER 0
 #define G_BLUE_LOWER 0
 #define B_BLUE_UPPER 0
 #define B_BLUE_LOWER 0

 #define R_WHITE_UPPER 0
 #define R_WHITE_LOWER 0
 #define G_WHITE_UPPER 0
 #define G_WHITE_LOWER 0
 #define B_WHITE_UPPER 0
 #define B_WHITE_LOWER 0

 #define R_BLACK_UPPER 0
 #define R_BLACK_LOWER 0
 #define G_BLACK_UPPER 0
 #define G_BLACK_LOWER 0
 #define B_BLACK_UPPER 0
 #define B_BLACK_LOWER 0

// values to control RGB LED
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

// used for calibration. ENSURE THESE HAVE BEEN UPDATED!
Color black = {0, 0, 0};
Color white = {0, 0, 0};

// initialise peripheral objects
MeLightSensor light_sensor(PORT_6);
MeRGBLed led(PORT_7);  // the internal connection is handled by port 7, with default slot number 2

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

void setup() {
  // initialize serial communication at 9600 bps
  Serial.begin(9600);

  // initialize start button
  // press then release button to begin the run
  pinMode(BUTTON, INPUT);

  while (analogRead(BUTTON) > 500) {}
  while (analogRead(BUTTON) < 500) {}
  
  // wait for button press to continue with the for loop
  Serial.println("Waiting for button press");
}

void loop() {
  // print out color detected for debugging only
  Serial.println(use_light_sensor());
}

int use_light_sensor() {
  /* returns the color code based on measurement from the color sensor */
  
  // take color measurement
  Color new_color = get_colors();

  // do color balancing. first constrain the new_color measurements so they don't
  // go out of bounds, then scale them
  new_color.red = constrain(new_color.red, black.red, white.red);
  new_color.green = constrain(new_color.green, black.green, white.green);
  new_color.blue = constrain(new_color.blue, black.blue, white.blue);
  
  new_color.red = map(new_color.red, black.red, white.red, 0, 100);
  new_color.green = map(new_color.green, black.green, white.green, 0, 100);
  new_color.blue = map(new_color.blue, black.blue, white.blue, 0, 100);
  
  // return the corresponding code for that color
  return get_color_code(new_color);
}

int get_color_code(Color color) {
   // returns the corresponding color code

   int red = color.red;
   int green = color.green;
   int blue = color.blue;

   if (red < R_RED_UPPER && red > R_RED_LOWER && 
       green < G_RED_UPPER && green > G_RED_LOWER && 
       blue < B_RED_UPPER && blue > B_RED_LOWER){
     // RED
     return 0;
   } else if (red < R_GREEN_UPPER && red > R_GREEN_LOWER && 
              green < G_GREEN_UPPER && green > G_GREEN_LOWER && 
              blue < B_GREEN_UPPER && blue > B_GREEN_LOWER){
     // GREEN
     return 1;
   } else if (red < R_YELLOW_UPPER && red > R_YELLOW_LOWER && 
              green < G_YELLOW_UPPER && green > G_YELLOW_LOWER && 
              blue < B_YELLOW_UPPER && blue > B_YELLOW_LOWER){
     // YELLOW
     return 2;
   } else if (red < R_PURPLE_UPPER && red > R_PURPLE_LOWER && 
              green < G_PURPLE_UPPER && green > G_PURPLE_LOWER && 
              blue < B_PURPLE_UPPER && blue > B_PURPLE_LOWER){
     // PURPLE
     return 3;
   } else if (red < R_BLUE_UPPER && red > R_BLUE_LOWER && 
              green < G_BLUE_UPPER && green > G_BLUE_LOWER && 
              blue < B_BLUE_UPPER && blue > B_BLUE_LOWER){
     // BLUE
     return 4;
   } else if (red < R_WHITE_UPPER && red > R_WHITE_LOWER && 
              green < G_WHITE_UPPER && green > G_WHITE_LOWER && 
              blue < B_WHITE_UPPER && blue > B_WHITE_LOWER){
     // WHITE
     return 5;
   } else if (red < R_BLACK_UPPER && red > R_BLACK_LOWER && 
              green < G_BLACK_UPPER && green > G_BLACK_LOWER && 
              blue < B_BLACK_UPPER && blue > B_BLACK_LOWER){
     // BLACK
     return 6;
   } else {
     // UNKNOWN COLOR: FAILED TO DETECT ANY OF THE ABOVE COLORS
     return -1;
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
  
  // turn off RGB LED
  led.setColor(OFF_LED);
  led.show();

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
