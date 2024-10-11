// Pin Definitions
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define RELAY_PIN 7
#define IR_SENSOR_LEFT_PIN 8  // Left IR sensor for line following
#define IR_SENSOR_RIGHT_PIN 9 // Right IR sensor for line following
#define LED_PIN 10  // Pin for LED
#define SERVO_PIN 11 // Pin for the servo motor

// Bluetooth Serial
#include <SoftwareSerial.h>
#include <Servo.h> // Include the Servo library

SoftwareSerial BTSerial(12, 13); // RX, TX for HC-05
Servo myServo; // Create a servo object

void setup() {
    // Set motor pins as outputs
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    
    // Set IR sensor pins for line following
    pinMode(IR_SENSOR_LEFT_PIN, INPUT);
    pinMode(IR_SENSOR_RIGHT_PIN, INPUT);
    
    // Set relay pin as output
    pinMode(RELAY_PIN, OUTPUT);
    
    // Set LED pin as output
    pinMode(LED_PIN, OUTPUT);
    
    // Initialize the servo
    myServo.attach(SERVO_PIN); // Attach the servo to the specified pin
    myServo.write(0); // Start the servo at 0 degrees
    
    // Start motors in a neutral state
    stopMotors();
    
    // Start Bluetooth Serial
    BTSerial.begin(9600);
}

void loop() {
    // Read the line following IR sensor values
    int leftSensorValue = digitalRead(IR_SENSOR_LEFT_PIN);
    int rightSensorValue = digitalRead(IR_SENSOR_RIGHT_PIN);
    
    // Line following logic
    if (leftSensorValue == HIGH && rightSensorValue == HIGH) {
        // Both sensors see the line, move forward
        moveForward();
    } else if (leftSensorValue == LOW && rightSensorValue == HIGH) {
        // Left sensor sees line, turn right
        turnRight();
    } else if (leftSensorValue == HIGH && rightSensorValue == LOW) {
        // Right sensor sees line, turn left
        turnLeft();
    } else {
        // No line detected
        stopMotors();
        waitAtEnd();  // Wait if at the end of the line
    }

    // Manage LED blink every 10 seconds
    static unsigned long previousMillis = 0;
    static const long interval = 10000;  // Interval for LED blink
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Blink the LED for 5 seconds
        digitalWrite(LED_PIN, HIGH);
        delay(5000); // LED ON for 5 seconds
        digitalWrite(LED_PIN, LOW);
    }
}

void waitAtEnd() {
    // Move the servo to 90 degrees
    myServo.write(90);
    delay(2500);  // Wait for 2.5 seconds
    myServo.write(0); // Move the servo back to 0 degrees

    delay(60000);  // Wait for 60 seconds before returning
    returnToStart();  // Move back to the starting position
}

void returnToStart() {
    // Reverse logic to move back to the starting position
    moveBackward();
    delay(3000);  // Move backward for 3 seconds (adjust as needed)
    stopMotors();
}

void moveForward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void moveBackward() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void turnLeft() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void turnRight() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}
