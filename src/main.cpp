#include "settings.h"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HX711.h>
#include <RunningMedian.h>

/* GLOBAL VARIABLES */
HX711 scale;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
RunningMedian zeroWeightBuffer(50);
RunningMedian catWeightBuffer(50);
char mqttOutputBuffer[100];

/* GLOBAL STATE */
bool catOnScale = false;
bool catHasBeenWeighed = false;
float catWeight = 0;
float oldZeroWeight = 0;
unsigned long settleTimer = 0;

void wifiInit() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  Serial.print("..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.print("Succesfully connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
}

void scaleInit() {
  Serial.print("Intializing HX711 using DOUT on GPIO");
  Serial.print(LOADCELL_DOUT_PIN);
  Serial.print(" and SCK on GPIO");
  Serial.print(LOADCELL_SCK_PIN);
  Serial.print(" with calibration factor ");
  Serial.print(CALIBRATION_FACTOR, 0);
  Serial.println(".");

  Serial.println();

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);

  Serial.println("Scale will be tared in 10 seconds...");
  delay(10000);
  scale.tare();

  long zero_factor = scale.read_average();
  Serial.print("Scale has been tared. Zero factor: ");
  Serial.println(zero_factor);
  Serial.println();
}

void mqttCallback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void mqttInit() {
  Serial.println("Initializing MQTT...");
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback(mqttCallback);
  Serial.println("MQTT initialized.");
}

void mqttReconnect() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");

    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("Successfully connected, subscribing...");
      mqttClient.subscribe(MQTT_INPUT_TOPIC);
      Serial.println("Successfully subscribed.");
      Serial.println();
    } else {
      Serial.print("Failed connecting, rc=");
      Serial.print(mqttClient.state());
      Serial.println(". Will try again in 5 seconds.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("IoT Cat litter box");
  Serial.println("by Thijs Dortmann (github.com/thijsdortmann)");
  Serial.println();

  wifiInit();
  mqttInit();
  scaleInit();
}

void loop() {
  if (!mqttClient.connected()) {
    mqttReconnect();
  }

  mqttClient.loop();

  float measurement = scale.get_units();
  float averageZeroWeight = zeroWeightBuffer.getAverage(15);

  if (!catOnScale && measurement - averageZeroWeight > CAT_MIN_WEIGHT && measurement - averageZeroWeight < CAT_MAX_WEIGHT) {
    Serial.println("Cat entered litter box.");

    catOnScale = true;
    catHasBeenWeighed = false;

    oldZeroWeight = zeroWeightBuffer.getAverage(15);

    catWeightBuffer.clear();
  }

  if (catOnScale && (measurement - averageZeroWeight < CAT_MIN_WEIGHT || measurement - averageZeroWeight > CAT_MAX_WEIGHT)) {
    Serial.println("Cat left litter box.");

    catOnScale = false;
    catHasBeenWeighed = true;

    zeroWeightBuffer.clear();

    settleTimer = millis() + 5000;
  }

  if (!catOnScale && catHasBeenWeighed && millis() >= settleTimer) {
    float catWeight = catWeightBuffer.getAverage(15) - oldZeroWeight;
    float excrementWeight = zeroWeightBuffer.getAverage(15) - oldZeroWeight;

    Serial.print("Cat weighs ");
    Serial.print(catWeight, 2);
    Serial.println(" kg.");

    Serial.print("Cat produced excrement weighing ");
    Serial.print(excrementWeight, 2);
    Serial.println(" kg.");

    Serial.println("Publishing weights to MQTT...");
    String mqttOutputString = "{ \"cat\": " + String(catWeight) + ", \"excrement\": " + String(excrementWeight) + " }";
    mqttOutputString.toCharArray(mqttOutputBuffer, mqttOutputString.length() + 1);
    mqttClient.publish(MQTT_OUTPUT_TOPIC, mqttOutputBuffer);
    Serial.println("Publishing done.");
    Serial.println();

    catHasBeenWeighed = false;
  }

  if (catOnScale) {
    catWeightBuffer.add(measurement);
  }

  if (!catOnScale) {
    zeroWeightBuffer.add(measurement);
  }

  delay(100);
}