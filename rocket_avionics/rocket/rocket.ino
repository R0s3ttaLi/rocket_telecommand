#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <RTClib.h>
#include <SD.h>

#define SD_CS 5  // SD card CS pin
RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);

  // Init RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (true);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting default time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set to compile time
  }

  // Init SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    while (true);
  }
  Serial.println("SD card ready");

  // Init LoRa
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa init failed!");
    while (true);
  }
  Serial.println("LoRa receiver ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String command = LoRa.readString();
    Serial.println("Received: " + command);

    // Get current time
    DateTime now = rtc.now();
    String timestamp = String(now.year()) + "-" +
                       String(now.month()) + "-" +
                       String(now.day()) + " " +
                       String(now.hour()) + ":" +
                       String(now.minute()) + ":" +
                       String(now.second());

    // Format log entry
    String logEntry = "[" + timestamp + "] " + command;

    // Log to SD
    File logFile = SD.open("/command_log.txt", FILE_APPEND);
    if (logFile) {
      logFile.println(logEntry);
      logFile.close();
      Serial.println("Logged: " + logEntry);
    } else {
      Serial.println("Failed to open log file");
    }

    // Send ACK back
    LoRa.beginPacket();
    LoRa.print("ACK: " + command);
    LoRa.endPacket();
  }
}
