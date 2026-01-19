#include <Wire.h>;
#include <Adafruit_PWMServoDriver.h>;

int potBase = A0;
int potShoulder = A1;
int potElbow = A2;
int potWrist = A3;

int baseServo = 15;
int shoulder = 14;
int elbow = 13;
int wrist = 12;
int hand = 11;

int buttonPin = 13;

const double WRIST_STARTING_ANGLE = 231;
const double ELBOW_STARTING_ANGLE = 46;
const double SHOULDER_STARTING_ANGLE = 230; // 42 - 230
const double BASE_STARTING_ANGLE = 233;

const int SAMPLE_ARRAY_LENGTH = 5;

int errorCount = 0;
const int ERROR_LIMIT = 100;
bool functional = true;

double wristAngleArray[5];
double elbowAngleArray[5];
double shoulderAngleArray[5];
double baseAngleArray[5];

Adafruit_PWMServoDriver servoDriver = Adafruit_PWMServoDriver();
// pin number - 11 gives corresponding array entry
 int SERVOMIN[] {118, 550, 560, 185, 560};
 int SERVOMAX[] {543, 122, 130, 570, 120};

// int SERVOMIN = 120; 
// int SERVOMAX = 545;

#define SERVO_FREQ 50

void setup() {
  Serial.begin(9600);

  servoDriver.begin();
  servoDriver.setOscillatorFrequency(25000000);
  servoDriver.setPWMFreq(SERVO_FREQ);

  pinMode(buttonPin, INPUT_PULLUP);
  // moveServo(wrist, 0);
  // delay(1000);
  // moveServo(hand, 0);
  // delay(1000);
  // moveServo(elbow, 0);
  // delay(1000);
  // moveServo(shoulder, 0);
  // delay(1000);
  // moveServo(baseServo, 0);
  // delay(10000);

    // moveServo(hand, 0);
  // moveServo(wrist, 0);

  // for (int i = 0; i <= 180; i++) {
  //   moveServo(hand, i);
  //   delay(1);
  // }
  // delay(5000);

  // for (int i = 0; i <= 180; i++) {
  //   moveServo(wrist, i);
  //   delay(2);
  // }

  // moveServo(elbow, 0);
  // moveServo(shoulder, 0);
  // moveServo(baseServo, 90);
  // moveServo(wrist, 0);
  // moveServo(hand, 0);
}

void loop() {
  if (functional) {
    moveServo(wrist, readAngle(potWrist));
    moveServo(elbow, readAngle(potElbow));
    moveServo(shoulder, readAngle(potShoulder));
    moveServo(baseServo, readAngle(potBase));

    // Serial.println(readAngle(potElbow));

    if (digitalRead(buttonPin) == 0) {
      moveServo(hand, 40);
    } else {
      moveServo(hand, 0);
    }
  }
}

double readAngle(int potPin) {
  int reading = analogRead(potPin);
  if ((reading == 0) || (reading == 1023)) {
    errorCount += 1;
    if (errorCount >= ERROR_LIMIT) {
      functional = false;
    }

    if (potPin == potBase) {
      return averageArray(baseAngleArray);
    } else if (potPin == potShoulder) {
      return averageArray(shoulderAngleArray); 
    } else if (potPin == potElbow) {
      return averageArray(elbowAngleArray);
    } else if (potPin == potWrist) {
      return averageArray(wristAngleArray);
    }
  } else {
    errorCount = 0;
    double angleReading = reading * (270.0)/(1023.0);
    if (potPin == potBase) {
      updateArray(baseAngleArray, BASE_STARTING_ANGLE - angleReading); // base is inverted
      return averageArray(baseAngleArray);

    } else if (potPin == potShoulder) {
      updateArray(shoulderAngleArray, SHOULDER_STARTING_ANGLE - angleReading); // shoulder is inverted
      if ((averageArray(shoulderAngleArray) < 180) && (averageArray(shoulderAngleArray) > 0)) {
        return averageArray(shoulderAngleArray);
      }

    } else if (potPin == potElbow) {
      updateArray(elbowAngleArray, angleReading - ELBOW_STARTING_ANGLE);
      Serial.println(averageArray(elbowAngleArray));
      return averageArray(elbowAngleArray);

    } else if (potPin == potWrist) {
      updateArray(wristAngleArray, WRIST_STARTING_ANGLE - angleReading);
      return averageArray(wristAngleArray);
    }
  }
}

void updateArray(double array[], double value) {
  for (int i = 1; i < SAMPLE_ARRAY_LENGTH; i++) {
    array[i-1] = array[i];
  }
  array[SAMPLE_ARRAY_LENGTH - 1] = value;
}

double averageArray(double array[]) {
  double sum = 0;
  for (int i = 0; i < sizeof(array); i++) {
    sum += array[i];
  }
  return (sum/sizeof(array));
}

void moveServo(int pin, double angle) {
  // double pulseLength = map(degree, 0, 180, SERVOMIN[pin - 11], SERVOMAX[pin - 11]);
  // Serial.println(angle);
  double pulseLength = map(angle, 0, 180, SERVOMIN[pin - 11], SERVOMAX[pin - 11]);
  // Serial.println(pulseLength);
  servoDriver.setPWM(pin, 0, pulseLength);
}

