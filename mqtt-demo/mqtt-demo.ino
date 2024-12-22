
#include <WiFi.h>
#include <PubSubClient.h>
const char* ssid = "<SSID>";
const char* password = "<Password>";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "makergram/tinyml/24/on";

WiFiClient espClient;
PubSubClient client(espClient);
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set MQTT server
  client.setServer(mqtt_server, mqtt_port);

  // Connect to MQTT broker
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
    } else {
      delay(1000);
    }
  }

  // Publish "Hello, World!" message
  client.publish(mqtt_topic, "Hello, World!");
}

void loop() {
  // Keep the MQTT connection alive
  client.loop();
}
