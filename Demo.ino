#include <WiFiManager.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <LITTLEFS.h>
#include <MAX44009.h>
#include <Adafruit_MCP9808.h>
#include "neocampus_debug.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
//#include <avr/wdt.h>

#define CREDENTIALS "/cred.txt"
#define CONFIG "/conf.txt"

#define DEFT_LOGIN "test"
#define DEFT_PWD "test"
#define DEFT_SERVER "195.220.53.10"
#define DEFT_TOPIC "testTopic/display"
#define DEFT_TOPIC_CLASS "testTopic/display/command"
#define DEFT_PORT 10483
#define DEFT_ID "UberSensor_demo"

#define SDA1 21
#define SCL1 22

#define pinButtonChange 2
#define pinButtonReset 34

//Screen address
#define SSD1306 0x3c 
//Screen setup
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

#define NEOCAMPUS_WIDTH 128
#define NEOCAMPUS_HEIGHT 25

const unsigned char neOCampus_bmp [] PROGMEM=
{
B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
B00000000, B00000000, B00000000, B00000000, B00011000, B00011100, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
B00000000, B00000000, B00000000, B00000000, B01111110, B01111111, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
B00000000, B00000000, B00000000, B00000000, B11111100, B11111111, B11000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
B00000000, B00000000, B00000000, B00000001, B11000001, B11000000, B11000000, B11110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000001, B11000000,
B00110011, B11100000, B00111111, B00000011, B10000011, B10000000, B00000011, B11111100, B00010011, B11100011, B11100000, B01001111, B11000001, B10000000, B11000111, B11110000,
B00110111, B11110000, B01111111, B10000111, B00000111, B00100000, B00000011, B00001110, B00011111, B11110111, B11110000, B01111111, B11100001, B10000000, B11001100, B00110000,
B00111100, B00111000, B11100001, B11000110, B00000110, B01100000, B00000000, B00000110, B00011100, B00111100, B00111000, B01110000, B01100001, B10000000, B11001100, B00000000,
B00111000, B00011000, B11000000, B11000110, B00000110, B01100000, B00000000, B00000110, B00011000, B00011000, B00011000, B01100000, B00110001, B10000000, B11001100, B00000000,
B00110000, B00011000, B11000000, B11101110, B00001110, B01110000, B00000000, B00000110, B00011000, B00011000, B00011000, B01100000, B00110001, B10000000, B11000110, B00000000,
B00110000, B00011001, B11111111, B11101110, B00001110, B01110000, B00000000, B11111110, B00011000, B00011000, B00011000, B01100000, B00110001, B10000000, B11000011, B11000000,
B00110000, B00011001, B11111111, B11001110, B00001110, B01110000, B00000011, B11111110, B00011000, B00011000, B00011000, B01100000, B00110001, B10000000, B11000001, B11100000,
B00110000, B00011001, B11000000, B00001110, B00001110, B01110000, B00000111, B00000110, B00011000, B00011000, B00011000, B01100000, B00110001, B10000000, B11000000, B00110000,
B00110000, B00011000, B11000000, B00000110, B00000110, B01100000, B00000110, B00000110, B00011000, B00011000, B00011000, B01100000, B00110001, B10000000, B11000000, B00011000,
B00110000, B00011000, B11000000, B11000110, B00000110, B01100000, B00000110, B00001110, B00011000, B00011000, B00011000, B01100000, B01110001, B10000000, B11000000, B00011000,
B00110000, B00011000, B11100001, B11000111, B00000100, B11100000, B00000110, B00011110, B00011000, B00011000, B00011000, B01110000, B11100001, B11000011, B11001100, B00011000,
B00110000, B00011000, B01111111, B10000011, B10000001, B11000000, B00000111, B11110110, B00011000, B00011000, B00011000, B01111111, B11000000, B11111111, B11001110, B00111000,
B00110000, B00011000, B00111111, B00000001, B11000011, B10000000, B11000011, B11100110, B00011000, B00011000, B00011000, B01101111, B10000000, B01111100, B11000111, B11110000,
B00000000, B00000000, B00000000, B00000000, B11111111, B00111111, B11000000, B00000000, B00000000, B00000000, B00000000, B01100000, B00000000, B00000000, B00000001, B11000000,
B00000000, B00000000, B00000000, B00000000, B01111110, B01111111, B10000000, B00000000, B00000000, B00000000, B00000000, B01100000, B00000000, B00000000, B00000000, B00000000,
B00000000, B00000000, B00000000, B00000000, B00011000, B00011100, B00000000, B00000000, B00000000, B00000000, B00000000, B01100000, B00000000, B00000000, B00000000, B00000000,
B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B01100000, B00000000, B00000000, B00000000, B00000000,
B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B01100000, B00000000, B00000000, B00000000, B00000000,
B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B01100000, B00000000, B00000000, B00000000, B00000000,
B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000
};


//enum mode screen
typedef enum {
  Date,
  Temperature,
  Luminosity,
  Customized_message
} enum_mode_print;

/* MAC vars */
uint8_t macAddr[6];
char strMacAddr[32];

/*WiFiManager vars */
char ssid[20]; /*WM ssid defined by device's class + part of MAC address*/
const char* pwd = "password"; /* need 8 char pwd else autoConnect fails.*/
WiFiManager wm;

/*HTTPS vars*/
HTTPClient httpsClient;
const char* root_ca = NULL;
char url [90];
const char* config_server = ""; //"https://sensocampus.univ-tlse3.fr/device/config";
/* JSon Vars */
StaticJsonDocument<200> JSON_cred;
StaticJsonDocument<1000> JSON_conf;/*
const char* cred_login = JSON_cred["login"];
const char* cred_pwd = JSON_cred["password"];
const char*  cred_server = JSON_cred["server"];
int cred_port = JSON_cred["port"];*/


/* MQTTS vars */
const char* _cacert = NULL;
const char* _clicert = nullptr;
const char* _clikey = nullptr;
WiFiClient *wcsClient = new WiFiClient;
PubSubClient client(*wcsClient);

//I2C bus
TwoWire I2Cone = TwoWire(0);

//global variable
int address_devices[5];
enum_mode_print screen_mode;
enum_mode_print last_seen;

float* lum_data;
float* temp_data;

WiFiUDP ntpUDP;
//3600 for UTC+1
//NTP =====================================================================
const char* ntpServer = "pool.ntp.org";
//Sensors
MAX44009 lumSensor;
Adafruit_MCP9808 tempSensor;

String formattedDate;
int splitT;
volatile int counter = 0;
float temp;
float lux;

void get_MAC(){
  log_info(F("\n---------------------\n"));log_info(F("get_MAC()"));log_info(F("\n---------------------\n"));
  Serial.println();
  Serial.print("ESP BOARD MAC Address: ");
  WiFi.macAddress(macAddr);
  snprintf( strMacAddr, sizeof(strMacAddr), "%02X:%02X:%02X:%02X:%02X:%02X", macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
  snprintf(ssid, sizeof(ssid),"%s%02X:%02X","m2_Demo_",macAddr[4], macAddr[5]);
  Serial.println(strMacAddr);
  Serial.println(ssid);
  Serial.flush();
  log_info(F("\n---------------------\n"));log_info(F("end of get_MAC()"));log_info(F("\n---------------------\n"));
}

void set_WiFi(){
  log_info(F("\n---------------------\n"));log_info(F("set_Wifi()"));log_info(F("\n---------------------\n"));
  //wm.resetSettings();
  WiFi.mode(WIFI_STA);
  if(!wm.autoConnect(ssid,pwd)){
    log_error(F("Connection failed"));
  }
  log_info(F("\n---------------------\n"));log_info(F("end of set_Wifi()"));log_info(F("\n---------------------\n"));
}

void IRAM_ATTR button_Pressed_Change(){
  log_info(F("\n---------------------\n"));log_info(F("button_pressed_change()"));log_info(F("\n---------------------\n"));
   switch(screen_mode){
      case Date:
      log_debug(F("DATE ---> TEMP\n"));
        screen_mode = Temperature;
        break;
      case Temperature:
      log_debug(F("TEMP ---> LUM\n"));
        screen_mode = Luminosity;
        break;
      case Luminosity:
      log_debug(F("LUM ----> DATE\n"));
        screen_mode = Date;
        break;
      case Customized_message:
        switch (last_seen){
          case Date:
          log_debug(F("Custom ---> TEMP\n"));
            screen_mode = Temperature;
            break;
          case Temperature:
          log_debug(F("Custom ---> LUM\n"));
            screen_mode = Luminosity;
            break;
          case Luminosity:
          log_debug(F("Custom ----> DATE\n"));
            screen_mode = Date;
            break;
          }
        break;
      default:
        log_error(F("\n[setupLed] unknwown screen_mode ?!?!"));
    }
  counter = 0;
  log_info(F("\n---------------------\n"));log_info(F("button_pressed_change()"));log_info(F("\n---------------------\n"));
  return;
}


void get_conf(){
  log_info(F("\n---------------------\n"));log_info(F("mqtt_get_conf()"));log_info(F("\n---------------------\n"));
  const char* cred_login = JSON_cred["login"];
  const char* cred_pwd = JSON_cred["password"];
  const char*  conf_server = JSON_cred["server"];
  int conf_port = JSON_cred["port"];
  httpsClient.begin(config_server, root_ca );
  httpsClient.setAuthorization(cred_login, cred_pwd);
  int httpCode = httpsClient.GET();
   if (httpCode > 0) { //Check for the returning code
     String payload = httpsClient.getString();
     log_debug(F("HTTP code received :"));log_debug(F(httpCode));log_debug(F("\n"));
     //Serial.println(payload);
     /* Unboxing credentials from cred server */
     DeserializationError error = deserializeJson(JSON_conf, payload);
     // Test if parsing succeeds.
     if (error) {
       log_error(F("deserializeJson() failed: ")); log_error(F(error.c_str())); log_error(F("\n"));
       return;
     }
     serializeJsonPretty(JSON_conf,Serial);
     File file = LITTLEFS.open(CONFIG, "w");
      if (!file) {
        log_error(F("unable to open config file\n"));
        return;
      }
      if (serializeJson(JSON_conf, file) == 0) {
        log_error(F("unable to write into config file\n"));
      } else {
        log_debug(F("Config successfully saved\n"));
      }
      file.close();
   } 
   httpsClient.end();
  log_info(F("\n---------------------\n"));log_info(F("end of mqtt_get_conf()"));log_info(F("\n---------------------\n"));
  return;
}

void callback(char* topic, byte* payload, unsigned int length){
  StaticJsonDocument<200> msg;
  //Pas bon pour récuperer l'objet
  char json[] = (char*)payload;
  log_info(F("\n---------------------\n"));log_info(F("mqtt_callback()"));log_info(F("\n---------------------\n"));
  log_debug(F("[")); log_debug(F(topic)); log_debug(F("]:"));
  if(strncmp(topic,DEFT_TOPIC_CLASS,size_t(sizeof(DEFT_TOPIC_CLASS)))==0){
    log_debug(F("message recevied\n")); 
    //if (strncmp((char*)payload,"Change",length)==0){
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(msg, json);
    //TODO CHANGER L'AFFICHAGE
   // Test if parsing succeeds.
   if (error) {
     Serial.print(F("deserializeJson() failed: "));
     // f_string ou c_string :shrug:
     Serial.println(error.c_str());
     return;
   }
   if(msg["order"] == "Change"){
    
    //TODO CHANGER L'AFFICHAGE
    switch(screen_mode){
      case Date:
      log_debug(F("DATE ---> TEMP\n"));
        screen_mode = Temperature;
        break;
      case Temperature:
      log_debug(F("TEMP ---> LUM\n"));
        screen_mode = Luminosity;
        break;
      case Luminosity:
      log_debug(F("LUM ----> DATE\n"));
        screen_mode = Date;
        break;
      case Customized_message:
        switch (last_seen){
          case Date:
          log_debug(F("Custom ---> TEMP\n"));
            screen_mode = Temperature;
            break;
          case Temperature:
          log_debug(F("Custom ---> LUM\n"));
            screen_mode = Luminosity;
            break;
          case Luminosity:
          log_debug(F("Custom ----> DATE\n"));
            screen_mode = Date;
            break;
          }
        break;
      default:
        log_error(F("\n[setupLed] unknwown screen_mode ?!?!"));
    }
   }else if msg["order"] == "Custom"{
    if (screen_mode != Date and screen_mode != Luminosity and screen_mode != Temperature){
      //Si il n y a aps de last_seen ?
      if (!last_seen){
        last_seen = Date
      }
    }
    else{
      last_seen = screen_mode;
    }
    screen_mode = Customized_message;
    //Print le message custom
    counter = 0;
   }
   else{
    log_warning(F("Received a non Change Order message"));
   }
  }else{
    log_warning(F("Received a non-airquality related message"));
  }
  log_info(F("\n---------------------\n"));log_info(F("end of mqtt_callback()"));log_info(F("\n---------------------\n"));
  return;
}


void mqtt(){
  log_info(F("\n---------------------\n"));log_info(F("mqtt_init()"));log_info(F("\n---------------------\n"));
  if(wcsClient) {
    client.setServer(DEFT_SERVER, DEFT_PORT);
    client.setCallback(callback);
    //if(client.connect(conf_server,cred_login, cred_pwd)){
      if(client.connect(DEFT_ID, DEFT_LOGIN, DEFT_PWD)){
        log_debug(F("Client login:"));log_debug(F(DEFT_LOGIN));
        log_debug(F("Client password:"));log_debug(F(DEFT_PWD));log_debug(F("\n"));
        log_debug(F("Client ID:"));log_debug(F(DEFT_ID));log_debug(F("\n"));
        if(!client.subscribe(DEFT_TOPIC_CLASS)){
          log_debug(F("Client failed to subscribe to "));
        }else{
          log_debug(F("Client subscribed to "));
        }
        log_debug(F(DEFT_TOPIC_CLASS));
      }else
      log_warning(F("MQTT connection failed."));
  }else{
    log_warning(F("Failed to set up WiFiClientSecure"));
  } 
  log_info(F("\n---------------------\n"));log_info(F("end of mqtt_init()"));log_info(F("\n---------------------\n"));
  return;
}

void reconnect(){
  log_info(F("\n---------------------\n"));log_info(F("mqtt_reconnect()"));log_info(F("\n---------------------\n"));
  while(!client.connected()){
    log_debug(F("MQTT client, reconnecting...\n"));
    if(client.connect(DEFT_SERVER)){
      log_debug(F("MQTT client reconnected\n"));
      client.publish(DEFT_TOPIC,"hello world");
      client.subscribe(DEFT_TOPIC_CLASS);
    }else{
      log_error(F("MQTT client failed, rc="));log_error(client.state());log_error(F(" try again in 5 seconds\n"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  log_info(F("\n---------------------\n"));log_info(F("end of mqtt_reconnect()"));log_info(F("\n---------------------\n"));
  return;
}

void printLocalTime(){
  char str_hms[13];
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(str_hms, 13, "%H:%M:%S",&timeinfo);
  display.clearDisplay();
  display.setCursor(15, 0);
  display.println(str_hms);
  display.drawLine(0, 24, 120, 24, WHITE);
  //display.drawLine(71, 28, 120, 28, WHITE);
  display.setCursor(40, 34);
  return;
}

void printLocalDate()
{
  char str_Ddm[11];
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    log_error(F("Failed to obtain time"));
    return;
  }
  strftime(str_Ddm, 22, "%a %d %b",&timeinfo);
  display.setCursor(0, 34);
  display.println(str_Ddm);
  return;
}

void scanner ()
{
  log_info(F("\n---------------------\n"));log_info(F("i2c _scannner()"));log_info(F("\n---------------------\n"));
  byte count = 0;
  lumSensor = MAX44009();
  tempSensor = Adafruit_MCP9808();
  Wire.begin();
  for (byte i = 8; i < 120; i++){
    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0){
      //log_debug(F("Found address: 0x")); log_debug((i, HEX)); log_debug(F("\n")); log_flush();
      Serial.print("Found address: "); Serial.print("0x"); Serial.println(i, HEX);
      address_devices[count] = i;
      count++; 
      if (lumSensor.is_device(i)){
        log_debug(F("found sensor in MAX44009 address pool\n"));log_flush();
        if(lumSensor.begin(i)){
          delay(500);
        }
      }else{        
        Serial.print("\n");
        Serial.print(i,HEX);
        Serial.println (": not luminosity sensor");
      }
      if(tempSensor.is_device(i)){
        log_debug(F("found sensor in MCP9808 address pool\n"));log_flush();
        if(tempSensor.begin(i)){
          delay(500);
          tempSensor.setResolution(MCP9808_RESOLUTION_00625DEG);
        }
      }else{
        Serial.print("\n");
        Serial.print(i,HEX);
        Serial.println (": not temperature sensor");
      }
    }
  }
  log_debug(F("i2c scan device count: "));log_debug(count);log_debug(F("\n"));
  log_info(F("\n---------------------\n"));log_info(F("end of i2c_scannner()"));log_info(F("\n---------------------\n"));
}

void testdrawbitmap(void) {
  log_info(F("\n---------------------\n"));log_info(F("drawbitmap()"));log_info(F("\n---------------------\n"));
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - NEOCAMPUS_WIDTH ) / 2,
    (display.height() - NEOCAMPUS_HEIGHT) / 2,
    neOCampus_bmp, NEOCAMPUS_WIDTH, NEOCAMPUS_HEIGHT, 1);
  display.display();
  delay(1000);
  
  log_info(F("\n---------------------\n"));log_info(F("end of drawbitmap()"));log_info(F("\n---------------------\n"));
}
void setup() {
  Wire.begin();
  Wire.setClock(100000);
  delay(3000);
  // put your setup code here, to run once:
  Serial.begin(115200);
  log_info(F("\n---------------------\n"));log_info(F("setup()"));log_info(F("\n---------------------\n"));
  log_debug(F("debug mode enabled\n"));
  delay(1000);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS,100000)) {
    log_debug(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setTextSize(2);
  display.cp437(true);
  display.setTextColor(SSD1306_WHITE);
  
  testdrawbitmap();
  bool success = LITTLEFS.begin();
 
  if (success) {
    log_debug(F("File system mounted with success"));
  } else {
    log_debug(F("Error mounting the file system"));
    return;
  }
  get_MAC();
  set_WiFi();
  mqtt();
  scanner();
  I2Cone.begin(SDA1,SCL1,400000); // SDA pin 21, SCL pin 22
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  
  screen_mode = Date;
  configTime(3600, 3600, ntpServer);
  temp_data = (float *)malloc(sizeof(float));
  lum_data = (float *)malloc(sizeof(float));
  *lum_data = 0.0;
  *temp_data=0.0;
  log_debug(F("\n---------------------\n"));log_debug(F("Fetching data"));log_debug(F("\n---------------------\n"));
  while(!lumSensor.acquire(lum_data)){
    log_debug(F("\nluminosity init value not acquired\n"));
      delay(800);
  }
  log_debug(F("\nluminosity value acquired in setup\n"));

   if(tempSensor.acquire(temp_data)){
    log_debug(F("\ntemperature value acquired in setup\n"));
  }else{
    log_debug(F("\ntemperature init value not acquired\n"));
  }
  log_debug(F("\n---------------------\n"));log_debug(F("Fetching done"));log_debug(F("\n---------------------\n"));
  //interrupt button change
  attachInterrupt(digitalPinToInterrupt(pinButtonChange), button_Pressed_Change, RISING);
  log_info(F("\n---------------------\n"));log_info(F("end of setup()"));log_info(F("\n---------------------\n"));
}

void loop() {
  counter++;
  delay(100);
  display.clearDisplay();
  switch(screen_mode){
    case Date:
      printLocalTime();
      printLocalDate();
      //Mettre a jour la date et l'afficher
      break;
    case Temperature:
      //Mettre a jour la temperature et l'afficher
      // Read and print out the temperature, also shows the resolution mode used for reading.
      printLocalTime();
      display.println(F("Temp:"));  
      if(counter==1){
        if(tempSensor.acquire(temp_data)){
          log_debug(F("\nTemperature :"));log_debug(*temp_data);log_debug(F(" C\n"));
          //Serial.print(f, 4); Serial.println("*F.");
        }
      }
        display.print(*temp_data);
        display.println(F(" C"));
      //}else{
        //display.println(F("no data"));
      //}
      break;
      
    case Luminosity:
      //Mettre a jour la luminosité et l'afficher
      printLocalTime();
      display.println(F("Lum:"));
      if(counter==1){
        if(lumSensor.acquire(lum_data)){
          log_debug(F("\nLuminosity :"));log_debug(*lum_data);log_debug(F(" lux\n"));
        }
      }
        display.print(*lum_data);
        display.println(F(" lux"));
      //}else{
       // display.println(F("no data"));
      //}
      break;
    default:
      log_error(F("\n[setupLed] unknwown screen_mode ?!?!"));
  }
  display.display();
  if (counter == 90) {  
      if(!client.connected()){
        log_warning(F("Connection lost, reconnecting... "));
        reconnect();
      }
      cmd["order"]="change";   
      boolean rc = client.publish(DEFT_TOPIC_CLASS, cmd); 
      counter = 0;
  }
  client.loop();
  
  Serial.print(".");
  Serial.flush();
}
