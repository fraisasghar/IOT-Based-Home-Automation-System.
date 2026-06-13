// ========================================
// Pin Definitions (COMPLETE SYSTEM)
// ========================================
#define TRIG_PIN 12       // Ultrasonic sensor trigger
#define ECHO_PIN 11       // Ultrasonic sensor echo
#define GAS_SENSOR A1     // Potentiometer (simulating gas sensor)
#define LED_PIN 13        // LED pin
#define BUZZER_PIN 2      // Buzzer pin
#define MOTOR_PIN 8       // DC Motor (Water Pump)
#define DOOR_SERVO_PIN 6  // Servo motor (door lock)
#define BUTTON_PIN 7      // Push button for door control

#include <Servo.h>

Servo doorServo;

// Threshold values
const int GAS_THRESHOLD = 400;
const int DISTANCE_THRESHOLD = 10;  // Pump turns OFF when distance < 10

// Door lock variables
bool doorLocked = true;
bool buttonPressed = false;

// Distance simulation variables
int simulatedDistance = 80;  // Start at 80 cm
bool decreasing = true;       // Start by decreasing

void setup() {
  Serial.begin(9600);
  
  // Pin modes
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GAS_SENSOR, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Door servo
  doorServo.attach(DOOR_SERVO_PIN);
  doorServo.write(0);  // Start LOCKED
  
  // Initial states
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(MOTOR_PIN, HIGH);  // Pump starts ON (distance is 80)
  
  Serial.println("===========================================");
  Serial.println("        SMART HOME AUTOMATION SYSTEM       ");
  Serial.println("===========================================");
  Serial.println("Gas Threshold: 400 | Distance Threshold: 10 cm");
  Serial.println("Pump ON when distance > 10 cm");
  Serial.println("Pump OFF when distance <= 10 cm");
  Serial.println("===========================================");
  Serial.println("");
}

void loop() {
  // ========================================
  // READ SENSORS
  // ========================================
  int gasValue = analogRead(GAS_SENSOR);
  
  // Simulate distance: Start at 80, decrease to 10, then increase back to 80
  long distance = simulatedDistance;
  
  if (decreasing) {
    simulatedDistance -= 2;  // Decrease by 2 cm
    if (simulatedDistance <= 10) {
      decreasing = false;  // Start increasing
    }
  } else {
    simulatedDistance += 2;  // Increase by 2 cm
    if (simulatedDistance >= 80) {
      decreasing = true;   // Start decreasing again
    }
  }
  
  // ========================================
  // GAS SENSOR LOGIC
  // ========================================
  if (gasValue > GAS_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
  
  // ========================================
  // WATER PUMP CONTROL - FIXED LOGIC
  // ========================================
  // Pump ON when distance > 10 (water level LOW, need to fill)
  // Pump OFF when distance <= 10 (water level HIGH, tank full)
  
  if (distance > DISTANCE_THRESHOLD) {
    digitalWrite(MOTOR_PIN, HIGH);   // Pump ON (filling tank)
  } else {
    digitalWrite(MOTOR_PIN, LOW);    // Pump OFF (tank full)
  }
  
  // ========================================
  // DOOR LOCK CONTROL - SIMPLIFIED (NO DEBOUNCE ISSUES)
  // ========================================
  int buttonState = digitalRead(BUTTON_PIN);
  
  // Button pressed (goes LOW because of pull-up)
  if (buttonState == LOW && !buttonPressed) {
    buttonPressed = true;
    
    // Toggle door state
    doorLocked = !doorLocked;
    
    if (doorLocked) {
      doorServo.write(0);    // LOCK
      Serial.println("");
      Serial.println("🔒 ================================");
      Serial.println("   DOOR LOCKED");
      Serial.println("   ================================");
      Serial.println("");
    } else {
      doorServo.write(180);  // UNLOCK
      Serial.println("");
      Serial.println("🔓 ================================");
      Serial.println("   DOOR UNLOCKED");
      Serial.println("   ================================");
      Serial.println("");
    }
    
    delay(300);  // Simple debounce delay
  }
  
  // Button released (goes HIGH)
  if (buttonState == HIGH) {
    buttonPressed = false;
  }
  
  // ========================================
  // COMPLETE SERIAL OUTPUT
  // ========================================
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Gas: ");
  Serial.print(gasValue);
  Serial.print(" | LED: ");
  Serial.print(digitalRead(LED_PIN) ? "ON " : "OFF");
  Serial.print(" | Buzzer: ");
  Serial.print(digitalRead(BUZZER_PIN) ? "ON " : "OFF");
  Serial.print(" | Pump: ");
  Serial.print(digitalRead(MOTOR_PIN) ? "ON " : "OFF");
  Serial.print(" | Door: ");
  Serial.println(doorLocked ? "LOCKED" : "UNLOCKED");
  
  delay(500);  // Update every 500ms
}

// ========================================
// Get Distance Function
// ========================================
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  
  return distance;
}