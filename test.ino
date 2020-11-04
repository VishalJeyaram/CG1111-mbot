#include <MeMCore.h>

// Define time delay before the next RGB colour turns ON to allow LDR to stabilize
#define RGBWait 200 //in milliseconds 

// Define time delay before taking another LDR reading
#define LDRWait 10 //in milliseconds 

// macros to control rgb
#define RED_LED 255,000,000
#define GREEN_LED 000,255,000
#define BLUE_LED 000,000,255

// initialise peripheral objects
MeLightSensor light_sensor(PORT_6);
MeRGBLed led(PORT_7);  // the internal connection for LED is handled by port 7, with default slot number 2

/* COLOR CODE FOR REFERENCE DURING CALIBRATION
 *  RED -> return 0
   GREEN -> return 1
   YELLOW -> return 2
   PURPLE -> return 3
   BLUE -> return 4 
   WHITE -> return 5
   BLACK -> return 6
   OTHERS -> return -1 */

//floats to hold colour arrays
float colourArray[] = {0,0,0};
float whiteArray[] = {0,0,0};
float blackArray[] = {0,0,0};
float greyDiff[] = {0,0,0};

char colourStr[3][5] = {"R = ", "G = ", "B = "};

//placeholders for colour detected
int red = 0;
int green = 0;
int blue = 0;

void setup() {
  // initialize serial communication at 9600 bps
  Serial.begin(9600);

  setBalance();  // balance calibration
}

void loop() {
  //turn on one colour at a time and LDR reads 5 times
  for(int c = 0;c<=2;c++){    
    Serial.print(colourStr[c]);
    // CODE TO SET R, G OR B LED LIGHT
    if (c == 0) {
      led.setColor(RED_LED);
    } else if (c == 1) {
      led.setColor(GREEN_LED);
    } else {
      led.setColor(BLUE_LED);
    }
    led.show();
    delay(RGBWait);
    
    //get the average of 5 consecutive readings for the current colour and return an average 
    colourArray[c] = getAvgReading(5);
    //the average reading returned minus the lowest value divided by the maximum possible range, multiplied by 255 will give a value between 0-255, representing the value for the current reflectivity (i.e. the colour LDR is exposed to)
    colourArray[c] = (colourArray[c] - blackArray[c])/(greyDiff[c])*255;  // MIGHT NEED TO CHANGE THIS 255 based on scale of this new sensor

    Serial.println(int(colourArray[c])); //show the value for the current colour LED, which corresponds to either the R, G or B of the RGB code
  }
}

void setBalance(){
  //set white balance
  Serial.println("Put White Sample For Calibration ...");
  delay(5000);           //delay for five seconds for getting sample ready
  //scan the white sample.
  //go through one colour at a time, set the maximum reading for each colour -- red, green and blue to the white array

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
    led.show();
    delay(RGBWait);

    whiteArray[j] = getAvgReading(5);         //scan 5 times and return the average, 
    delay(RGBWait);
  }
  
  //done scanning white, time for the black sample.
  //set black balance
  Serial.println("Put Black Sample For Calibration ...");
  delay(5000);     //delay for five seconds for getting sample ready 
  //go through one colour at a time, set the minimum reading for red, green and blue to the black array
  for (int j = 0; j < 3; j++){
    // CODE TO SET R, G OR B LED LIGHT
    if (j == 0) {
      led.setColor(RED_LED);
    } else if (j == 1) {
      led.setColor(GREEN_LED);
    } else {
      led.setColor(BLUE_LED);
    }
    led.show();
    delay(RGBWait);
    blackArray[j] = getAvgReading(5);         //scan 5 times and return the average, 
    delay(RGBWait);
    //the differnce between the maximum and the minimum gives the range
     greyDiff[j] = whiteArray[j] - blackArray[j];
  }

  //delay another 5 seconds for getting ready colour objects
  Serial.println("Colour Sensor Is Ready.");
  delay(5000);
}

int getAvgReading(int times){      
  //find the average reading for the requested number of times of scanning LDR
  int reading;
  int total = 0;
  //take the reading as many times as requested and add them up
  for(int i = 0;i < times;i++){
     reading = light_sensor.read();
     total = reading + total;
     delay(LDRWait);
  }
  //calculate the average and return it
  return total/times;
}
