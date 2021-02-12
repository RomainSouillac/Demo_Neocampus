
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <LITTLEFS.h>
#include <Max44009.h>
#include <Adafruit_MCP9808.h>
#include "neocampus_debug.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"

#define CREDENTIALS "/cred.txt"
#define CONFIG "/conf.txt"

#define DEFT_LOGIN "test"
#define DEFT_PWD "test"
#define DEFT_SERVER "195.220.53.10"
#define DEFT_TOPIC "testTopic/display"
#define DEFT_TOPIC_CLASS "testTopic/display/command"
#define DEFT_PORT 10483


#define SDA1 21
#define SCL1 22

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
  Luminosity
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

WiFiUDP ntpUDP;
//3600 for UTC+1
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600);
//NTP =====================================================================
const char* ntpServer = "pool.ntp.org";
//Sensors
Max44009 lumSensor;
Adafruit_MCP9808 tempSensor;

String formattedDate;
int splitT;
int counter = 0;
float temp;
float lux;
  
void get_MAC(){
  Serial.println();
  Serial.print("ESP BOARD MAC Address: ");
  WiFi.macAddress(macAddr);
  snprintf( strMacAddr, sizeof(strMacAddr), "%02X:%02X:%02X:%02X:%02X:%02X", macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
  snprintf(ssid, sizeof(ssid),"%s%02X:%02X","m2_Demo_",macAddr[4], macAddr[5]);
  Serial.println(strMacAddr);
  Serial.println(ssid);
  //snprintf(url, sizeof(url), "%s%s","https://sensocampus.univ-tlse3.fr/device/credentials?mac=",strMacAddr);
  //Serial.println(url);
  Serial.flush();
}

void set_WiFi(){
  //wm.resetSettings();
  WiFi.mode(WIFI_STA);
  if(!wm.autoConnect(ssid,pwd))
    Serial.println("Conn Doomed");
  else{
    Serial.println("Conn est");
  }
}

void get_conf(){
  Serial.println("Attempting to get conf");
  const char* cred_login = JSON_cred["login"];
  const char* cred_pwd = JSON_cred["password"];
  const char*  conf_server = JSON_cred["server"];
  int conf_port = JSON_cred["port"];
  httpsClient.begin(config_server, root_ca );
  httpsClient.setAuthorization(cred_login, cred_pwd);
  int httpCode = httpsClient.GET();
   if (httpCode > 0) { //Check for the returning code
     String payload = httpsClient.getString();
     Serial.println(httpCode);
     //Serial.println(payload);
     /* Unboxing credentials from cred server */
     DeserializationError error = deserializeJson(JSON_conf, payload);
     // Test if parsing succeeds.
     if (error) {
       Serial.print(F("deserializeJson() failed: "));
       Serial.println(error.c_str());
       return;
     }
     serializeJsonPretty(JSON_conf,Serial);
     File file = LITTLEFS.open(CONFIG, "w");
      if (!file) {
        Serial.println("Error opening file for writing");
        return;
      }
      if (serializeJson(JSON_conf, file) == 0) {
        Serial.println(F("Failed to write to config.txt"));
      } else {
        Serial.println(F("Config successfully saved"));
      }
      file.close();
   } 
   httpsClient.end();
}
void callback(char* topic, byte* payload, unsigned int length){
  //byte* p = (byte*)malloc(length);
  //memcpy(p, payload,length);
  //String message;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  if(strncmp(topic,DEFT_TOPIC_CLASS,size_t(sizeof(DEFT_TOPIC_CLASS)))==0){
    Serial.println(F(": message received"));
    //for (int i=0;i<length;i++) {
    //  Serial.print((char)payload[i]);
     // message += ((char)payload[i]);
    //}
    //message +='\0';
    Serial.println(length);
   if (strncmp((char*)payload,"Change",length)==0){
    //TODO CHANGER L'AFFICHAGE
    switch(screen_mode){
      case Date:
      Serial.println(F("Changer pour temp"));
        screen_mode = Temperature;
        break;
      case Temperature:
      Serial.println(F("Changer pour Lum"));
        screen_mode = Luminosity;
        break;
      case Luminosity:
      Serial.println(F("Changer pour Date"));
        screen_mode = Date;
        break;
      default:
        log_error(F("\n[setupLed] unknwown screen_mode ?!?!"));
    }
   }else{
    Serial.println(F("Received a non Change Order message"));
   }
  }else{
    Serial.println(F("Received a non-airquality related message"));
  }
  //free(p);
}

void mqtt(){
  Serial.println("Attempting to get conf");
  if(wcsClient) {
    Serial.println(DEFT_SERVER);
    Serial.println(DEFT_PORT);
    client.setServer(DEFT_SERVER, DEFT_PORT);
    client.setCallback(callback);
    //if(client.connect(conf_server,cred_login, cred_pwd)){
      if(client.connect("toto", DEFT_LOGIN, DEFT_PWD)){
        Serial.println(F("Client connected"));
        if(!client.subscribe(DEFT_TOPIC_CLASS)){
          Serial.println("Failed to subscribe to ");
          Serial.println(DEFT_TOPIC_CLASS);
        }else{
          Serial.println("subscribing done to ");
          Serial.println(DEFT_TOPIC_CLASS);
        }
      }else
      Serial.println(F("MQTT connection failed."));
  }else{
    Serial.println("Failed to set up WiFiClientSecure");
  }
}

void reconnect(){
  while(!client.connected()){
    Serial.print(F("MQTT connection pending..."));
    if(client.connect(DEFT_SERVER)){
      Serial.println(F("Connected"));
      client.publish(DEFT_TOPIC,"hello world");
      client.subscribe(DEFT_TOPIC_CLASS);
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



void printLocalTime()
{
  char str_hms[6];
  char str_adb[13];
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  char buf[15];

  /*uint8_t mo = month();  // month (1 to 12)
  uint8_t dd = day();  // day of month
  uint8_t hh = hour();  // hour
  uint8_t mi = minute();  // minute

  snprintf(buf, "%c%c%c %02d - %02d:%02d",
          ("BJFMAMJJASOND"[(mo <= 12) ? mo : 0]),
          ("aaeapauuuecoe"[(mo <= 12) ? mo : 0]),
          ("dnbrrynlgptvc"[(mo <= 12) ? mo : 0]),
          dd, hh, mi);
          
  strftime(str_dd, sizeof(), "%3A %B %3d",&timeinfo);*/
  strftime(str_hms, 6, "%H:%M",&timeinfo);
  display.clearDisplay();
  display.setTextSize(2);
  display.cp437(true);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 0);
  //display.println(buf);
  display.println(str_hms);
  display.display();
}



void scanner ()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
  
  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print("Found address: "); Serial.print("0x"); Serial.println(i, HEX); 
      address_devices[count] = i;
      count++;
      if (i == 0x4B or i == 0x4A){
        lumSensor = Max44009((i,HEX),SDA,SCL);
      }
      if (i == 0x1F ){
        Serial.println ("Found temperature sensor");
        tempSensor = Adafruit_MCP9808();
          tempSensor.begin((i,HEX));
          delay(500);
          tempSensor.setResolution((3,HEX));
      }
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s).");
}

void testdrawbitmap(void) {
  display.clearDisplay();
  display.drawBitmap(
    (display.width()  - NEOCAMPUS_WIDTH ) / 2,
    (display.height() - NEOCAMPUS_HEIGHT) / 2,
    neOCampus_bmp, NEOCAMPUS_WIDTH, NEOCAMPUS_HEIGHT, 1);
  display.display();
  delay(1000);
  
}
void setup() {
  Wire.begin();
  Wire.setClock(50000);
  delay(3000);
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(F("Hello ..."));
  delay(1000);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  testdrawbitmap();
  bool success = LITTLEFS.begin();
 
  if (success) {
    Serial.println("File system mounted with success");
  } else {
    Serial.println("Error mounting the file system");
    return;
  }
  get_MAC();
  set_WiFi();
  mqtt();
  scanner();
  I2Cone.begin(SDA1,SCL1,400000); // SDA pin 21, SCL pin 22
  // TODO PRINT LOGO neocampus here
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  
  timeClient.begin();
  screen_mode = Date;
  configTime(3600, 3600, ntpServer);
  
}
void loop() {
  // put your main code here, to run repeatedly:
  counter++;
  
  delay(1000);
  if(counter == 1){
    Serial.println();
    delay(500);
      switch(screen_mode){
        case Date:
          printLocalTime();
          /*display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(SSD1306_WHITE);
          display.setCursor(40, 0);
          display.cp437(true);
          display.print(DatePlusTime);
          display.display()
;         */ //Mettre a jour la date et l'afficher
          break;
          
        case Temperature:
          //Mettre a jour la temperature et l'afficher
          Serial.println("wake up MCP9808.... "); // wake up MCP9808 - power consumption ~200 mikro Ampere
          tempSensor.shutdown_wake(1);   // wake up, ready to read!
          // Read and print out the temperature, also shows the resolution mode used for reading.
          Serial.print("Resolution in mode: ");
          Serial.println (tempSensor.getResolution());
          temp = tempSensor.readTempC();
          //float f = tempSensor.readTempF();
          Serial.print("Temp: "); Serial.print(temp); Serial.println("*C"); 
          //Serial.print(f, 4); Serial.println("*F.");
          delay(2000);
          Serial.println("Shutdown MCP9808.... ");
          tempSensor.shutdown_wake(0); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
          Serial.println("");
          delay(200);
        Serial.println(temp);
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(SSD1306_WHITE);
          display.setCursor(40, 0);
          display.cp437(true);
          display.println(F("Temp:"));
          display.print(temp);
          display.println(F(" C"));
          display.display();
          break;
          
        case Luminosity:
          //Mettre a jour la luminosité et l'afficher
          display.clearDisplay();
          Serial.println("About to get the luminosity");
          lux = lumSensor.getLux();
          Serial.println("Got the luminosity");
        Serial.println(lux);
          display.drawLine(0, 32, 43, 32, WHITE);
          display.drawLine(71, 32, 120, 32, WHITE);
          display.setCursor(40,34);
          display.println(F("Lum:"));
          display.print(lux);
          display.println(F(" lux"));
          display.display();
          break;
        default:
          log_error(F("\n[setupLed] unknwown screen_mode ?!?!"));
      }
    }
      
  if (counter == 10) {  
      if(!client.connected()){
        Serial.println(F("Connection lost, reconnecting... "));
        reconnect();
      }   
      boolean rc = client.publish(DEFT_TOPIC_CLASS, "Change"); 
      counter = 0;
  }
  client.loop();
  
  Serial.print(".");
  Serial.flush();
}
