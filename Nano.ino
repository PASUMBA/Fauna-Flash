// Receiver Code for Arduino Nano with RYLR998 LoRa Module

#include <SoftwareSerial.h>

// Define pins for SoftwareSerial communication with the RYLR998 LoRa module
// LoRa RX pin (connects to LoRa module's TX pin)
#define LORA_RX_PIN 4
// LoRa TX pin (connects to LoRa module's RX pin)
#define LORA_TX_PIN 5

// Define the digital output pin on the Arduino Nano (D13, often with built-in LED)
// This pin will be set HIGH or LOW based on the received signal
#define OUTPUT_PIN 13 // Changed from 3 to 13 as per user request

// Create a SoftwareSerial object for communication with the LoRa module
// The baud rate for RYLR998 is typically 115200 by default.
SoftwareSerial loraSerial(LORA_RX_PIN, LORA_TX_PIN);

// LoRa Module Configuration Parameters
// These must match on both the transmitter and receiver for communication
const int LORA_ADDRESS = 0;    // Module address (0-65535) - must match transmitter's destination address
const int LORA_NETWORK_ID = 10; // Network ID (0-16) - must match transmitter

// --- New variables for timed output ---
// Duration for which the output pin should remain HIGH when 'H' is received
const unsigned long HIGH_DURATION_MS = 5000; // 5 seconds (editable)

// Variable to store the time when the output pin was set HIGH
unsigned long outputPinHighStartTime = 0;

// Flag to indicate if the output pin is currently in a timed HIGH state
bool isOutputPinTimedHigh = false;

void setup() {
  // Initialize the hardware serial for debugging output (optional, but recommended)
  Serial.begin(115200);
  Serial.println("Receiver (Arduino Nano) Initializing...");

  // Initialize the SoftwareSerial communication with the LoRa module
  loraSerial.begin(115200); // Default baud rate for RYLR998

  // Set the output pin mode
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, LOW); // Ensure the output pin starts LOW

  // --- LoRa Module Configuration ---
  delay(1000); // Give the LoRa module time to power up and stabilize

  // Set LoRa Module Address
  Serial.print("Setting LoRa Address to ");
  Serial.print(LORA_ADDRESS);
  Serial.print("... ");
  loraSerial.print("AT+ADDRESS=");
  loraSerial.print(LORA_ADDRESS);
  loraSerial.print("\r\n");
  delay(500);
  while (loraSerial.available()) {
    Serial.write(loraSerial.read());
  }
  Serial.println("Done.");

  // Set LoRa Module Network ID
  Serial.print("Setting LoRa Network ID to ");
  Serial.print(LORA_NETWORK_ID);
  Serial.print("... ");
  loraSerial.print("AT+NETWORKID=");
  loraSerial.print(LORA_NETWORK_ID);
  loraSerial.print("\r\n");
  delay(500);
  while (loraSerial.available()) {
    Serial.write(loraSerial.read());
  }
  Serial.println("Done.");

  // Enable receive mode (RYLR998 usually enters receive mode automatically after setup)
  // You can explicitly send AT+RCV if needed, but often not necessary for continuous listening.
  // loraSerial.print("AT+RCV\r\n");
  // delay(100);

  Serial.println("Receiver setup complete. Waiting for signals...");
}

void loop() {
  // Check if data is available from the LoRa module
  if (loraSerial.available()) {
    // Read the incoming data
    String receivedData = "";
    while (loraSerial.available()) {
      char c = loraSerial.read();
      receivedData += c;
      // Small delay to allow entire message to arrive, or use a timeout
      // A more robust approach would be to wait for a newline or a specific end character.
      delay(2);
    }

    // The RYLR998 sends data in the format: "+RCV=<address>,<data_length>,<data>,<RSSI>,<SNR>"
    // We need to parse this string to extract the actual data.
    Serial.print("Received raw: ");
    Serial.println(receivedData);

    // Find the first comma after "+RCV="
    int firstComma = receivedData.indexOf(',');
    // Find the second comma
    int secondComma = receivedData.indexOf(',', firstComma + 1);
    // Find the third comma (which precedes RSSI)
    int thirdComma = receivedData.indexOf(',', secondComma + 1);

    // Ensure all necessary commas are found
    if (firstComma != -1 && secondComma != -1 && thirdComma != -1) {
      // Extract the actual data part, which is between the second and third comma
      String actualData = receivedData.substring(secondComma + 1, thirdComma);
      actualData.trim(); // Remove any leading/trailing whitespace

      Serial.print("Parsed data: ");
      Serial.println(actualData);

      // Check the parsed data and set the output pin accordingly
      if (actualData.equals("H")) {
        digitalWrite(OUTPUT_PIN, HIGH);
        Serial.println("Output pin set HIGH for timed duration.");
        outputPinHighStartTime = millis(); // Record the time when pin went HIGH
        isOutputPinTimedHigh = true;      // Activate the timed HIGH flag
      } else if (actualData.equals("L")) {
        digitalWrite(OUTPUT_PIN, LOW);
        Serial.println("Output pin set LOW (received 'L').");
        isOutputPinTimedHigh = false;     // Deactivate the timed HIGH flag
      } else {
        Serial.print("Unknown signal received: ");
        Serial.println(actualData);
      }
    } else {
      Serial.println("Could not parse received data format. Missing expected commas.");
    }
  }

  // --- Check for timed HIGH duration ---
  // If the output pin is in a timed HIGH state and the duration has passed
  if (isOutputPinTimedHigh && (millis() - outputPinHighStartTime >= HIGH_DURATION_MS)) {
    digitalWrite(OUTPUT_PIN, LOW);     // Turn the output pin LOW
    isOutputPinTimedHigh = false;      // Reset the flag
    Serial.println("Output pin timed HIGH duration ended. Set LOW.");
  }

  // Small delay to prevent busy-waiting and allow other tasks
  delay(10);
}
