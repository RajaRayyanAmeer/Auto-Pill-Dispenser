#include <Servo.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

Servo servo;
RTC_DS3231 rtc;

const int servoPin = 6;
const int buttonPin = 9;
const int ledPin = 7;
const int buzzerPin = 8;

LiquidCrystal_I2C lcd(0x27, 16, 2);'\
'

int angle = 0;
const int angleStep = 60;
int dispenseCount = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  servo.attach(servoPin);
  servo.write(angle);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.setCursor(0, 1);
    lcd.print("RTC Error!");
    while (1);
  }

  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (dispenseCount >= 3) return;

  // Step 1: 15 seconds idle
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Manual Dispenser");
  delay(15000);

  // Step 2: 60 seconds alert
  unsigned long alertStart = millis();
  bool buttonPressed = false;

  while (millis() - alertStart < 60000) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Take Medicine");
    updateTime();

    delay(300);

    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    delay(300);

    // Extracted push button handling from Program 1
    if (digitalRead(buttonPin) == HIGH && !buttonPressed) {
      buttonPressed = true;

      // Wait until the button is released and pressed again
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Take medicine!");
      for (int i = 0; i < 4; i++) {
        digitalWrite(ledPin, HIGH);
        digitalWrite(buzzerPin, HIGH);
        delay(150);
        digitalWrite(ledPin, LOW);
        digitalWrite(buzzerPin, LOW);
        delay(150);
      }

      // Wait for button press (blocking like Program 1)
      while (digitalRead(buttonPin) == LOW) {}
      delay(1000); // debounce delay

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Manual Dispense");
      rotateServo();
    }
  }

  // Step 4: Rotate automatically if button not pressed
  if (!buttonPressed) {
    rotateServo();
  }

  // Step 5: 60 seconds delay
  delay(60000);

  // Step 6: Repeat until 180°
  dispenseCount++;
}

void rotateServo() {
  angle += angleStep;
  if (angle > 180) angle = 180;
  servo.write(angle);
  delay(500);
}

void updateTime() {
  DateTime now = rtc.now();
  lcd.setCursor(0, 1);
  printTwoDigits(now.hour());
  lcd.print(':');
  printTwoDigits(now.minute());
  lcd.print(':');
  printTwoDigits(now.second());
}

void printTwoDigits(int number) {
  if (number < 10) lcd.print('0');
  lcd.print(number);
}