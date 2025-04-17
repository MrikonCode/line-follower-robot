#include <AFMotor.h>
#include <SoftwareSerial.h>

// Мотори
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

// IR сензори
const int IR_LEFT = A0;c:\Users\HP\Documents\Arduino\sketch_apr17a
const int IR_CENTER = A1;
const int IR_RIGHT = A2;

// Bluetooth
SoftwareSerial BT(10, 11); // RX, TX

// Променливи
int leftValue, centerValue, rightValue;
bool manualMode = true;  // Стартово ръчен режим

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_CENTER, INPUT);
  pinMode(IR_RIGHT, INPUT);
}

void loop() {
  if (BT.available()) {
    char command = BT.read();
    handleBluetooth(command);
  }

  if (!manualMode) {
    followLine();
  }
}

void handleBluetooth(char cmd) {
  switch (cmd) {
    case 'A': manualMode = false; break; // Автоматичен режим
    case 'S': stopMotors(); break;       // Стоп
    case 'F': moveForward(); break;
    case 'B': moveBackward(); break;
    case 'L': turnLeft(); break;
    case 'R': turnRight(); break;
    default: stopMotors(); break;
  }
}

void followLine() {
  readIRSensors();

  if (centerValue == 1 && leftValue == 0 && rightValue == 0) {
    moveForward();
  } else if (leftValue == 1) {
    turnLeft();
  } else if (rightValue == 1) {
    turnRight();
  } else {
    stopMotors();
  }
}

void readIRSensors() {
  leftValue = digitalRead(IR_LEFT);
  centerValue = digitalRead(IR_CENTER);
  rightValue = digitalRead(IR_RIGHT);
}

void moveForward() {
  setMotors(150, 150, FORWARD);
}

void moveBackward() {
  setMotors(150, 150, BACKWARD);
}

void turnLeft() {
  setMotors(100, 180, FORWARD);
}

void turnRight() {
  setMotors(180, 100, FORWARD);
}

void stopMotors() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void setMotors(int leftSpeed, int rightSpeed, int direction) {
  motor1.setSpeed(leftSpeed);
  motor2.setSpeed(leftSpeed);
  motor3.setSpeed(rightSpeed);
  motor4.setSpeed(rightSpeed);

  motor1.run(direction);
  motor2.run(direction);
  motor3.run(direction);
  motor4.run(direction);
}
