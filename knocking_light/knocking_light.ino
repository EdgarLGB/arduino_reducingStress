/* Knock Sensor
  
   This sketch reads a piezo element to detect a knocking sound. 
   It reads an analog pin and compares the result to a set threshold. 
   If the result is greater than the threshold, it writes
   "knock" to the serial port, and toggles the LED on pin 13.
  
   The circuit:
	* + connection of the piezo attached to analog in 0
	* - connection of the piezo attached to ground
	* 1-megohm resistor attached from analog in 0 to ground

   http://www.arduino.cc/en/Tutorial/Knock
   
   created 25 Mar 2007
   by David Cuartielles <http://www.0j0.org>
   modified 30 Aug 2011
   by Tom Igoe
   
   This example code is in the public domain.

 */
 

// these constants won't change:
const int ledPin = 3;      // the led acts likes respiration
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 200;  // threshold value to decide when the detected sound is a knock or not

// these variables will change:
int sensorReading = 0;      // variable to store the value read from the sensor pin

void setup() {
 pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT
 Serial.begin(9600);       // use the serial port
}

int lumilosity = 0;
boolean switchoff = false;
const int breathingSpeed = 20;
unsigned long time;
boolean knocked = false;
const int sampling = 12;
void loop() {
  if (!switchoff) {
    // the light is breathing
    analogWrite(ledPin, lumilosity++);
    delay(breathingSpeed);  
  }
  
  if (lumilosity % sampling == 0) {  
    // when the sampling rate is 10 times of breathingSpeed
    // read the sensor and store it in the variable sensorReading:
    sensorReading = analogRead(knockSensor);
  
    Serial.println(sensorReading);  
    
    // if the peizo is knocked
    if (sensorReading >= threshold) {
      // send the string "Knock!" back to the computer, followed by newline
      // we need to detect if it is knocked in 1 secondes.
      unsigned long time1 = millis();
      unsigned long deltaTime = time1 - time;
      if (knocked && deltaTime >=0 && deltaTime <= 500) {
        Serial.println("Knock twice!");
        knocked = false;
        time = 0;
      } else {
        knocked = true;
        time = time1;
      }
  
    }
    
    // when after one second, we say that it is knocked once
    if (knocked) {
      unsigned long time1 = millis();
      unsigned long deltaTime = time1 - time;
      if (deltaTime >= 500) {
        Serial.println("Knock once! Stop the light");
        analogWrite(ledPin, 0);
        while (true) {
          // a die loop for stop the light
        }
      }
    }
  }
}
