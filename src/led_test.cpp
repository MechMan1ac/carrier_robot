#include <Arduino.h>
#include <PPMReader.h>

#define interruptPin 2
const int channelCount = 10;

PPMReader ppmReader(interruptPin, channelCount);

#define FORWARD_LED_PIN 5
#define LEFT_LED_PIN 6 
#define RIGHT_LED_PIN 3

void setup() {
  // put your setup code here, to run once:
  pinMode(FORWARD_LED_PIN, OUTPUT);
  pinMode(LEFT_LED_PIN, OUTPUT);
  pinMode(RIGHT_LED_PIN, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  int ch2_val = ppmReader.latestValidChannelValue(2, 1500);
  int ch4_val = ppmReader.latestValidChannelValue(4, 1500);
  int ch6_val = ppmReader.latestValidChannelValue(6, 1500);

  // Serial.print("Channel 2: ");
  // Serial.print(ch2_val);
  // Serial.print(" | Channel 4: ");
  // Serial.print(ch4_val);
  // Serial.print(" | Channel 6: ");
  // Serial.println(ch6_val);

  if (ch6_val == 1000){ //Enable Autonomous Mode
    if (Serial.available() > 0) {
      
      char command = Serial.read();

      if (command == 'f') {
        digitalWrite(FORWARD_LED_PIN, HIGH);
        digitalWrite(LEFT_LED_PIN, LOW);
        digitalWrite(RIGHT_LED_PIN, LOW);
      } else if (command == 'l') {
        digitalWrite(FORWARD_LED_PIN, LOW);
        digitalWrite(LEFT_LED_PIN, HIGH);
        digitalWrite(RIGHT_LED_PIN, LOW);
      } else if (command == 'r') {
        digitalWrite(FORWARD_LED_PIN, LOW);
        digitalWrite(LEFT_LED_PIN, LOW);
        digitalWrite(RIGHT_LED_PIN, HIGH);
      } else if (command == 's') {
        // Stop command received
        digitalWrite(FORWARD_LED_PIN, LOW);
        digitalWrite(LEFT_LED_PIN, LOW);
        digitalWrite(RIGHT_LED_PIN, LOW);
      } else {
        // Invalid command received
        Serial.println("Invalid command. Use FORWARD, LEFT, RIGHT, or STOP.");
      }
    }
  } else { //Disable Autonomous Mode
    digitalWrite(FORWARD_LED_PIN, LOW);
    digitalWrite(LEFT_LED_PIN, LOW);
    digitalWrite(RIGHT_LED_PIN, LOW);

  }  
}
