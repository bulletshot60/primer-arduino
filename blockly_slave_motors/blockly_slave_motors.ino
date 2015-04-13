#include <SPI.h>
#include <EnableInterrupt.h>
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
#define SPEED 100

#define TURN_LEFT_ITERATIONS 50
#define TURN_RIGHT_ITERATIONS 50
#define MOVE_FORWARD_ITERATIONS 80
#define MOVE_BACKWARD_ITERATIONS 80

int left_moving = 0;
int right_moving = 1;

void setup() {
  Wire.begin(2);
  Wire.onReceive(receiveEvent);

  pinMode(RIGHT_DIRECTION_PIN, OUTPUT);
  pinMode(LEFT_DIRECTION_PIN, OUTPUT);
  pinMode(RIGHT_SPEED_PIN, OUTPUT);
  pinMode(LEFT_SPEED_PIN, OUTPUT);
  pinMode(RIGHT_BRAKE_PIN, OUTPUT);
  pinMode(LEFT_BRAKE_PIN, OUTPUT);
  
  enableInterrupt(1, brake, CHANGE);
  enableInterrupt(2, brake, CHANGE);

  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
}

int command = 0;

void loop() {
  if(command != 0) {

    if(command == TURN_LEFT) { //RIGHT HIGH, LEFT HIGH
      for(int i = 0; i < TURN_LEFT_ITERATIONS; ++i) {
        left_moving = 1;
        digitalWrite(LEFT_BRAKE_PIN, LOW);
        digitalWrite(LEFT_DIRECTION_PIN, HIGH);
        analogWrite(LEFT_SPEED_PIN, SPEED);
        while(left_moving == 1) { delay(10); }
        
        right_moving = 1;
        digitalWrite(RIGHT_BRAKE_PIN, LOW);
        digitalWrite(RIGHT_DIRECTION_PIN, HIGH);
        analogWrite(RIGHT_SPEED_PIN, SPEED);
        while(right_moving == 1) { delay(10); }
      }
    } else if (command == TURN_RIGHT) {  //RIGHT LOW, LEFT LOW
      for(int i = 0; i < TURN_RIGHT_ITERATIONS; ++i) {
        left_moving = 1;
        digitalWrite(LEFT_BRAKE_PIN, LOW);
        digitalWrite(LEFT_DIRECTION_PIN, LOW);
        analogWrite(LEFT_SPEED_PIN, SPEED);
        while(left_moving == 1) { delay(10); }
        
        right_moving = 1;
        digitalWrite(RIGHT_BRAKE_PIN, LOW);
        digitalWrite(RIGHT_DIRECTION_PIN, LOW);
        analogWrite(RIGHT_SPEED_PIN, SPEED);
        while(right_moving == 1) { delay(10); }
      }
    } else if (command == MOVE_FORWARD) { //RIGHT HIGH, LEFT LOW
      for(int i = 0; i < MOVE_FORWARD_ITERATIONS; ++i) {
        left_moving = 1;
        digitalWrite(LEFT_BRAKE_PIN, LOW);
        digitalWrite(LEFT_DIRECTION_PIN, LOW);
        analogWrite(LEFT_SPEED_PIN, SPEED);
        while(left_moving == 1) { delay(10); }
        
        right_moving = 1;
        digitalWrite(RIGHT_BRAKE_PIN, LOW);
        digitalWrite(RIGHT_DIRECTION_PIN, HIGH);
        analogWrite(RIGHT_SPEED_PIN, SPEED);
        while(right_moving == 1) { delay(10); }
      }
    } else if (command == MOVE_BACKWARD) { //RIGHT LOW, LEFT HIGH
      for(int i = 0; i < MOVE_BACKWARD_ITERATIONS; ++i) {
        left_moving = 1;
        digitalWrite(LEFT_BRAKE_PIN, LOW);
        digitalWrite(LEFT_DIRECTION_PIN, HIGH);
        analogWrite(LEFT_SPEED_PIN, SPEED);
        while(left_moving == 1) { delay(10); }
        
        right_moving = 1;
        digitalWrite(RIGHT_BRAKE_PIN, LOW);
        digitalWrite(RIGHT_DIRECTION_PIN, LOW);
        analogWrite(RIGHT_SPEED_PIN, SPEED);
        while(right_moving == 1) { delay(10); }
      }
    }
    
    //alert master that we are done
    Wire.beginTransmission(1);
    Wire.write(1);
    Wire.endTransmission();

    command = 0;
  }
  delay(5000);
}

void brake() {
  if(right_moving == 1) {
    digitalWrite(RIGHT_BRAKE_PIN, HIGH);
    digitalWrite(RIGHT_DIRECTION_PIN, LOW);
    analogWrite(RIGHT_SPEED_PIN, 0);
    right_moving = 0;
  } else if (left_moving == 1) {
    digitalWrite(LEFT_DIRECTION_PIN, LOW);
    digitalWrite(LEFT_BRAKE_PIN, HIGH);  
    analogWrite(LEFT_SPEED_PIN, 0);
    left_moving = 0;
  }
}

void receiveEvent(int howMany) {
  int x = Wire.read(); // receive byte as an integer
  command = x;
}
