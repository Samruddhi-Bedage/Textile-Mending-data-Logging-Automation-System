#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"

#define TX_PIN 10 // Arduino transmit (Yellow wire, labeled RX on printer)
#define RX_PIN 13 // Arduino receive (Green wire, labeled TX on printer)

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);

const float wheelRadius = 6.3662;
const int encoderPulsePerRevolution = 39;

volatile long encoderPos = 0;
volatile int lastEncoderA = LOW;
volatile int encoderAPrev = LOW;
volatile bool encoderChanged = false;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 11, 12};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char loomNumber[3];
char fabricQuality[5];
bool takingLoomNumber = true;
bool inputComplete = false;
bool measurementInProgress = false;




void updateEncoder() {
  int encoderA = digitalRead(2);
  int encoderB = digitalRead(3);

  if (encoderA != lastEncoderA) {
    if (encoderB != encoderA) {
      encoderPos++;
    } else {
      encoderPos--;
    }
    encoderChanged = true;
  }
  lastEncoderA = encoderA;
}

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  mySerial.begin(9600);
  printer.begin();

  attachInterrupt(digitalPinToInterrupt(2), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), updateEncoder, CHANGE);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("TEXTILE  MENDING");
  lcd.setCursor(0, 1);
  lcd.print("DATA LOGGING");
  delay(6000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loom No.: ");



}

void printReport() {
  printer.justify('C');
  printer.setSize('M');
  printer.println("Textile Mending Report");
  printer.setSize('S');
  printer.println("Loom No.: " + String(loomNumber));
  printer.println("Fabric Quality: " + String(fabricQuality));
  printer.print("Fabric Length: ");
  Serial.begin(9600);
  Serial.println( String(loomNumber));
   Serial.println( String(fabricQuality));
  

  float distance = (2 * PI * wheelRadius * encoderPos) / (encoderPulsePerRevolution * 100); // Convert cm to meters
  printer.println(String(distance, 2) + " Meters");
 Serial.println( String(distance, 2) + " Meters");
  printer.println("------------------------------");
  printer.feed(3);

 
}

void loop() {
  char key = keypad.getKey();

  if (key != NO_KEY) {
    if (takingLoomNumber) {
      if (isdigit(key) && strlen(loomNumber) < 2) {
        strcat(loomNumber, &key);
      }
      lcd.setCursor(strlen(loomNumber) + 10, 0);
      lcd.print(" ");
      lcd.setCursor(10, 0);
      lcd.print(loomNumber);
      if (strlen(loomNumber) == 2) {
        takingLoomNumber = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Qual.:");
      }
    } else {
      if (!inputComplete) {
        if (isdigit(key) && strlen(fabricQuality) < 4) {
          strcat(fabricQuality, &key);
        }
        lcd.setCursor(strlen(fabricQuality) + 10, 1);
        lcd.print(" ");
        lcd.setCursor(10, 1);
        lcd.print(fabricQuality);
        if (strlen(fabricQuality) == 4) {
          inputComplete = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Loom No.: ");
          lcd.print(loomNumber);
          lcd.setCursor(0, 1);
          lcd.print("Qual.: ");
          lcd.print(fabricQuality);
          delay(2000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("measuring...: ");
          measurementInProgress = true;
        }
      }
    }
  }

  if (measurementInProgress) {
    updateMeasurementDisplay();
    delay(100);
  }

  if (key == 'D' && measurementInProgress) {
    printReport();
    loomNumber[0] = '\0';
    fabricQuality[0] = '\0';
    encoderPos = 0;
    takingLoomNumber = true;
    inputComplete = false;
    measurementInProgress = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Loom No.: ");
  }
}

void updateMeasurementDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FABRIC: ");

  float distance = (2 * PI * wheelRadius * encoderPos) / (encoderPulsePerRevolution * 100); // Convert cm to meters
  lcd.setCursor(5, 1);
  lcd.print(distance, 2);
  lcd.print(" Meter");
}
