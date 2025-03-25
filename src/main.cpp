#include <WiFi.h>
#include <HttpClient.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

// Sensor
Adafruit_AHTX0 aht;

// WiFi credentials
const char ssid[] = "mondi";
const char pass[] = "123456789";

// AWS Flask server
const char kHostname[] = "54.177.211.80";
const int kPort = 5000;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Connecting to WiFi...");
  
  // Set DNS manually (in case default fails)
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 4, 4);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, primaryDNS, secondaryDNS);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  // I2C for ESP32: SDA 21, SCL 22
  Wire.begin(21, 22);
  if (!aht.begin()) {
    Serial.println("AHT20 sensor not detected. Check wiring.");
    while (1);
  }
  Serial.println("AHT20 sensor detected.");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  float t = temp.temperature;
  float h = humidity.relative_humidity;

  Serial.printf(" Temp: %.2f °C | Humidity: %.2f %%\n", t, h);

  // Format HTTP path
  String path = "/?var=Temp:" + String(t) + "-Hum:" + String(h);

  WiFiClient client;
  client.setTimeout(15000); // Avoid connection drops
  HttpClient http(client);

  Serial.println("Sending data to server...");
  int err = http.get(kHostname, kPort, path.c_str());

  if (err == 0) {
    Serial.println("Request sent.");
    int statusCode = http.responseStatusCode();
    Serial.printf("Status code: %d\n", statusCode);

    http.skipResponseHeaders();
    while (http.available()) {
      char c = http.read();
      Serial.print(c);
    }
  } else {
    Serial.printf("Connection error: %d\n", err);
  }

  http.stop();
  Serial.println("Waiting 10 seconds...\n");
  delay(10000);
}
