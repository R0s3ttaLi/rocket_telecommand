#include <SPI.h>
#include <LoRa.h>

// LoRa pin mapping
#define LORA_CS     5
#define LORA_RST    14
#define LORA_DIO0   26

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Rocket telemetry system starting...");

  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    while (true) delay(100);
  }

  Serial.println("LoRa init OK. Waiting for ground commands...");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    // Send ACK back
    logMessage("Received command: " + received);

    String ack = "ACK:" + received;
    LoRa.beginPacket();
    LoRa.print(ack);
    LoRa.endPacket();

    logMessage("Sent: " + ack);
    Serial.println(ack);
  }

  delay(10); // Prevent watchdog resets
}

void logMessage(const String &msg) {
  unsigned long ms = millis();
  Serial.print("[");
  Serial.print(ms / 1000);  // seconds since boot
  Serial.print("s] ");
  Serial.println(msg);
}

