#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include "PMS.h"


// Replace the next variables with your SSID/Password combination
const char* ssid = "GOMEZ";//"vanguardIOT";
const char* password = "familiagomez310";//"12345678";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "driver-01.cloudmqtt.com";

#define RXD2 16 //RXX2 pin
#define TXD2 17 //TX2 pin
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
PMS pms(Serial2);
PMS::DATA data;
Adafruit_BME280 bme; // I2C

const int ledPin = 4;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  setup_wifi();
  client.setServer(mqtt_server, 18967);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
  
  while(!Serial);    
  Serial.println(F("BME280 test"));

  unsigned status;
  status = bme.begin(0x76);  

  if (!status) {
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      while (1) delay(10);
  }
  Serial.println();
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT
  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if (messageTemp == "on") {
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if (messageTemp == "off") {
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("VS1", "wpclsouf", "T8EzUR0QlEwO")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

char currMessage[100]={NULL};
float temperatura=0;
float humedad=0;
int pm1=0;
int pm25=0;
int pm10=0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 6000a ok bien 0) {
    lastMsg = now;
   
    pmsvalues();
    printValues();
    delay(1000);
    String message = "{\"temp\":\"";
    message+=temperatura;
    message+="\",\"humedad\":\"";
    message+=humedad;
    message+="\",\"pm1\":\"";
    message+=pm1;
    message+="\",\"pm25\":\"";
    message+=pm25;
    message+="\",\"pm10\":\"";
    message+=pm10;
    message+="\"}";
    Serial.println(message);
    message.toCharArray(currMessage, 100);
    client.publish("iotvanguard/datos", currMessage);
  }
}

void printValues() {
    temperatura = bme.readTemperature();
    humedad = bme.readHumidity();
    Serial.println("--------------------------------------------------------------------------");
}
 void pmsvalues() {
   pms.requestRead(); // enviar solicitud de lectura
    if (pms.readUntil(data))
    {
      pm1=data.PM_AE_UG_1_0;
      pm25=data.PM_AE_UG_2_5;
      pm10=data.PM_AE_UG_10_0;
    }
    else
    {
      Serial.println("No data.");
    }
  }
/**/
