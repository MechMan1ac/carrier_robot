#include <Arduino.h>
#include <PPMReader.h>

const int interruptPin = 2; // Pin for PPM signal input
const int channelAmount = 10; // Number of channels in the PPM signal

PPMReader ppm(interruptPin, channelAmount); // Create a PPMReader object with the interrupt pin and number of channels

int ch2_value;
int ch4_value;
int ch6_value;

// Motor class  
class Motor {
public:
  Motor(int pin) {
    _pin = pin;
  }

  void write(int value) {
    analogWrite(_pin, value);
  }

private:
  int _pin;
};

Motor leftMotor(3); // Create a Motor object for the left motor on pin 3
Motor rightMotor(6); // Create a Motor object for the right motor on pin 6

#define LEFT_DIR_PIN 7 // Pin for left motor direction control
#define RIGHT_DIR_PIN 8 // Pin for right motor direction control

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LEFT_DIR_PIN, OUTPUT);
  pinMode(RIGHT_DIR_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  ch2_value = ppm.latestValidChannelValue(2, 1500);
  ch4_value = ppm.latestValidChannelValue(4, 1500);
  ch6_value = ppm.latestValidChannelValue(6, 1500);

  if(ch6_value == 1000) { //Autonomous mode enabled
    if (Serial.available()) {
      char c = Serial.read();
      if (c == 'f') { //Move forward
        digitalWrite(LEFT_DIR_PIN, HIGH); // Set left motor direction to forward
        digitalWrite(RIGHT_DIR_PIN, HIGH); // Set right motor direction to forward
        leftMotor.write(128);
        rightMotor.write(128);
      }else if (c == 'r') { //Turn right
        digitalWrite(LEFT_DIR_PIN, HIGH); // Set left motor direction to forward
        digitalWrite(RIGHT_DIR_PIN, LOW); // Set right motor direction to backward
        leftMotor.write(128);
        rightMotor.write(128);
      }else if (c == 'l') { //Turn left
        digitalWrite(LEFT_DIR_PIN, LOW); // Set left motor direction to backward
        digitalWrite(RIGHT_DIR_PIN, HIGH); // Set right motor direction to forward
        leftMotor.write(128);
        rightMotor.write(128);
      }else if (c == 's') { //Stop
        leftMotor.write(0);
        rightMotor.write(0);
      }
    }
  }else { //Autonomous mode disabled
    if(ch2_value > 1500) { //Move forward
      digitalWrite(LEFT_DIR_PIN, HIGH); // Set left motor direction to forward
      digitalWrite(RIGHT_DIR_PIN, HIGH); // Set right motor direction to forward
      leftMotor.write(map(ch2_value, 1500, 2000, 0, 255));
      rightMotor.write(map(ch2_value, 1500, 2000, 0, 255));
    }else if(ch2_value < 1500) { //Move backward
      digitalWrite(LEFT_DIR_PIN, LOW); // Set left motor direction to backward
      digitalWrite(RIGHT_DIR_PIN, LOW); // Set right motor direction to backward
      leftMotor.write(map(ch2_value, 1000, 1500, 255, 0));
      rightMotor.write(map(ch2_value, 1000, 1500, 255, 0));
    }else if (ch4_value > 1500) { //Turn right
      digitalWrite(LEFT_DIR_PIN, HIGH); // Set left motor direction to forward
      digitalWrite(RIGHT_DIR_PIN, LOW); // Set right motor direction to backward
      leftMotor.write(map(ch4_value, 1500, 2000, 0, 255));
      rightMotor.write(map(ch4_value, 1500, 2000, 0, 255));
    }else if (ch4_value < 1500) { //Turn left
      digitalWrite(LEFT_DIR_PIN, LOW); // Set left motor direction to backward
      digitalWrite(RIGHT_DIR_PIN, HIGH); // Set right motor direction to forward
      leftMotor.write(map(ch4_value, 1000, 1500, 255, 0));
      rightMotor.write(map(ch4_value, 1000, 1500, 255, 0));
    }else { //Stop
      leftMotor.write(0);
      rightMotor.write(0);
    }
  }
}
