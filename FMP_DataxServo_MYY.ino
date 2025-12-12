// #include <ArduinoWebsockets.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h> //for ESP32
#include <ArduinoJson.h>
// #include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

//OOCSI Sending: 
#include "OOCSI.h"

//servo code: 
#include <ESP32Servo.h>
 
// WiFiClient client;
 
// WiFi credentials
const char* ssid = " ";
const char* password = " ";
 
const char* kitID = " ";
const char* apiURL = "https://api.smartcitizen.me/v0/devices/____";

const char* OOCSIName = " ";
const char* hostserver = "oocsi.id.tue.nl";

OOCSI oocsi = OOCSI ();

const int sensorIDs[5] = {53, 14, 55, 56, 112};
 
WiFiClientSecure client;
 
float air;
float temperature;
float humidity; 
float light;
float sound;

int CurrentTimeLight;
int CurrentTimeAir;
int CurrentTimeTemperature;
int CurrentTimeHumidity;
int CurrentTimeSound;

int PreviousTimeLight;
int PreviousTimeAir;
int PreviousTimeTemperature;
int PreviousTimeHumidity;
int PreviousTimeSound;

Servo Motor1;
Servo Motor2;
Servo Motor3;
Servo Motor4;
Servo Motor5;

// WebSocket server
//const char* socketServer = "wss://api.smartcitizen.me";
//const int port = 443;
 
 
void setup() {
  Serial.begin(115200);
    while (!Serial) { }
  Serial.println("Serial Monitor is working!");
 
  WiFi.begin(ssid, password);
 
  // Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
 
  Serial.println(WiFi.localIP());

  oocsi.connect (OOCSIName, hostserver, ssid, password);
  
  Motor1.attach (17);
  Motor2.attach (16);
  Motor3.attach (25);    
  Motor4.attach (26);
  Motor5.attach (13);

  delay (28800); // 8 min to make sure nothing would go wrong
}
 
void loop() {

  // delay (60000);
  for (int i = 0; i < 5; i++) {
    fetchdata(sensorIDs[i]);
  }
  sendOOCSI();
  mapdata();
  MotorMapsData();
  delay (360000); 
  backtotop();

}
 
 
void fetchdata(int sensorId) {
  if (WiFi.status() == WL_CONNECTED) {
 
    HTTPClient http;
    // WiFiClient client;  // Create a WiFiClient object
 
    // Start the HTTP GET request with WiFiClient
    String requestURL = apiURL + String(sensorId);// + String("&rollup=1m");
    Serial.println(apiURL);
    client.setPrivateKey("tS3Aqtl-eSb3ZJXF7b3jNx5KkaERqt281FA8qalxJcg");
    http.begin(apiURL);  // Pass WiFiClient and URL
    int httpResponseCode = http.GET();
 
 
    if (httpResponseCode > 0) {
      // Parse the response
      String response = http.getString();
      // Serial.println("API Response: ");
      // Serial.println(response);
      Serial.println("Raw JSON Response: ");
      Serial.println(response);
 
      // Parse JSON response
      StaticJsonDocument<2048> jsonDoc;
      DeserializationError error = deserializeJson(jsonDoc, response);
      if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.f_str());
        return;
      }
 
      // Extract relevant sensor data (adjust based on your API response structure)
      const char* deviceName = jsonDoc["name"];
      air = jsonDoc["data"]["sensors"][1]["value"]; // sensorID 112, eCO2
      light = jsonDoc["data"]["sensors"][2]["value"]; //sensorID 14, Light
      sound = jsonDoc["data"]["sensors"][4]["value"]; //sensorID 53, Sound
      humidity = jsonDoc["data"]["sensors"][9]["value"]; //sensorID 56, humidity
      temperature = jsonDoc["data"]["sensors"][10]["value"]; //sensorID 55, temp

 
      Serial.print("Device Name: ");
      Serial.println(deviceName);
      Serial.print("air: ");
      Serial.println(air);
      Serial.print("light: ");
      Serial.println(light);
      Serial.print("sound: ");
      Serial.println(sound);
      Serial.print("humidity: ");
      Serial.println(humidity);
      Serial.println("temperature: ");
      Serial.print(temperature);
      Serial.println("");
      // You can now send this data over WebSocket
    } else {
      Serial.print("HTTP GET failed, error: ");
      Serial.println(httpResponseCode);
    }
 
    http.end(); // End HTTP connection
  } else {
    Serial.println("WiFi not connected");
  }
}
 
void sendOOCSI() {

  oocsi.newMessage("FMPSCDataSaved_MYY");
  oocsi.addFloat ("sound", sound);
  oocsi.addFloat ("light", light);
  oocsi.addFloat ("air", air);
  oocsi.addFloat ("temperature", temperature);
  oocsi.addFloat ("humidity", humidity);
  oocsi.sendMessage();

  oocsi.printSendMessage();
}


void mapdata() { //the MD&C function
  CurrentTimeAir = map (int(air), 0, 5000, 0, 10000);
  CurrentTimeLight = map (int(light), 0, 500, 0, 10000);
  CurrentTimeTemperature = map (int(temperature), 10, 35, 0, 10000);
  CurrentTimeHumidity = map (int(humidity), 0, 100, 0, 10000);
  CurrentTimeSound = map (int(sound), 0, 60, 0, 10000);

  Serial.print("Air motor time to run: ");
  Serial.println (CurrentTimeAir);
  Serial.print("Light motor time to run: ");
  Serial.println (CurrentTimeLight);
  Serial.print("Temperature motor time to run: ");
  Serial.println (CurrentTimeTemperature);
  Serial.print("Humidity motor time to run: ");
  Serial.println (CurrentTimeHumidity);
  Serial.print("Sound motor time to run: ");
  Serial.println (CurrentTimeSound);
}

void MotorMapsData() { //the T&R function

  Serial.println("Motors are working for going down to map data ...");

  Motor2.writeMicroseconds(1700); 
  delay(CurrentTimeLight);
  Motor2.writeMicroseconds(1500);

  Motor3.writeMicroseconds(1300); //Air going down
  delay(CurrentTimeAir);
  Motor3.writeMicroseconds(1500);

  Motor4.writeMicroseconds(1700);
  delay(CurrentTimeTemperature);
  Motor4.writeMicroseconds(1500);

  Motor5.writeMicroseconds(1700);
  delay(CurrentTimeHumidity);
  Motor5.writeMicroseconds(1500);

  Motor1.writeMicroseconds(1300); //Sound going down
  delay(CurrentTimeSound);
  Motor1.writeMicroseconds(1500);

  PreviousTimeAir = CurrentTimeAir;
  PreviousTimeSound = CurrentTimeSound;
  PreviousTimeLight = CurrentTimeLight;
  PreviousTimeTemperature = CurrentTimeTemperature;
  PreviousTimeHumidity = CurrentTimeHumidity;
}

void backtotop() {

  Serial.println("Back to top has started.");

  Motor1.writeMicroseconds(1700); 
  delay(PreviousTimeSound);
  Motor1.writeMicroseconds(1500);

  Motor2.writeMicroseconds(1300); 
  delay(PreviousTimeLight);
  Motor2.writeMicroseconds(1500);

  Motor3.writeMicroseconds(1700);
  delay(PreviousTimeAir);
  Motor3.writeMicroseconds(1500);

  Motor4.writeMicroseconds(1300);
  delay(PreviousTimeTemperature);
  Motor4.writeMicroseconds(1500);

  Motor5.writeMicroseconds(1300);
  delay(PreviousTimeHumidity);
  Motor5.writeMicroseconds(1500);
}




