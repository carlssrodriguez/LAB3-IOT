#include <WiFi.h>
#include <HttpClient.h>

// Replace with your network credentials
char ssid[] = "BLVD63";
char pass[] = "sdBLVD63";

// HTTP server configuration
const char kHostname[] = "example.com";
const char kPath[] = "/";
const int kPort = 80;

// Timeout settings
const int kNetworkTimeout = 30000; // 30 seconds
const int kNetworkDelay = 1000;    // 1 second

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  int err = 0;

  WiFiClient client;
  HttpClient http(client);

  Serial.println("\nSending HTTP request...");

  err = http.get(kHostname, kPort, kPath);

  if (err == 0) {
    Serial.println("Request started successfully.");

    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Response status code: ");
      Serial.println(err);

      err = http.skipResponseHeaders();
      if (err >= 0) {
        int bodyLen = http.contentLength();
        Serial.print("Content length: ");
        Serial.println(bodyLen);
        Serial.println("Response body:");

        unsigned long timeoutStart = millis();
        char c;

        while ((http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout)) {
          if (http.available()) {
            c = http.read();
            Serial.print(c);
            bodyLen--;
            timeoutStart = millis();
          } else {
            delay(kNetworkDelay);
          }
        }
      } else {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    } else {
      Serial.print("Invalid response code: ");
      Serial.println(err);
    }
  } else {
    Serial.print("Connection failed: ");
    Serial.println(err);
  }

  http.stop();
  Serial.println("Connection closed.");

  while (1); // Stop execution after first request
}
