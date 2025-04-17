
#include <AFMotor.h>
#include <SoftwareSerial.h>

// === Мотори (канали на L298N) ===
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

// === IR сензори ===
const int IR_LEFT = A0;
const int IR_CENTER = A1;
const int IR_RIGHT = A2;

// === Ултразвуков сензор (по избор) ===
const int TRIG_PIN = 6;
const int ECHO_PIN = 7;

// === Bluetooth модул ===
SoftwareSerial BT(10, 11); // RX, TX

// === Променливи ===
int leftValue, centerValue, rightValue;
bool manualMode = true;  // Стартово ръчен режим
unsigned long lastCommandTime = 0;
const unsigned long commandTimeout = 3000; // 3 секунди

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_CENTER, INPUT);
  pinMode(IR_RIGHT, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  stopMotors();
}

void loop() {
  if (BT.available()) {
    char command = BT.read();
    handleBluetooth(command);
    lastCommandTime = millis();
  }

  if (!manualMode) {
    if (checkObstacle()) {
      stopMotors();
    } else {
      followLine();
    }
  }

  if (manualMode && millis() - lastCommandTime > commandTimeout) {
    stopMotors();  // Автоматично спиране при липса на команда
  }
}

// === Обработка на Bluetooth команди ===
void handleBluetooth(char cmd) {
  switch (cmd) {
    case 'A': manualMode = false; break;
    case 'S': stopMotors(); break;
    case 'F': moveForward(); break;
    case 'B': moveBackward(); break;
    case 'L': turnLeft(); break;
    case 'R': turnRight(); break;
    case 'M': manualMode = true; break;
    default: stopMotors(); break;
  }
}

// === Следене на линия ===
void followLine() {
  readIRSensors();

  if (centerValue == 1 && leftValue == 0 && rightValue == 0) {
    moveForward();
  } else if (leftValue == 1 && centerValue == 0) {
    slightLeft();
  } else if (rightValue == 1 && centerValue == 0) {
    slightRight();
  } else {
    stopMotors();
  }
}

// === Четене на IR сензори ===
void readIRSensors() {
  leftValue = digitalRead(IR_LEFT);
  centerValue = digitalRead(IR_CENTER);
  rightValue = digitalRead(IR_RIGHT);
}

// === Движение ===
void moveForward() {
  setMotors(180, 180, FORWARD);
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

void slightLeft() {
  setMotors(130, 180, FORWARD);
}

void slightRight() {
  setMotors(180, 130, FORWARD);
}

void stopMotors() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

// === Унифицирано управление на мотори ===
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

// === Проверка за препятствие (по избор) ===
bool checkObstacle() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;

  return (distance > 0 && distance < 10); // препятствие под 10 см
}
