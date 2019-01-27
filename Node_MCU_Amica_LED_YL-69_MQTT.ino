#include <ArduinoJson.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <PubSubClient.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

const char* ssid="ASUS";
const char* mqtt_server = "192.168.1.150";

int sensor_pin = A0;       //set AO as pin for YL-69
int LEDPin = 13;           //sets D7 as pin for LED
int m=0;

#define moisture_topic "sensor/moisture"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi(){
  delay(10);
  //connect to Wi-Fi
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wi-Fi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect(){
  while(!client.connected()){
    Serial.print("Attempting MQTT connection...");
    if(client.connect("Sensors")){
      Serial.println("connected");
      } else {
      Serial.print("failed, rc= ");
      Serial.print(client.state());
      Serial.println("Try again in 5 seconds");
      delay(5000);
      }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
    reconnect();
    }
  client.loop();

  //publish moisture
      m = analogRead(sensor_pin);
      m = constrain(m, 485, 1023);
      m = map(m, 485, 1023, 100, 0);

      Serial.print("Moisture: ");
      Serial.println(m);
      client.publish(moisture_topic, String(m).c_str(), true);
      if(m < 30){
        digitalWrite(LEDPin, HIGH);
      }
    //every minute
    delay(1000*60);
  }
