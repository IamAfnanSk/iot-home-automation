/* ==========
Include all required libraries below
===========*/
// Wifi lib to use wifi
#include <ESP8266WiFi.h>
// ____Start of wifimanager libs____
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
// ___End of wifimanager libs____
// ____Start of webServer libs____
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
// ____End of webServer libs____
// PubSubClient for mqtt
#include <PubSubClient.h>

/* ==========
Variable declaration
===========*/
// For ota updates
bool otaFlag = false;
uint16_t otaTimeElapsed = 0;
// For connection status update
unsigned long previousMillis = 0;
long interval = 10000;

#define Relay1 D1
#define Relay2 D2
#define Relay3 D3
#define Relay4 D4
#define Relay5 D5
#define Relay6 D6
#define Relay7 D7
#define Relay8 D8

#define mqttServer ""
#define mqttPort 1883
#define mqttUserName ""
#define mqttPassword ""

// Callback function for mqtt
void callback(char *topic, byte *payload, unsigned int length)
{
  // Convert payload to string
  String messageRec;
  for (int i = 0; i < length; i++)
  {
    messageRec += String((char)payload[i]);
  }

  // Operation with payload accordingly
  if (String(topic) == "hall/lights/light1")
  {
    Serial.println("Got light one with message: " + messageRec);
    if (messageRec == "0")
    {
      digitalWrite(Relay3, LOW);
    }
    if (messageRec == "1")
    {
      digitalWrite(Relay3, HIGH);
    }
  }

  if (String(topic) == "hall/lights/light2")
  {
    Serial.println("Got light two with message: " + messageRec);
    if (messageRec == "0")
    {
      digitalWrite(Relay2, LOW);
    }
    if (messageRec == "1")
    {
      digitalWrite(Relay2, HIGH);
    }
  }

  if (String(topic) == "hall/lights/light3")
  {
    Serial.println("Got light three with message: " + messageRec);
    if (messageRec == "0")
    {
      digitalWrite(Relay4, LOW);
    }
    if (messageRec == "1")
    {
      digitalWrite(Relay4, HIGH);
    }
  }

  if (String(topic) == "hall/lights/light4")
  {
    Serial.println("Got light four with message: " + messageRec);
    if (messageRec == "0")
    {
      digitalWrite(Relay5, LOW);
    }
    if (messageRec == "1")
    {
      digitalWrite(Relay5, HIGH);
    }
  }

  if (String(topic) == "hall/fans/fan1")
  {
    Serial.println("Got fan one with message: " + messageRec);
    if (messageRec == "0")
    {
      digitalWrite(Relay8, LOW);
    }
    if (messageRec == "1")
    {
      digitalWrite(Relay8, HIGH);
    }
  }

  if (String(topic) == "hall/fans/fan2")
  {
    Serial.println("Got fan two with message: " + messageRec);
    if (messageRec == "0")
    {
      digitalWrite(Relay6, LOW);
    }
    if (messageRec == "1")
    {
      digitalWrite(Relay6, HIGH);
    }
  }

  if (String(topic) == "hall/bell")
  {
    Serial.println("Got door bell with message: " + messageRec);
    if (messageRec == "0")
    {
      digitalWrite(Relay7, LOW);
    }
    if (messageRec == "1")
    {
      digitalWrite(Relay7, HIGH);
    }
  }

  if (String(topic) == "hall/chandelier")
  {
    Serial.println("Got chandelier with message: " + messageRec);
    if (messageRec == "0")
    {
      digitalWrite(Relay1, LOW);
    }
    if (messageRec == "1")
    {
      digitalWrite(Relay1, HIGH);
    }
  }
}

// Wifi & PubSub client declaration
WiFiClient wifiClient;
ESP8266WebServer server;
PubSubClient client(mqttServer, mqttPort, callback, wifiClient);

/* ==========
All function declaration
===========*/
// Mqtt sub topics
void mqttSubs()
{
  client.subscribe("hall/lights/light1", 1);
  client.subscribe("hall/lights/light2", 1);
  client.subscribe("hall/lights/light3", 1);
  client.subscribe("hall/lights/light4", 1);
  client.subscribe("hall/fans/fan1", 1);
  client.subscribe("hall/fans/fan2", 1);
  client.subscribe("hall/bell", 1);
  client.subscribe("hall/chandelier", 1);
}

// Mqtt connect
void connectMqtt()
{
  if (client.connected())
  {
    return;
  }
  else
  {
    // uint8_t retries = 2;
    while (client.connect("espClientNew", mqttUserName, mqttPassword, "hall/connection", 1, true, "off", true) != 1)
    {
      Serial.println("Retrying MQTT connection in few seconds");
      client.disconnect();
      delay(3000);
    }

    client.publish("hall/connection", "on", true);
    client.publish("hall/ip", WiFi.localIP().toString().c_str(), true);
    mqttSubs();
  }
}

// Wifi setup
void wifiSetup()
{
  if (WiFi.waitForConnectResult() == WL_CONNECTED)
  {
    return;
  }
  else
  {
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(120);
    wifiManager.autoConnect("Home Automation");

    delay(500);
    Serial.print("Wifi Connected to IP:- ");
    Serial.print(WiFi.localIP());
    Serial.println("");
  }
}

// Ota setup
void otaSetup()
{
  ArduinoOTA.setPassword("afnan");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
    {
      type = "sketch";
    }
    else
    {
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
    {
      Serial.println("Auth Failed");
    }
    else if (error == OTA_BEGIN_ERROR)
    {
      Serial.println("Begin Failed");
    }
    else if (error == OTA_CONNECT_ERROR)
    {
      Serial.println("Connect Failed");
    }
    else if (error == OTA_RECEIVE_ERROR)
    {
      Serial.println("Receive Failed");
    }
    else if (error == OTA_END_ERROR)
    {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
}

// Webserver setup
void serverSetup()
{
  server.on("/restart", []() {
    server.send(200, "text/plain", "Restarting...");
    delay(1000);
    ESP.restart();
  });
  server.on("/newupload", []() {
    server.send(200, "text/plain", "Ready to upload");
    Serial.println("Ota ready");
    otaFlag = true;
    otaTimeElapsed = 0;
  });
  server.begin();
  Serial.println("Server started");
}

/* ==========
Setup function
===========*/
void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");

  // Calling all functions
  wifiSetup();
  serverSetup();
  otaSetup();
  delay(500);
  connectMqtt();

  // Pinmode to output
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
  pinMode(Relay5, OUTPUT);
  pinMode(Relay6, OUTPUT);
  pinMode(Relay7, OUTPUT);
  pinMode(Relay8, OUTPUT);
}

/* ==========
Main loop
===========*/
void loop()
{
  // Webserver
  server.handleClient();

  wifiSetup();
  delay(500);
  connectMqtt();

  // Update connection
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {
    previousMillis = currentMillis;
    client.publish("hall/connection", "on", true);
    client.publish("hall/ip", WiFi.localIP().toString().c_str(), true);
  }

  // Ota
  uint16_t otaTimeStart = millis();
  if (otaFlag)
  {
    while (otaTimeElapsed < 20000)
    {
      ArduinoOTA.handle();
      otaTimeElapsed = millis() - otaTimeStart;
      delay(10);
    }
    otaFlag = false;
    Serial.println("Time up");
  }

  // Loop the mqtt
  client.loop();
}
