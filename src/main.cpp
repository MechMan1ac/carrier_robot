#include <Arduino.h>
#include <PPMReader.h>

const int interruptPin = 2; // Pin for PPM signal input
const int channelAmount = 10; // Number of channels in the PPM signal

PPMReader ppm(interruptPin, channelAmount); // Create a PPMReader object with the interrupt pin and number of channels

// LED pins
#define FORWARD_LED_PIN 5
#define RIGHT_LED_PIN 3
#define LEFT_LED_PIN 6

const int pulseMin = 1000;  // microseconds (us)
const int pulseMax = 2000;
const int timeout = 1;  // microseconds (us)

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
    value = map(value, 1000, 2000, 0, 255); // Map the value from the range of 1000-2000 to 0-255
    value = constrain(value, 0, 255); // Ensure the value is within the range of 0-255
    analogWrite(_pin, value);
  }

private:
  int _pin;
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(FORWARD_LED_PIN, OUTPUT);
  pinMode(RIGHT_LED_PIN, OUTPUT);
  pinMode(LEFT_LED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  ch2_value = ppm.latestValidChannelValue(2, 1500);
  ch4_value = ppm.latestValidChannelValue(4, 1500);
  ch6_value = ppm.latestValidChannelValue(6, 1500);

  // Serial.print("CH2: ");
  // Serial.print(ch2_value);
  // Serial.print(" CH4: ");
  // Serial.print(ch4_value);
  // Serial.print(" CH6: ");
  // Serial.println(ch6_value);

  if(ch6_value == 1000) { //Autonomous mode enabled
    if (Serial.available()) {
      char c = Serial.read();
      if (c == 'f') { //Move forward
        analogWrite(FORWARD_LED_PIN, 255);
        analogWrite(RIGHT_LED_PIN, 0);
        analogWrite(LEFT_LED_PIN, 0);
      }else if (c == 'r') { //Turn right
        analogWrite(RIGHT_LED_PIN, 255);
        analogWrite(FORWARD_LED_PIN, 0);
        analogWrite(LEFT_LED_PIN, 0);
      }else if (c == 'l') { //Turn left
        analogWrite(LEFT_LED_PIN, 255);
        analogWrite(FORWARD_LED_PIN, 0);
        analogWrite(RIGHT_LED_PIN, 0);
      }else if (c == 's') {
        analogWrite(FORWARD_LED_PIN, 0);
        analogWrite(RIGHT_LED_PIN, 0);
        analogWrite(LEFT_LED_PIN, 0);
      }
    }
  }else { //Autonomous mode disabled
    analogWrite(FORWARD_LED_PIN, 0);
    analogWrite(RIGHT_LED_PIN, 0);
    analogWrite(LEFT_LED_PIN, 0);
  }
}

