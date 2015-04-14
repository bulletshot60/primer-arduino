#include <SPI.h>
#include <EnableInterrupt.h>
#include <Wire.h>

#define TURN_LEFT 100
#define TURN_RIGHT 101
#define MOVE_FORWARD 102
#define MOVE_BACKWARD 103

#define RIGHT_DIRECTION_PIN 13
#define LEFT_DIRECTION_PIN 12
#define RIGHT_SPEED_PIN 11
#define LEFT_SPEED_PIN 3
#define RIGHT_BRAKE_PIN 8
#define LEFT_BRAKE_PIN 9
#define RIGHT_WHEEL_SENSOR_PIN 4
#define LEFT_WHEEL_SENSOR_PIN 7
#define RIGHT_SPEED 200
#define LEFT_SPEED 200

#define TURN_RIGHT_ITERATIONS 20
#define MOVE_FORWARD_ITERATIONS 20
#define MOVE_BACKWARD_ITERATIONS 20

int left_moving = 0;
int right_moving = 0;

void setup() {
  Wire.begin(2);
  Wire.onReceive(receiveEvent);

  pinMode(RIGHT_DIRECTION_PIN, OUTPUT);
  pinMode(LEFT_DIRECTION_PIN, OUTPUT);
  pinMode(RIGHT_SPEED_PIN, OUTPUT);
  pinMode(LEFT_SPEED_PIN, OUTPUT);
  pinMode(RIGHT_BRAKE_PIN, OUTPUT);
  pinMode(LEFT_BRAKE_PIN, OUTPUT);
  
  enableInterrupt(RIGHT_WHEEL_SENSOR_PIN, right_brake, RISING);
  enableInterrupt(LEFT_WHEEL_SENSOR_PIN, left_brake, RISING);

  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
}

int command = 0;

void move_left(int amount, int dir) {
  left_moving = amount;
  digitalWrite(LEFT_BRAKE_PIN, LOW);
  digitalWrite(LEFT_DIRECTION_PIN, dir);
  analogWrite(LEFT_SPEED_PIN, LEFT_SPEED);
  while(left_moving > 0) {
    int current = digitalRead(LEFT_WHEEL_SENSOR_PIN);
  }
}
  
void move_right(int amount, int dir) {
  right_moving = amount;
  digitalWrite(RIGHT_BRAKE_PIN, LOW);
  digitalWrite(RIGHT_DIRECTION_PIN, dir);
  analogWrite(RIGHT_SPEED_PIN, RIGHT_SPEED);
  while(right_moving > 0) {
    int current = digitalRead(RIGHT_WHEEL_SENSOR_PIN);
  }
}

void move_left_forward(int amount) {
  move_left(amount, HIGH);
}

void move_left_backward(int amount) {
  move_left(amount, LOW);
}

void move_right_forward(int amount) {
  move_right(amount, LOW);
}

void move_right_backward(int amount) {
  move_right(amount, HIGH);
}

void loop() {
  if(command != 0) {
    if(command == TURN_LEFT) {
      for(int i = 0; i < 7; ++i) {
        move_left_forward(1);
        move_right_backward(1);
      }
    } else if(command == TURN_RIGHT) {
      for(int i = 0; i < 7; ++i) {
        move_right_forward(1);
        move_left_backward(1);
      }
    } else if(command == MOVE_FORWARD) {
      for(int i = 0; i < 10; ++i) {
        move_right_forward(1);
        move_left_forward(1);
      }
    } else if(command == MOVE_BACKWARD) {
      for(int i = 0; i < 10; ++i) {
        move_right_forward(1);
        move_left_forward(1);
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

void right_brake() {
  Serial.println(right_moving);
  Serial.println("right_brake");
  if(right_moving > 0) {
    --right_moving;
  } else {
    digitalWrite(RIGHT_BRAKE_PIN, HIGH);
    digitalWrite(RIGHT_DIRECTION_PIN, LOW);
    analogWrite(RIGHT_SPEED_PIN, 0);
  } 
}

void left_brake() {
  Serial.println(left_moving);
  Serial.println("left_brake");
  if(left_moving > 0) {
    --left_moving;
  } else {
    digitalWrite(LEFT_BRAKE_PIN, HIGH);
    digitalWrite(LEFT_DIRECTION_PIN, LOW);
    analogWrite(LEFT_SPEED_PIN, 0);
  } 
}

void receiveEvent(int howMany) {
  int x = Wire.read(); // receive byte as an integer
  command = x;
}
