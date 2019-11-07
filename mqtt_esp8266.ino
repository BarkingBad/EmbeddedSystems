#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

const char* ssid = "DESKTOP-M6800-303";
const char* password = "12345678";
const char* mqtt_server = "192.168.137.11";

const char* topic = "outTopic";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("\noops something went wrong :/");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  sensors.begin();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    sensors.requestTemperatures();
    double tmp = sensors.getTempCByIndex(0);
    snprintf (msg, 75, "%lf", tmp);
    Serial.println(msg);
    client.publish(topic, msg);
  }
}
