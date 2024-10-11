#include <Servo.h> // Include the Servo library

// Pin Definitions
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define LEFT_IR_SENSOR_PIN 9
#define RIGHT_IR_SENSOR_PIN 10
#define RELAY_PIN 7
#define PIR_SENSOR_PIN 6  // PIR sensor for motion detection
#define LED_PIN 8  // Pin for LED
#define SERVO_PIN 11 // Pin for Servo Motor

// Timing
unsigned long previousMillis = 0;
const long interval = 10000;  // Interval for LED blink in milliseconds
unsigned long ledOnTime = 0;
bool ledState = LOW;
bool relayActive = false;

// Servo variables
Servo myServo;
bool firstSignalReceived = false; // Track the first signal
unsigned long lastPIRSignalTime = 0; // To time out after the second signal

// State management for line following
enum State {
    FOLLOWING_LINE,
    WAITING_AT_END,
    RETURNING
};

State currentState = FOLLOWING_LINE;
unsigned long waitStartTime = 0;

void setup() {
    // Set motor pins as outputs
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    
    // Set line following IR sensor pins as inputs
    pinMode(LEFT_IR_SENSOR_PIN, INPUT);
    pinMode(RIGHT_IR_SENSOR_PIN, INPUT);
    
    // Set relay pin as output
    pinMode(RELAY_PIN, OUTPUT);
    
    // Set PIR sensor pin as input
    pinMode(PIR_SENSOR_PIN, INPUT);
    
    // Set LED pin as output
    pinMode(LED_PIN, OUTPUT);
    
    // Initialize the servo
    myServo.attach(SERVO_PIN);
    myServo.write(0); // Start at 0 degrees
    
    // Start motors in a neutral state
    stopMotors();
}

void loop() {
    // Manage PIR sensor
    managePIRSensor();

    // Manage LED blinking
    manageLED();

    // Handle current state
    switch (currentState) {
        case FOLLOWING_LINE:
            lineFollowing();
            break;
        case WAITING_AT_END:
            waitAtEnd();
            break;
        case RETURNING:
            returnToStart();
            break;
    }
}

void lineFollowing() {
    int leftSensorValue = digitalRead(LEFT_IR_SENSOR_PIN);
    int rightSensorValue = digitalRead(RIGHT_IR_SENSOR_PIN);
    
    // Detecting the end of the line
    if (leftSensorValue == LOW && rightSensorValue == LOW) {
        // Both sensors lose the line, assume end of the line
        currentState = WAITING_AT_END;
        waitStartTime = millis(); // Start the wait timer
        stopMotors();
    } else {
        // Normal line following logic
        if (leftSensorValue == HIGH && rightSensorValue == HIGH) {
            moveForward();
        } else if (leftSensorValue == LOW && rightSensorValue == HIGH) {
            turnRight();
        } else if (leftSensorValue == HIGH && rightSensorValue == LOW) {
            turnLeft();
        } else {
            stopMotors();
        }
    }
}

void waitAtEnd() {
    // Wait for 60 seconds at the end of the line
    if (millis() - waitStartTime >= 60000) { // 60 seconds
        currentState = RETURNING; // Move to return state
    }
}

void returnToStart() {
    // Implement logic to return to the starting position
    // Here we will turn around and move forward
    turnAround();
    moveForward(); // Move in the opposite direction
    
    // Assume you want to return until you detect the line again
    if (digitalRead(LEFT_IR_SENSOR_PIN) == HIGH || digitalRead(RIGHT_IR_SENSOR_PIN) == HIGH) {
        currentState = FOLLOWING_LINE; // Back to line following
    }
}

void turnAround() {
    // Simple 180-degree turn logic
    turnLeft(); // Turn left for a moment
    delay(500); // Adjust delay for your turn speed
    stopMotors(); // Stop before moving forward
}

void managePIRSensor() {
    int pirSignalValue = digitalRead(PIR_SENSOR_PIN);
    unsigned long currentMillis = millis();
    
    if (pirSignalValue == HIGH) {
        if (!firstSignalReceived) {
            firstSignalReceived = true; // First signal received
            digitalWrite(RELAY_PIN, HIGH);
            relayActive = true;
            delay(1000); // Relay ON for 1 second
            digitalWrite(RELAY_PIN, LOW);
            
            myServo.write(90); // Turn servo to 90 degrees
            lastPIRSignalTime = currentMillis; // Record the time of the first signal
        } else {
            // Check if a second signal has been received within a reasonable time
            if (currentMillis - lastPIRSignalTime >= 2000) { // 2 seconds for a second signal
                myServo.write(0); // Tuck servo back to 0 degrees
                firstSignalReceived = false; // Reset for the next cycle
            }
        }
    } else {
        // If PIR signal is low and we're within the timeout, reset the first signal
        if (firstSignalReceived && (currentMillis - lastPIRSignalTime >= 2000)) {
            firstSignalReceived = false; // Reset if no second signal
        }
    }
}

void manageLED() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        ledOnTime = currentMillis + 5000; // Set time for LED ON
        ledState = HIGH; // Turn LED ON
    }
    
    if (ledState == HIGH) {
        digitalWrite(LED_PIN, HIGH);
        if (currentMillis >= ledOnTime) {
            ledState = LOW; // Turn LED OFF after 5 seconds
            digitalWrite(LED_PIN, LOW);
        }
    }
}

void moveForward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void turnRight() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void turnLeft() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}
