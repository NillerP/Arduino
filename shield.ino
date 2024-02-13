#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

#define LED_PIN 3

byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0x2E, 0x3A }; // Shield's Mac Adress
IPAddress serverIP(172,67,34,170);  // PC Address 
EthernetClient client;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  Ethernet.begin(mac); // Initialize Ethernet connection
  delay(1000); // Allow time for Ethernet to initialize

  Serial.println("Ethernet initialized");
}

void loop() {
  if (client.connect(serverIP, 80)) {
    Serial.println("Connected to server");

    client.print("GET /raw/UNavt19C HTTP/1.1\r\n"); 
    client.print("Host: pastebin.com\r\n"); 
    client.print("Connection: close\r\n\r\n");

    while (client.connected()) {
      if (client.available()) {
        Serial.println("Data received from server");

        // Read JSON data from the server
        DynamicJsonDocument doc(1024); // Adjust the buffer size as needed
        DeserializationError error = deserializeJson(doc, client);
        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
        }

        // Process JSON data and control LED
        JsonArray ledStates = doc["led_states"];
        for (bool ledState : ledStates) {
          digitalWrite(LED_PIN, ledState ? HIGH : LOW);
          delay(1000); // Adjust delay time as needed
        }

        client.stop(); // Close the connection
      }
    }
  } else {
    Serial.println("Connection failed");
  }

  delay(5000); // Wait 5 seconds before fetching JSON data again
}
