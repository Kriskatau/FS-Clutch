#include <Arduino.h>

// Pin definitions
#define FORWARD_PIN 3
#define BACKWARD_PIN 2

// Global variables
volatile int currentPosition = 0;
const unsigned long fullTravelTime = 5000; // 5 seconds for 0-100 movement
const int positionUpdateInterval = 50; // ms per position unit

// Function declarations
bool isValidNumber(String input);
void handleSerialInput();
void moveToPosition(int target);
void printDebugInfo(String message);

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial connection
  
  // Initialize relay control pins
  pinMode(FORWARD_PIN, OUTPUT);
  pinMode(BACKWARD_PIN, OUTPUT);
  
  // Ensure relays are off initially
  digitalWrite(FORWARD_PIN, LOW);
  digitalWrite(BACKWARD_PIN, LOW);

  printDebugInfo("System Ready");
  Serial.println("Enter target position (0-100):");
}

void loop() {
  if (Serial.available() > 0) {
    handleSerialInput();
  }
}

// Function implementations
bool isValidNumber(String input) {
  if (input.length() == 0) return false;
  for (byte i = 0; i < input.length(); i++) {
    if (!isdigit(input.charAt(i))) return false;
  }
  return true;
}

void handleSerialInput() {
  String input = Serial.readStringUntil('\n');
  input.trim();

  if (!isValidNumber(input)) {
    printDebugInfo("Error: Invalid input - numbers only");
    return;
  }

  int target = input.toInt();
  if (target < 0 || target > 100) {
    printDebugInfo("Error: Target out of range (0-100)");
    return;
  }

  moveToPosition(target);
}

void moveToPosition(int target) {
  if (target == currentPosition) {
    printDebugInfo("Already at target position");
    return;
  }

  // Calculate movement parameters
  const int direction = (target > currentPosition) ? FORWARD_PIN : BACKWARD_PIN;
  const int unitsToMove = abs(target - currentPosition);
  const unsigned long moveDuration = unitsToMove * (fullTravelTime / 100);

  // Debug output
  Serial.print("Current: ");
  Serial.print(currentPosition);
  Serial.print(" | Target: ");
  Serial.print(target);
  Serial.print(" | Moving ");
  Serial.print((direction == FORWARD_PIN) ? "FORWARD" : "BACKWARD");
  Serial.print(" for ");
  Serial.print(moveDuration);
  Serial.println("ms");

  // Execute movement
  digitalWrite(direction, HIGH);
  delay(moveDuration);
  digitalWrite(direction, LOW);

  // Update position tracking
  currentPosition = target;
  
  // Final debug output
  Serial.print("Reached position: ");
  Serial.println(currentPosition);
  Serial.println("Ready for new input");
}

void printDebugInfo(String message) {
  Serial.println();
  Serial.println("=== DEBUG ===");
  Serial.println(message);
  Serial.print("Current Position: ");
  Serial.println(currentPosition);
  Serial.println("==============");
  Serial.println();
}