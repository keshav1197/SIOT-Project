#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

// WiFi details
char ssid[] = "VM798720-2G";
char pass[] = "******";

//Creating 2 clients, one for openweathermaps and the other for pushingbox
WiFiClient client1;
WiFiClient client2;

// Open Weather Map API server name
const char server[] = "api.openweathermap.org";

const char WEBSITE[] = "api.pushingbox.com"; //pushingbox API server
const String devid = "v4B10D9469C3D6E5"; //for humidity, r, g, b

// City, country code, for API call
String nameOfCity = "Merton,GB";

// My API Key
String apiKey = "12beb882b31e6ec9ca611f879fdd4324";

String text;
int jsonend = 0;
boolean startJson = false;
int status = WL_IDLE_STATUS;
#define JSON_BUFF_DIMENSION 2500

// Initialisation of the colour sensor, with specific integration time and gain
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

// Specifying the input pins for the sensor using the WIRE library
const uint8_t scl = D1;
const uint8_t sda = D2;

void setup() {

  Serial.begin(9600); // Initialise serial port

}

void loop() {

  text.reserve(JSON_BUFF_DIMENSION);

//Begind Wifi at the start of every loop
  WiFi.begin(ssid, pass);
  Serial.println("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");

//Find sensor at every loop
  Wire.begin(sda, scl);
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

//Calls the makehttprequest function
  makehttpRequest();

//After obtaining values and sending it to the Google sheet, the ESP, deepsleep for 1 hour (3600 seconds)
  ESP.deepSleep(3600e6);
}



// to request data from Openweathermaps
void makehttpRequest() {
  // close any connection before send a new request to allow client make connection to server
  client1.stop();

  // if there's a successful connection:
  if (client1.connect(server, 80)) {
    
    // send the HTTP PUT request:
    client1.println("GET /data/2.5/weather?q=" + nameOfCity + "&units=metric&APPID=" + apiKey);
    client1.println("Host: api.openweathermap.org");
    client1.println("User-Agent: ArduinoWiFi/1.1");
    client1.println("Connection: close");
    client1.println();

    // Else print timeout and stop the client
    unsigned long timeout = millis();
    while (client1.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client1.stop();
        return;
      }
    }

    Initialising the string that is going to store the json object from openweathermaps//
    char c = 0;
    while (client1.available()) {
      c = client1.read();
      // since json contains equal number of open and close curly brackets, this means we can determine when a json is completely received  by counting
      // the open and close occurences,
      Serial.print(c);
      if (c == '{') {
        startJson = true;         // set startJson true to indicate json message has started
        jsonend++;
      }
      if (c == '}') {
        jsonend--;
      }
      if (startJson == true) {
        text += c;
      }
      // if jsonend = 0 then we have have received equal number of curly braces
      if (jsonend == 0 && startJson == true) {
        parseandsend(text.c_str());  // parse c string text in parseJson function
        text = "";                // clear text string for the next time
        startJson = false;        // set startJson to false to indicate that a new message has not yet started
      }
    }
  }
  else {
    // if no connction was made:
    Serial.println("connection failed");
    return;
  }
}

//to parse json data recieved from OWM
void parseandsend(const char * jsonString) {
  const size_t bufferSize = 2 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(2) + 4 * JSON_OBJECT_SIZE(1) + 3 * JSON_OBJECT_SIZE(2) + 3 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 2 * JSON_OBJECT_SIZE(7) + 2 * JSON_OBJECT_SIZE(8) + 720;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // FIND FIELDS IN JSON TREE
  JsonObject& root = jsonBuffer.parseObject(jsonString);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  //Searching the main json object
  JsonObject& main = root["main"];

  //Searching for humidity inside the main object and setting that to float humidityData
  float humidityData = main["humidity"];
  Serial.println();
  Serial.println(humidityData);
  Serial.println();

  delay(500);

  //Initialising the variables for the colour sensor
  uint16_t r, g, b, c, colorTemp, lux;

  //calling raw data
  tcs.getRawData(&r, &g, &b, &c);

  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");

  //Connecting to pushing box and sending the himidityData, r, g, and b values
  if (client2.connect(WEBSITE, 80))
  {
    Serial.println("Connecting to Google sheet");
    client2.print("GET /pushingbox?devid=" + devid
                  + "&humidityData=" + (String) humidityData
                  + "&r=" + (String) r
                  + "&g=" + (String) g
                  + "&b=" + (String) b
                 );
    client2.println(" HTTP/1.1");
    client2.print("Host: ");
    client2.println(WEBSITE);
    client2.println("User-Agent: ESP8266/1.0");
    client2.println("Connection: close");
    client2.println();
    Serial.println("Data sent"); //Return to main loop to deepsleep
  }
}
