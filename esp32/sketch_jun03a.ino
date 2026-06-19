// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <ArduinoJson.h>
// #include <ModbusMaster.h>

// #define RXD2 16
// #define TXD2 17
// #define DERE 4

// const char* ssid = "xxxxxxxxx";
// const char* password = "xxxxxxxxx";
// const char* mqtt_server = "xxxxxxxxxx";

// WiFiClient espClient;
// PubSubClient client(espClient);
// ModbusMaster node;

// void preTransmission() {
//   digitalWrite(DERE, HIGH);
// }

// void postTransmission() {
//   digitalWrite(DERE, LOW);
// }

// bool readRegister(uint16_t reg, uint16_t &value) {
//   uint8_t result = node.readHoldingRegisters(reg, 1);

//   if (result == node.ku8MBSuccess) {
//     value = node.getResponseBuffer(0);
//     return true;
//   }
//   return false;
// }

// void connectWiFi() {

//   Serial.print("Connecting WiFi");

//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println();
//   Serial.println("WiFi Connected");
//   Serial.print("ESP32 IP: ");
//   Serial.println(WiFi.localIP());
// }

// void connectMQTT() {

//   while (!client.connected()) {

//     Serial.print("Connecting MQTT...");

//     if (client.connect("ESP32_Soil")) {

//       Serial.println("Connected");

//     } else {

//       Serial.print("Failed rc=");
//       Serial.println(client.state());

//       delay(3000);
//     }
//   }
// }

// void setup() {

//   Serial.begin(115200);

//   pinMode(DERE, OUTPUT);
//   digitalWrite(DERE, LOW);

//   Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

//   node.begin(1, Serial2);

//   node.preTransmission(preTransmission);
//   node.postTransmission(postTransmission);

//   connectWiFi();

//   client.setServer(mqtt_server, 1883);

//   Serial.println("JXCT Soil Sensor MQTT Ready");
// }

// void loop() {

//   if (!client.connected()) {
//     connectMQTT();
//   }

//   client.loop();

//   uint16_t moisture = 0;
//   uint16_t tempRaw = 0;
//   uint16_t ec = 0;
//   uint16_t phRaw = 0;
//   uint16_t nitrogen = 0;
//   uint16_t phosphorus = 0;
//   uint16_t potassium = 0;

//   float moistureVal = 0;
//   float tempVal = 0;
//   float phVal = 0;

//   if (readRegister(18, moisture))
//     moistureVal = moisture / 10.0;

//   if (readRegister(19, tempRaw))
//     tempVal = ((int16_t)tempRaw) / 10.0;

//   if (readRegister(21, ec));

//   if (readRegister(6, phRaw))
//     phVal = phRaw / 100.0;

//   if (readRegister(30, nitrogen));
//   if (readRegister(31, phosphorus));
//   if (readRegister(32, potassium));

//   JsonDocument doc;

//   doc["moisture"] = moistureVal;
//   doc["temperature"] = tempVal;
//   doc["ph"] = phVal;
//   doc["ec"] = ec;
//   doc["nitrogen"] = nitrogen;
//   doc["phosphorus"] = phosphorus;
//   doc["potassium"] = potassium;

//   char payload[256];

//   serializeJson(doc, payload);

//   client.publish("farm/soil/1", payload);

//   Serial.println(payload);

//   delay(5000);
// } 



#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ModbusMaster.h>

// =================== RS485 ===================
#define RXD2 16
#define TXD2 17
#define DERE 4

// =================== WiFi ===================
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// =================== MQTT ===================
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883;

// =================== Sensor Identity ===================
const char* SENSOR_ID = "s01";
const char* SENSOR_TYPE = "soil";

// =======================================================

WiFiClient espClient;
PubSubClient client(espClient);
ModbusMaster node;

String mqttTopic;

// =======================================================

void preTransmission() {
  digitalWrite(DERE, HIGH);
}

void postTransmission() {
  digitalWrite(DERE, LOW);
}

// =======================================================

bool readRegister(uint16_t reg, uint16_t &value) {

  uint8_t result = node.readHoldingRegisters(reg, 1);

  if (result == node.ku8MBSuccess) {
    value = node.getResponseBuffer(0);
    return true;
  }

  return false;
}

// =======================================================

void connectWiFi() {

  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());
}

// =======================================================

void connectMQTT() {

  while (!client.connected()) {

    Serial.print("Connecting MQTT...");

    if (client.connect(SENSOR_ID)) {

      Serial.println("Connected");

    } else {

      Serial.print("Failed. RC=");
      Serial.println(client.state());

      delay(3000);
    }
  }
}

// =======================================================

void setup() {

  Serial.begin(115200);

  pinMode(DERE, OUTPUT);
  digitalWrite(DERE, LOW);

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  node.begin(1, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  mqttTopic = "sensors/";
  mqttTopic += SENSOR_TYPE;
  mqttTopic += "/";
  mqttTopic += SENSOR_ID;

  connectWiFi();

  client.setServer(mqtt_server, mqtt_port);

  Serial.println("--------------------------------");
  Serial.println("Smart Farm Soil Sensor Ready");
  Serial.print("Publishing Topic : ");
  Serial.println(mqttTopic);
  Serial.println("--------------------------------");
}

// =======================================================

void loop() {

  if (!client.connected())
    connectMQTT();

  client.loop();

  uint16_t moisture = 0;
  uint16_t tempRaw = 0;
  uint16_t ec = 0;
  uint16_t phRaw = 0;
  uint16_t nitrogen = 0;
  uint16_t phosphorus = 0;
  uint16_t potassium = 0;

  float moistureValue = NAN;
  float temperatureValue = NAN;
  float phValue = NAN;

  if (readRegister(18, moisture))
    moistureValue = moisture / 10.0;

  if (readRegister(19, tempRaw))
    temperatureValue = ((int16_t)tempRaw) / 10.0;

  readRegister(21, ec);

  if (readRegister(6, phRaw))
    phValue = phRaw / 100.0;

  readRegister(30, nitrogen);
  readRegister(31, phosphorus);
  readRegister(32, potassium);

  JsonDocument doc;

  // Identity
  doc["sensorId"] = SENSOR_ID;
  doc["sensorType"] = SENSOR_TYPE;

  // Readings
  doc["temperature"] = temperatureValue;
  doc["moisture"] = moistureValue;
  doc["ph"] = phValue;
  doc["ec"] = ec;
  doc["nitrogen"] = nitrogen;
  doc["phosphorus"] = phosphorus;
  doc["potassium"] = potassium;

  char payload[512];
  serializeJson(doc, payload);

  client.publish(mqttTopic.c_str(), payload);

  Serial.println("--------------------------------");
  Serial.println("Published Successfully");
  Serial.println(mqttTopic);
  Serial.println(payload);
  Serial.println("--------------------------------");

  delay(5000);
}#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ModbusMaster.h>

//  RS485 
#define RXD2 16
#define TXD2 17
#define DERE 4

//  WiFi 
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

//  MQTT 
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883;

//  Sensor Identity 
const char* SENSOR_ID = "s01";
const char* SENSOR_TYPE = "soil";


WiFiClient espClient;
PubSubClient client(espClient);
ModbusMaster node;

String mqttTopic;


void preTransmission() {
  digitalWrite(DERE, HIGH);
}

void postTransmission() {
  digitalWrite(DERE, LOW);
}


bool readRegister(uint16_t reg, uint16_t &value) {

  uint8_t result = node.readHoldingRegisters(reg, 1);

  if (result == node.ku8MBSuccess) {
    value = node.getResponseBuffer(0);
    return true;
  }

  return false;
}



void connectWiFi() {

  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());
}


void connectMQTT() {

  while (!client.connected()) {

    Serial.print("Connecting MQTT...");

    if (client.connect(SENSOR_ID)) {

      Serial.println("Connected");

    } else {

      Serial.print("Failed. RC=");
      Serial.println(client.state());

      delay(3000);
    }
  }
}


void setup() {

  Serial.begin(115200);

  pinMode(DERE, OUTPUT);
  digitalWrite(DERE, LOW);

  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  node.begin(1, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  mqttTopic = "sensors/";
  mqttTopic += SENSOR_TYPE;
  mqttTopic += "/";
  mqttTopic += SENSOR_ID;

  connectWiFi();

  client.setServer(mqtt_server, mqtt_port);

  Serial.println("--------------------------------");
  Serial.println("Smart Farm Soil Sensor Ready");
  Serial.print("Publishing Topic : ");
  Serial.println(mqttTopic);
  Serial.println("--------------------------------");
}


void loop() {

  if (!client.connected())
    connectMQTT();

  client.loop();

  uint16_t moisture = 0;
  uint16_t tempRaw = 0;
  uint16_t ec = 0;
  uint16_t phRaw = 0;
  uint16_t nitrogen = 0;
  uint16_t phosphorus = 0;
  uint16_t potassium = 0;

  float moistureValue = NAN;
  float temperatureValue = NAN;
  float phValue = NAN;

  if (readRegister(18, moisture))
    moistureValue = moisture / 10.0;

  if (readRegister(19, tempRaw))
    temperatureValue = ((int16_t)tempRaw) / 10.0;

  readRegister(21, ec);

  if (readRegister(6, phRaw))
    phValue = phRaw / 100.0;

  readRegister(30, nitrogen);
  readRegister(31, phosphorus);
  readRegister(32, potassium);

  JsonDocument doc;

  // Identity
  doc["sensorId"] = SENSOR_ID;
  doc["sensorType"] = SENSOR_TYPE;

  // Readings
  doc["temperature"] = temperatureValue;
  doc["moisture"] = moistureValue;
  doc["ph"] = phValue;
  doc["ec"] = ec;
  doc["nitrogen"] = nitrogen;
  doc["phosphorus"] = phosphorus;
  doc["potassium"] = potassium;

  char payload[512];
  serializeJson(doc, payload);

  client.publish(mqttTopic.c_str(), payload);

  Serial.println("--------------------------------");
  Serial.println("Published Successfully");
  Serial.println(mqttTopic);
  Serial.println(payload);
  Serial.println("--------------------------------");

  delay(5000);
}