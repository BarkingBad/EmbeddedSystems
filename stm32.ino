#include <Dhcp.h>
#include <Dns.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <lwipopts.h>
#include <lwipopts_default.h>
#include <STM32Ethernet.h>

#include <PubSubClient.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

long lastMsg = 0;
char msg[50];
const char topic[] = "outTopic";
const char server[] = "192.168.0.100";

#define ONE_WIRE_BUS 6 // D6 ON Board

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Nucleo-144")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 second");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  client.setServer(server, 1883);

  Serial.println("Connecting");
  while(Ethernet.begin() == 0) {
    Serial.print(".");
  }
  Serial.println("Connected");
  sensor.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long now = millis();
  
  if (now - lastMsg > 2000) {
    lastMsg = now;
    sensor.requestTemperatures();
    double tmp = sensor.getTempCByIndex(0);
    dtostrf(tmp,5,2,msg);
    Serial.println(msg);
    client.publish(topic, msg);
  }
}
