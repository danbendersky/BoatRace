#include "arduino_secrets.h"

#include <Arduino.h>
#include <TM1637Display.h>
/*
  
*/
//Serial display port 8
#define CLK 9
#define DIO 8

// The amount of time (in milliseconds) between tests
#define TEST_DELAY   1

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};

TM1637Display display(CLK, DIO);


const int pingPinStartOut = 3;
const int pingPinStartIn = 4;
const int pingPinEndOut = 10;
const int pingPinEndIn = 11;
const int startDistanceMeasurement = 8;
const int endDistanceMeasurement = 10;

//Time is in centiseconds
int time;
int startTime;


void setup() {
    Serial.begin(9600);
    // Must begin s7s software serial at the correct baud rate.
    //  The default of the s7s is 9600.

  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
  display.setBrightness(0x0f);

  // All segments on
  display.setSegments(data);
  
  // Clear the display before jumping into loop
  display.showNumberDec(1234, false);
  delay(1000);
  display.clear();
}

void loop() {
    startTime = millis() / 10;
    if (getDistanceInches(pingPinStartOut, pingPinStartIn) < startDistanceMeasurement){
      while (true){
        if (getDistanceInches(pingPinEndOut, pingPinEndIn) < endDistanceMeasurement){
          break;
        }
        delay(10);
        time = (millis()/10) - startTime;
        displayTime(time);
      }
    } else {
      delay(10);
    }
}

long getDistanceInches(int pingPinOut, int pingPinIn){
  long duration;
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPinOut, OUTPUT);
  digitalWrite(pingPinOut, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPinOut, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPinOut, LOW);

  pinMode(pingPinIn, INPUT);
  duration = pulseIn(pingPinIn, HIGH);

  // convert the time into a distance
  return microsecondsToInches(duration);
}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are 73.746
  // microseconds per inch (i.e. sound travels at 1130 feet per second).
  // This gives the distance travelled by the ping, outbound and return,
  // so we divide by 2 to get the distance of the obstacle.
  // See: https://www.parallax.com/package/ping-ultrasonic-distance-sensor-downloads/
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}

void displayTime(int centiseconds){

  Serial.println(centiseconds);
  display.showNumberDecEx(centiseconds, 0b01000000, false);  // Expect: 0001
}

