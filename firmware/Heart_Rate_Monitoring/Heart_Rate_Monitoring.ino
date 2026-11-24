#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int PulseWire = 0;       // PulseSensor signal connected to Analog Pin A0
const int LED13 = 13;          // On-board / external pulse indicator LED on D13
int Threshold = 550;           // ADC beat-detection threshold (0-1023)
int buzzerPin = 9;             // Relay & low-BPM alarm driver on D9

PulseSensorPlayground pulseSensor;
LiquidCrystal_I2C lcd(0x20, 20, 4);

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);

  pulseSensor.analogInput(PulseWire);
  pulseSensor.blinkOnPulse(LED13);
  pulseSensor.setThreshold(Threshold);
  lcd.begin();

  if (pulseSensor.begin()) {
    lcd.setCursor(3, 0);
    lcd.print("Calibrating");
    lcd.setCursor(2, 1);
    lcd.print("System Ready");
  }
}

double Light(int RawADC1) {
  double Vout = RawADC1 * 0.0048828125;
  int lux = 500 / (10 * ((5 - Vout) / Vout));
  return lux;
}

void loop() {
  int myBPM = pulseSensor.getBeatsPerMinute();

  if (pulseSensor.sawStartOfBeat()) {
    int currentLux = int(Light(analogRead(1)));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(currentLux);
    lcd.setCursor(4, 0);
    lcd.print("Lux");
    lcd.setCursor(0, 1);
    lcd.print("BPM: ");
    lcd.setCursor(5, 1);
    lcd.print(myBPM);

    Serial.print("Lux: ");
    Serial.print(currentLux);
    Serial.print(" | BPM: ");
    Serial.println(myBPM);
    delay(1000);
  }

  delay(20);
  if (myBPM <= 55) {
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Low BPM alarm");
  } else {
    digitalWrite(buzzerPin, LOW);
  }
}
