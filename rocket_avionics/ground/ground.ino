#include <SPI.h>
#include <LoRa.h>
// #include <RTClib.h> // Uncomment if RTC is connected

// RTC_DS3231 rtc; // Uncomment if RTC is used
#define LORA_CS   5
#define LORA_RST  14
#define LORA_DIO0 26

String inputBuffer = "";

void setup() {
  Serial.begin(115200);
  // while (!Serial);  // hang until serial is ready

  Serial.println("Ground station booting...");

  // LoRa setup
  LoRa.setPins(5, 14, 26); // CS, RESET, DIO0
  Serial.println("Initializing LoRa...");

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Halting.");
    while (true) delay(100); // Safe WDT-friendly loop
  }

  Serial.println("LoRa init success.");

  // RTC setup
  /*
  if (!rtc.begin()) {
    Serial.println("RTC not found. Continuing without time.");
    // You may choose to halt instead: while(true) delay(100);
  } else {
    Serial.println("RTC OK");
  }
  */

  Serial.println("Ground station ready.");
}

void loop() {
  // Check if user typed something in the Serial Monitor
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {  // End of input
      if (inputBuffer.length() > 0) {
        Serial.print("Sending command: ");
        Serial.println(inputBuffer);

        // Send over LoRa
        LoRa.beginPacket();
        LoRa.print(inputBuffer);
        LoRa.endPacket();

        inputBuffer = "";  // Clear for next command
      }
    } else {
      inputBuffer += c; // Add to command buffer
    }
  }

  // (Optional) Also check for ACK or any incoming LoRa response
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String reply = "";
    while (LoRa.available()) {
      reply += (char)LoRa.read();
    }

    Serial.print("Received from rocket: ");
    Serial.println(reply);
  }

  delay(10);
}
