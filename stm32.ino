#include <Dhcp.h>
#include <Dns.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>
#include <lwipopts.h>
#include <lwipopts_default.h>
#include <STM32Ethernet.h>
#include <PubSubClient.h>


const char topic[] = "outTopic";
const char server[] = "192.168.0.104";

const float epsilon = 0.01;
float lastMsg = 0;

EthernetClient ethClient;
PubSubClient client(ethClient);

void blink(int led) {
  
    digitalWrite(led, HIGH);
    delay(300);
    digitalWrite(led, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  float newMsg = atof((char*) payload);
  Serial.println(newMsg);
  if(newMsg != 0.0) {
    float diff = lastMsg - newMsg;
    
    if (fabs(diff) < epsilon) {
      blink(LED_GREEN);
    } else if(diff < 0) {
      blink(LED_RED);
    } else {
      blink(LED_BLUE);
    }
  
    lastMsg = newMsg;
  }
}

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
  client.subscribe(topic);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_RED, LOW);
  
  client.setServer(server, 1883);
  
  Serial.println("Connecting");
  while(Ethernet.begin() == 0) {
    Serial.print(".");
  }
  
  client.setCallback(callback);
  Serial.println("Connected");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
