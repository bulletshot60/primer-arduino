#include <SPI.h>
#include <Wire.h>

#define TURN_LEFT 100
#define TURN_RIGHT 101
#define MOVE_FORWARD 102
#define MOVE_BACKWARD 103

#define RIGHT_DIRECTION_PIN 12
#define LEFT_DIRECTION_PIN 13
#define RIGHT_SPEED_PIN 3
#define LEFT_SPEED_PIN 11
#define RIGHT_BRAKE_PIN 9
#define LEFT_BRAKE_PIN 8

void setup() {
  Wire.begin(2);
  Wire.onReceive(receiveEvent);

  pinMode(RIGHT_DIRECTION_PIN, OUTPUT);
  pinMode(LEFT_DIRECTION_PIN, OUTPUT);
  pinMode(RIGHT_SPEED_PIN, OUTPUT);
  pinMode(LEFT_SPEED_PIN, OUTPUT);
  pinMode(RIGHT_BRAKE_PIN, OUTPUT);
  pinMode(LEFT_BRAKE_PIN, OUTPUT);

  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
}

int command = 0;

void loop() {
  if(command != 0) {

    if(command == TURN_LEFT) {
      digitalWrite(RIGHT_DIRECTION_PIN, HIGH);
      digitalWrite(LEFT_DIRECTION_PIN, HIGH);
      analogWrite(RIGHT_SPEED_PIN, 150);
      analogWrite(LEFT_SPEED_PIN, 150);
      digitalWrite(RIGHT_BRAKE_PIN, LOW);
      digitalWrite(LEFT_BRAKE_PIN, LOW);
    } else if(command == TURN_RIGHT) {
      digitalWrite(RIGHT_DIRECTION_PIN, LOW);
      digitalWrite(LEFT_DIRECTION_PIN, LOW);
      analogWrite(RIGHT_SPEED_PIN, 150);
      analogWrite(LEFT_SPEED_PIN, 150);
      digitalWrite(RIGHT_BRAKE_PIN, LOW);
      digitalWrite(LEFT_BRAKE_PIN, LOW);
    } else if(command == MOVE_FORWARD) {
      digitalWrite(RIGHT_DIRECTION_PIN, HIGH);
      digitalWrite(LEFT_DIRECTION_PIN, LOW);
      analogWrite(RIGHT_SPEED_PIN, 150);
      analogWrite(LEFT_SPEED_PIN, 150);
      digitalWrite(RIGHT_BRAKE_PIN, LOW);
      digitalWrite(LEFT_BRAKE_PIN, LOW);
    } else if(command == MOVE_BACKWARD) {
      digitalWrite(RIGHT_DIRECTION_PIN, LOW);
      digitalWrite(LEFT_DIRECTION_PIN, HIGH);
      analogWrite(RIGHT_SPEED_PIN, 150);
      analogWrite(LEFT_SPEED_PIN, 150);
      digitalWrite(RIGHT_BRAKE_PIN, LOW);
      digitalWrite(LEFT_BRAKE_PIN, LOW);
    }
		
    delay(1000);
		
    digitalWrite(RIGHT_DIRECTION_PIN, LOW);
    digitalWrite(LEFT_DIRECTION_PIN, LOW);
    analogWrite(RIGHT_SPEED_PIN, 0);
    analogWrite(LEFT_SPEED_PIN, 0);
    digitalWrite(RIGHT_BRAKE_PIN, HIGH);
    digitalWrite(LEFT_BRAKE_PIN, HIGH);

    Wire.beginTransmission(1);
    Wire.write(1);
    Wire.endTransmission();

    command = 0;
  }
  delay(5000);
}

void receiveEvent(int howMany) {
  int x = Wire.read(); // receive byte as an integer
  command = x;
}
