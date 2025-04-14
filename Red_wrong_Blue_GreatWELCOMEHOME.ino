#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#define Blue_LED 7     // Blue LED pin for authorized
#define Red_LED 6    // Red LED pin for unauthorized

SoftwareSerial mySerial(2, 3);  // RX, TX for fingerprint module
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial on some boards
  delay(100);

  pinMode(Blue_LED, OUTPUT);
  pinMode(Red_LED, OUTPUT);
  digitalWrite(Blue_LED, LOW);
  digitalWrite(Red_LED, LOW);

  finger.begin(57600);
  delay(5);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); 
  Serial.print(finger.templateCount); 
  Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
}

void loop() {
  uint8_t result = getFingerprintID();  // Store result from fingerprint scan

  // Control LEDs based on result: Forme it only works when result == 99 ou result == 1 based on the enrollement I did.
  if (result == 99 ) {
    digitalWrite(Blue_LED, HIGH);     // Authorized: Green ON
    digitalWrite(Red_LED, LOW);     // Yellow OFF
    Serial.println("Authorized fingerprint!");
    delay(1500);
  } else if (result == 0xFF) {
    // No finger or failed reading — do nothing
    digitalWrite(Blue_LED, LOW);
    digitalWrite(Red_LED, LOW);
  } else {
    digitalWrite(Blue_LED, LOW);      // Unauthorized: Green OFF
    digitalWrite(Red_LED, HIGH);    // Yellow ON
    Serial.println("Unauthorized fingerprint!");
    delay(1500);
  }

  // Reset LEDs after display time
  digitalWrite(Blue_LED, LOW);
  digitalWrite(Red_LED, LOW);
  delay(200);
}

// Function returns:
//  - Finger ID (1, 3, 4, 5, etc.) if found
//  - 0xFF if no finger or error
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return 0xFF;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return 0xFF;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return 0xFE;  // No match

  // Match found
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence "); Serial.println(finger.confidence);
  return finger.fingerID;
}
