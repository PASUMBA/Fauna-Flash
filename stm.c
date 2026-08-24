/*
  FaunaFlash - Router Bridge Firmware
  Controls onboard LED & Pin D0 (3.3V) for external XIAO receiver
*/

#include <Arduino_RouterBridge.h>

const int ledPin = LED_BUILTIN;
const int outputPin = D0; // 3.3V digital signal pin to XIAO receiver

void set_led(int state) {
  if (state > 0) {
    // Elephant Detected -> Turn LED ON & drive D0 HIGH (3.3V)
    digitalWrite(ledPin, LOW);     // ON (Active-LOW LED)
    digitalWrite(outputPin, HIGH); // 3.3V HIGH output to XIAO
    delay(15000);
  } else {
    // No Elephant -> Turn LED OFF & drive D0 LOW (0V)
    digitalWrite(ledPin, HIGH);    // OFF
    digitalWrite(outputPin, LOW);  // 0V LOW output to XIAO
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(outputPin, OUTPUT);

  // Initial state: OFF
  digitalWrite(ledPin, HIGH);
  digitalWrite(outputPin, LOW);

  // Initialize Router Bridge
  Bridge.begin();

  // Register function matching your existing Python call
  Bridge.provide("set_led", set_led);
}

void loop() {
  // Maintain communication loop
  Bridge.update();
}