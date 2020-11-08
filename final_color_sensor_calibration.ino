#include <MeMCore.h>

// number of samples for color calibration
#define NO_SAMPLES 30

// response time of light sensor in ms (need to check this value using response_time.ino)
#define RESPONSE_TIME 100

// values to control RGB LED
#define RED_LED 255,000,000
#define GREEN_LED 000,255,000
#define BLUE_LED 000,000,255
#define OFF_LED 000,000,000

// initialise peripheral objects
MeLightSensor light_sensor(PORT_6);
MeRGBLed led(PORT_7);  // the internal connection for LED is handled by port 7, with default slot number 2

// contains details for red, green and blue channels
struct Color {
  int red;
  int green;
  int blue;
};

/* COLOR CODE FOR REFERENCE DURING CALIBRATION
 * RED -> return 0
   GREEN -> return 1
   YELLOW -> return 2
   PURPLE -> return 3
   BLUE -> return 4 
   WHITE -> return 5
   BLACK -> return 6
   OTHERS -> return -1 */

void setup() {
  // initialize serial communication at 9600 bps
  Serial.begin(9600);

  // perform color calibration first
  Serial.println("Place white sample for calibration...");
  delay(5000);
  Color white = get_colors();
  Serial.print(white.red);
  Serial.print("\t");
  Serial.print(white.green);
  Serial.print("\t");
  Serial.println(white.blue);
  Serial.println();

  Serial.println("Place black sample for calibration...");
  delay(5000);
  Color black = get_colors();
  Serial.print(black.red);
  Serial.print("\t");
  Serial.print(black.green);
  Serial.print("\t");
  Serial.println(black.blue);
  Serial.println();
  Serial.println("Finished calibration");

  // take red values
  Serial.println("Place red sample for identification...");
  delay(5000);
  Color red = get_colors();
  Serial.print(red.red);
  Serial.print("\t");
  Serial.print(red.green);
  Serial.print("\t");
  Serial.println(red.blue);
  Serial.println();

  // take green values
  Serial.println("Place green sample for identification...");
  delay(5000);
  Color green = get_colors();
  Serial.print(green.red);
  Serial.print("\t");
  Serial.print(green.green);
  Serial.print("\t");
  Serial.println(green.blue);
  Serial.println();
  
  // take yellow values
  Serial.println("Place yellow sample for identification...");
  delay(5000);
  Color yellow = get_colors();
  Serial.print(yellow.red);
  Serial.print("\t");
  Serial.print(yellow.green);
  Serial.print("\t");
  Serial.println(yellow.blue);
  Serial.println();

  // take purple values
  Serial.println("Place purple sample for identification...");
  delay(5000);
  Color purple = get_colors();
  Serial.print(purple.red);
  Serial.print("\t");
  Serial.print(purple.green);
  Serial.print("\t");
  Serial.println(purple.blue);
  Serial.println();

  // take blue values
  Serial.println("Place blue sample for identification...");
  delay(5000);
  Color blue = get_colors();
  Serial.print(blue.red);
  Serial.print("\t");
  Serial.print(blue.green);
  Serial.print("\t");
  Serial.println(blue.blue);
  Serial.println();

  Serial.println("Completed all!");
}

void loop() {
  // nothing here. everything done only once
}

int get_sample(){
  // takes 5 samples and returns the average
  int no_samples = 5;
  int sum = 0;
  delay(RESPONSE_TIME);
  
  for (int i = 0; i < no_samples; i++){
    sum += light_sensor.read();
  }
  
  return (sum / no_samples);
}

Color get_colors() {
  // takes NO_SAMPLES samples and returns the average for each channel
  Color new_color;

  // set RGB LED to RED
  led.setColor(RED_LED);
  led.show();
  new_color.red = get_single_color();
 
  // set RGB LED to GREEN
  led.setColor(GREEN_LED);
  led.show();
  new_color.green = get_single_color();

  // set RGB LED to BLUE
  led.setColor(BLUE_LED);
  led.show();
  new_color.blue = get_single_color();
  
  // turn off RGB LED
  led.setColor(OFF_LED);
  led.show();

  return new_color;
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
