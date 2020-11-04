#include <MeMCore.h>

// number of samples for color calibration
#define NO_SAMPLES 10

// response time of light sensor in ms (need to check this value using response_time.ino)
#define RESPONSE_TIME 10

// macros to control rgb
#define RED_LED 255,000,000
#define GREEN_LED 000,255,000
#define BLUE_LED 000,000,255

// initialise peripheral objects
MeLightSensor light_sensor(PORT_6);
MeRGBLed led(PORT_7);  // the internal connection for LED is handled by port 7, with default slot number 2

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

  // read the light sensor values
  int colorArray[7][NO_SAMPLES][3];

  // iterate through the different colors
  for (int i = 0; i < 7; i++) {
    // print color papers status
    Serial.print("Starting color ");
    Serial.println(i);

    // iterate through the different color channels
    for (int j = 0; j < 3; j++){
      // CODE TO SET R, G OR B LED LIGHT
      if (j == 0) {
        led.setColor(RED_LED);
      } else if (j == 1) {
        led.setColor(GREEN_LED);
      } else {
        led.setColor(BLUE_LED);
      }
      delay(200);
      led.show();

      // print color channel being used
      Serial.print("Using color channel ");
      Serial.println(j);
      
      // iterate through the different samples
      for (int k = 0; k < NO_SAMPLES; k++){
        colorArray[i][j][k] = get_sample();   // get average of 5 sensor readings

//        // print progress
//        if (j%10 == 0){
//          Serial.print("#");
//        }
      }
    }
    Serial.println();
    Serial.println("Insert next color paper for calibration");
    delay(100);  // wait 7s before taking next measurements to give enough time for color paper change
  }
  Serial.println("Completed all");
  Serial.println();
  Serial.println();
  Serial.println();

  // prints results in the following format <r0> <g0> <b0> <r1> ... <r6> <g6> <b6> 
  // in comma seperated values form
  for (int i = 0; i < 7; i++){
    Serial.print("Results for color ");
    Serial.println(i);
    for (int j = 0; j < NO_SAMPLES; j++){
      for (int k = 0; k < 3; k++){
        Serial.print(colorArray[i][j][k]);
        Serial.print(",");
      }
      // print the next set of values in another row
      Serial.println();
    }
    Serial.println();
    Serial.println();
  }
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
