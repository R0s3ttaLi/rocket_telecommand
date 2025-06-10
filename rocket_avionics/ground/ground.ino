#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa init failed!");
    while (true);
  }
  Serial.println("LoRa ready. Type a command:");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    String timestamp = String(millis());
    String packet = cmd + "|" + timestamp;

    LoRa.beginPacket();
    LoRa.print(packet);
    LoRa.endPacket();

    Serial.println("Sent: " + packet);

    // Wait for ACK
    long startTime = millis();
    while (millis() - startTime < 3000) {
      int packetSize = LoRa.parsePacket();
      if (packetSize) {
        String response = LoRa.readString();
        Serial.println("ACK: " + response);
        break;
      }
    }
  }
}
