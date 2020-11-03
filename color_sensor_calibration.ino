#include <MeMCore.h>

// number of samples for color calibration
#define NO_SAMPLES 100

// delay to change color
#define RGBWait 200

// response time of light sensor in ms (need to check this value using response_time.ino)
#define RESPONSE_TIME 10

// initialise peripheral objects
MeLightSensor light_sensor(PORT_6);

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
      // NEED TO ADD CODE TO SET R, G OR B LED LIGHT
      //
      // delay(RGBWait);

      // print color channel being used
      Serial.print("Using color channel ");
      Serial.print(j);
      
      // iterate through the different samples
      for (int k = 0; k < NO_SAMPLES; k++){
        colorArray[i][j][k] = get_sample();   // get average of 5 sensor readings

        // print progress
        if (j%10 == 0){
          Serial.print("#");
        }
      }
    }

    Serial.println("Insert next color paper for calibration");
    delay(7000);  // wait 7s before taking next measurements to give enough time for color paper change
  }
  Serial.println("Completed all");
  Serial.println();
  Serial.println();
  Serial.println();

  // prints results in the following format <r0> <g0> <b0> <r1> ... <r6> <g6> <b6> 
  // in comma seperated values form
  for (int i = 0; i < NO_SAMPLES; i++){
    for (int j = 0; j < 7; j++){
      for (int k = 0; k < 3; k++){
        Serial.print(colorArray[j][i][k]);
        Serial.print(",");
      }
    }
    // print the next set of values in another row
    Serial.print();
  }
}

void loop() {
  // nothing here. everything done only once
  break;
}

int get_sample(){
  // takes 5 samples and returns the average
  int no_samples = 5;
  int sum = 0;

  for (int i = 0; i < no_samples; i++){
    delay(RESPONSE_TIME); // NEED TO CHECK IF THIS IS REALLY REQUIRED
    sum += light_sensor.read();
  }
  return (sum / no_samples);
}
