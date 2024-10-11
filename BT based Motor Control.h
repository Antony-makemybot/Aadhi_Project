// Pin Definitions
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define RELAY_PIN 7
#define IR_SENSOR_PIN 6  // IR sensor for relay control
#define LED_PIN 8  // Pin for LED
#define SERVO_PIN 10  // Pin for the servo motor

#include <SoftwareSerial.h>
#include <Servo.h>  // Include the Servo library

SoftwareSerial BTSerial(11, 12); // RX, TX for HC-05
Servo myServo;  // Create a Servo object

void setup() {
    // Set motor pins as outputs
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    
    // Set relay pin as output
    pinMode(RELAY_PIN, OUTPUT);
    
    // Set IR sensor pin for motion detection
    pinMode(IR_SENSOR_PIN, INPUT);
    
    // Set LED pin as output
    pinMode(LED_PIN, OUTPUT);
    
    // Attach the servo to its pin
    myServo.attach(SERVO_PIN);
    myServo.write(0);  // Set the initial angle to 0 degrees

    // Start motors in a neutral state
    stopMotors();
    
    // Start Bluetooth Serial
    BTSerial.begin(9600);
    Serial.begin(9600); // For debugging
}

void loop() {
    // Check for incoming Bluetooth commands
    if (BTSerial.available()) {
        char command = BTSerial.read();
        executeCommand(command);
    }

    // Read the IR sensor for relay control
    int irSignalValue = digitalRead(IR_SENSOR_PIN);
    if (irSignalValue == HIGH) {
        digitalWrite(RELAY_PIN, HIGH);
        delay(1000); // Relay ON for 1 second
        digitalWrite(RELAY_PIN, LOW);
        
        // Move the servo to 90 degrees
        myServo.write(90);
        delay(5000); // Wait for 5 seconds
        myServo.write(0); // Return to 0 degrees
    }

    // Manage the LED blink every 10 seconds
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

void executeCommand(char command) {
    switch (command) {
        case 'F': // Move forward
            moveForward();
            break;
        case 'B': // Move backward
            moveBackward();
            break;
        case 'L': // Turn left
            turnLeft();
            break;
        case 'R': // Turn right
            turnRight();
            break;
        case 'S': // Stop
            stopMotors();
            break;
        default:
            break;
    }
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
