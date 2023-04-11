#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

// WiFi hotspot credentials
const char* ssid = "Merici stanice";
const char* password = "password";

// Serial configuration
const uint8_t SERIAL_RX = D2; // RX pin for SoftwareSerial communication
const uint8_t SERIAL_TX = D1; // TX pin for SoftwareSerial communication
const uint32_t SERIAL_BAUD = 9600; // Baud rate for SoftwareSerial communication

SoftwareSerial serial(SERIAL_RX, SERIAL_TX);

// Web server configuration
WiFiServer server(80);

void setup() {
  Serial.begin(115200); // initialize Serial
  delay(10);

  // Create WiFi hotspot
  WiFi.softAP(ssid, password);
  Serial.println("");
  Serial.println("WiFi hotspot created.");

  // Print IP address to Serial Monitor
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start software serial port
  serial.begin(SERIAL_BAUD);
  Serial.println("SoftwareSerial started.");

  // Start web server
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    // Wait for client to send data
    while (!client.available()) {
      delay(1);
    }

    // Read data from client (discard it)
    client.read();

    // Read temperature data from SoftwareSerial port
    uint16_t temperature_data = 0;
    while (serial.available() < 2) {
      delay(1);
    }
    uint8_t msb = serial.read();
    uint8_t lsb = serial.read();
    temperature_data = (msb << 8) | lsb;

    // Convert temperature data to degrees Celsius
    float temperature = (float)temperature_data / 10.0;

    // Send HTTP response to client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<html><head><title>Temperature Monitor</title></head><body><h1>");
    client.print("Temperature: ");
    client.print(temperature);
    client.println(" degrees Celsius");
    client.println("</h1></body></html>");
    delay(1);

    // Close connection
    client.stop();
  }
}
