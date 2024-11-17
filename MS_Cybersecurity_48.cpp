#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h> // Include Servo library

// Initialize LCD (RS=7, E=6, D4=5, D5=4, D6=3, D7=2)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// Pin definitions
const int redled = 12;
const int greenled = 11;
const int allowedled = 8;
const int notallowedled = 13;
const int trigpin = A3;       // HC-SR04 trigger pin
const int echopin = A4;       // HC-SR04 echo pin
const int buzzerpin = 10;
const int allowedbutton = A1; // Button for "allowed"
const int notallowedbutton = A0; // Button for "not allowed"
const int servoPin = 9;       // Servo motor control pin

long duration; // For ultrasonic timing
int distance;  // Calculated distance

Servo myServo; // Create Servo object

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Doorbell Ready");

  // Set pin modes
  pinMode(greenled, OUTPUT);
  pinMode(redled, OUTPUT);
  pinMode(allowedled, OUTPUT);
  pinMode(notallowedled, OUTPUT);
  pinMode(buzzerpin, OUTPUT);
  pinMode(allowedbutton, INPUT_PULLUP);   // Use pull-up for button
  pinMode(notallowedbutton, INPUT_PULLUP); // Use pull-up for button
  pinMode(echopin, INPUT);
  pinMode(trigpin, OUTPUT);

  // Attach servo motor
  myServo.attach(servoPin);
  myServo.write(0); // Servo starts in the closed position

  // Initial states
  digitalWrite(greenled, HIGH);
  digitalWrite(redled, LOW);
  digitalWrite(allowedled, LOW);
  digitalWrite(notallowedled, LOW);
  digitalWrite(buzzerpin, LOW);
}

void loop() {
  // Trigger ultrasonic sensor
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  // Measure the echo pulse duration
  duration = pulseIn(echopin, HIGH);

  // Calculate distance in cm
  distance = duration * 0.034 / 2;

  // Check if someone is within the threshold (adjust as needed)
  if (distance > 0 && distance < 15) { // Object detected within 15cm
    lcd.clear();
    lcd.print("Person Detected");
    digitalWrite(greenled, LOW);
    digitalWrite(redled, HIGH);
    digitalWrite(buzzerpin, HIGH);

    // Wait for user to press a button
    while (true) {
      // Check for "You are allowed" button press
      if (digitalRead(allowedbutton) == LOW) {
        delay(50); // Debounce delay
        lcd.clear();
        lcd.print("Welcome aboard");
        digitalWrite(buzzerpin, LOW);
        digitalWrite(redled, LOW);
        digitalWrite(allowedled, HIGH);
        digitalWrite(notallowedled, LOW);

        // Open the gate with the servo
        myServo.write(90); // Move servo to the open position
        delay(3000);       // Keep it open for 3 seconds
        myServo.write(0);  // Move servo back to the closed position
        delay(1000);       // Allow time to reset servo

        break;
      }

      // Check for "You are not allowed" button press
      if (digitalRead(notallowedbutton) == LOW) {
        delay(50); // Debounce delay
        lcd.clear();
        lcd.print("GET OUT!");
        digitalWrite(buzzerpin, LOW);
        digitalWrite(redled, LOW);
        digitalWrite(allowedled, LOW);
        digitalWrite(notallowedled, HIGH);
        break;
      }
    }

    // Reset to ready state
    lcd.clear();
    lcd.print("Doorbell Ready");
    digitalWrite(greenled, HIGH);
    digitalWrite(redled, LOW);
    digitalWrite(allowedled, LOW);
    digitalWrite(notallowedled, LOW);
    digitalWrite(buzzerpin, LOW);
  }

  delay(500); // Small delay to avoid rapid re-detection
}
