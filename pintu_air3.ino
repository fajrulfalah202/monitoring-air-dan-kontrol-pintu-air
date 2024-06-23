#define BLYNK_TEMPLATE_ID "TMPL62yLVv5mZ"
#define BLYNK_TEMPLATE_NAME "pintu air"
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <BlynkSimpleEsp8266.h>  // Tambahkan library Blynk

// Pin definition
const int trigPin = D5;
const int echoPin = D6;
const int servoPin = D3;

// Servo motor
Servo myservo;

// Variables for duration and distance
long duration;
int distance;

// Water level threshold (in cm)
const int lowerThreshold = 2; // Lower threshold
const int upperThreshold = 3; // Upper threshold

// Blynk Auth Token
char auth[] = "yz3Xb3XvWjLZZRwbHOiSP0Blz0q5zGzQ";  // Ganti dengan Auth Token dari Blynk

// WiFi credentials
char ssid[] = "Galaxy A21...";         // Ganti dengan SSID WiFi
char pass[] = "odadingmangoleng";     // Ganti dengan password WiFi

// Manual control variable
bool manualControl = false;
int manualPosition = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup...");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myservo.attach(servoPin);
  myservo.write(0); // Initial position of the servo motor

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }

  Serial.println("Connecting to Blynk...");
  Blynk.begin(auth, ssid, pass);
}


// Fungsi untuk menangani input dari tombol Blynk
BLYNK_WRITE(V2) {
  int pinValue = param.asInt(); // Get the value from virtual pin V2

  if (pinValue == 1) {
    manualControl = true; // Aktifkan mode kontrol manual
    manualPosition = 0;  // Set posisi servo manual ke 90 derajat (bisa disesuaikan)
    Serial.println("Kontrol manual: Servo pada posisi 90 derajat");
  } else {
    manualControl = false; // Nonaktifkan mode kontrol manual
    Serial.println("Kontrol manual dinonaktifkan");
  }
}

void loop() {
  Blynk.run();  // Call Blynk.run() to keep the connection alive

  if (!manualControl) { // Jika tidak dalam mode kontrol manual
    // Clear the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // Set the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Read the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculate the distance
    distance = duration * 0.034 / 2;

    // Print the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Send distance to Blynk
    Blynk.virtualWrite(V1, distance);  // Kirim data ke virtual pin V1 di Blynk

    // Check if the distance is within the threshold
    if (distance >= lowerThreshold && distance <= upperThreshold) {
      myservo.write(0); // Close the gate (adjust the angle according to your needs)
      Serial.print("Pintu ditutup: ");
    } else {
      myservo.write(90); // Open the gate
      Serial.print("Pintu dibuka: ");
    }
  } else {
    // Jika dalam mode kontrol manual, atur posisi servo sesuai manualPosition
    myservo.write(manualPosition);
  }

  // Wait for a while before measuring again
  delay(1000);
}
