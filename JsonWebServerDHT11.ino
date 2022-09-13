#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>


#include <SoftwareSerial.h>

#define RE 32
#define DE 33

  byte n,p,k;
 
//const byte code[]= {0x01, 0x03, 0x00, 0x1e, 0x00, 0x03, 0x65, 0xCD};
const byte nitro[] = {0x01,0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01,0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01,0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};
 
byte values[11];
SoftwareSerial mod(26,27);


const char* ssid = "MALUPONTON";
const char* password = "Paulette20";


// PARAMETROS PARA CONTROL DE SALIDA
const char* PARAM_INPUT_OUTPUT = "output";
const char* PARAM_INPUT_STATE = "state";

// DEFINIR NUMERO DE SALIDAS 
#define NUM_OUTPUTS 4

// ARRAY DE SALIDAS 
int outputGPIOs[NUM_OUTPUTS] = {18};

// OBTENER ESTADO DE SALIDAS
String getOutputStates(){
 JSONVar myArray;
 for (int i =0; i<NUM_OUTPUTS; i++){
 myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
 myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
 //myArray["gpios"][i] ="{\"output\":\"" + String(outputGPIOs[i]) + "\", \"state\": \"" + String(digitalRead(outputGPIOs[i])) + "\"}";
 }
 String jsonString = JSON.stringify(myArray);
 Serial.print(jsonString);
 return jsonString;
}


AsyncWebServer server(80);
// Create an Event Source on /events
AsyncEventSource events("/events");
// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// CONFIGURACION DE DHT11 (PINES TIPO Y DECLARACION)
#define DHTPIN 15
#define DHTTYPE    DHT11
DHT dht(DHTPIN, DHTTYPE);

String getSensorReadings(){
 readings["temperature"] = String(dht.readTemperature());
 readings["humidity"] = String(dht.readHumidity());
 readings["nitrogen"] = String(n);
 readings["potassium"] = String(p);
 readings["phosphorus"] = String(k);
 
 String jsonString = JSON.stringify(readings);
 return jsonString;
}

void initSPIFFS() {
 if (!SPIFFS.begin()) {
 Serial.println("An error has occurred while mounting LittleFS");
 }
 Serial.println("LittleFS mounted successfully");
}

void initWiFi() {
 WiFi.mode(WIFI_STA);
 WiFi.begin(ssid, password);
 Serial.print("Connecting to WiFi ..");
 while (WiFi.status() != WL_CONNECTED) {
 Serial.print('.');
 delay(1000);
 }
 Serial.println(WiFi.localIP());
}



void setup() {
  // put your setup code here, to run once:
initWiFi();
initSPIFFS();
mod.begin(9600);
Serial.begin(115200);
dht.begin();
initWiFi();
initSPIFFS();
  
 pinMode(RE, OUTPUT);
 pinMode(DE, OUTPUT);

 // Set GPIOs as outputs
 for (int i =0; i<NUM_OUTPUTS; i++){
 pinMode(outputGPIOs[i], OUTPUT);
 }


server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
 request->send(SPIFFS, "/index.html", "text/html");
 });

 server.serveStatic("/", SPIFFS, "/");

 server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
 String json = getSensorReadings();
 request->send(200, "application/json", json);
 json = String();
 });

events.onConnect([](AsyncEventSourceClient *client){
 if(client->lastId()){
 Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
}
 // send event with message "hello!", id current millis
 // and set reconnect delay to 1 second
 client->send("hello!", NULL, millis(), 10000);
 });
 server.addHandler(&events);

  server.on("/states", HTTP_GET, [](AsyncWebServerRequest *request){
 String json = getOutputStates();
 request->send(200, "application/json", json);
 json = String();
 });

 server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
 String output;
 String state;
 // GET input1 value on <ESP_IP>/update?output=<output>&state=<state>
 if (request->hasParam(PARAM_INPUT_OUTPUT) && request->hasParam(PARAM_INPUT_STATE)) {
 output = request->getParam(PARAM_INPUT_OUTPUT)->value();
 state = request->getParam(PARAM_INPUT_STATE)->value();
 // Control GPIO
 digitalWrite(output.toInt(), state.toInt());
 }
 else {
 output = "No message sent";
 state = "No message sent";
 }
 Serial.print("GPIO: ");
 Serial.print(output);
 Serial.print(" - Set to: ");
 Serial.println(state);

 request->send(200, "text/plain", "OK");
 });
 // Start server
 server.begin();


}

void loop() {
  n = nitrogen();
  delay(250);
  p = phosphorous();
  delay(250);
  k = potassium();
  delay(250);
  
  // put your main code here, to run repeatedly:
 if ((millis() - lastTime) > timerDelay) {
 // Send Events to the client with the Sensor Readings Every 30 seconds
 events.send("ping",NULL,millis());
 events.send(getSensorReadings().c_str(),"new_readings" ,millis());
 lastTime = millis();
 }
}

byte nitrogen(){
    digitalWrite(DE,HIGH);
    digitalWrite(RE,HIGH);
    delay(10);
    if(mod.write(nitro,sizeof(nitro))==8){
      digitalWrite(DE,LOW);
      digitalWrite(RE,LOW);
      for(byte i=0;i<7;i++){
      //Serial.print(mod.read(),HEX);
      values[i] = mod.read();
      Serial.print(values[i],HEX);
      }
      Serial.println();
    }
    return values[4];
  }
   
  byte phosphorous(){
    digitalWrite(DE,HIGH);
    digitalWrite(RE,HIGH);
    delay(10);
    if(mod.write(phos,sizeof(phos))==8){
      digitalWrite(DE,LOW);
      digitalWrite(RE,LOW);
      for(byte i=0;i<7;i++){
      //Serial.print(mod.read(),HEX);
      values[i] = mod.read();
      Serial.print(values[i],HEX);
      }
      Serial.println();
    }
    return values[4];
  }
   
  byte potassium(){
    digitalWrite(DE,HIGH);
    digitalWrite(RE,HIGH);
    delay(10);
    if(mod.write(pota,sizeof(pota))==8){
      digitalWrite(DE,LOW);
      digitalWrite(RE,LOW);
      for(byte i=0;i<7;i++){
      //Serial.print(mod.read(),HEX);
      values[i] = mod.read();
      Serial.print(values[i],HEX);
      }
      Serial.println();
    }
    return values[4];
  }
