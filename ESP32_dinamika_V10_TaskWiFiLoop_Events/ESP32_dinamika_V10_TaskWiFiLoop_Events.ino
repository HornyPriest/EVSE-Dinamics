#include "EmonLib.h" // Include Emon Library
#include "math.h"
#include "driver/adc.h"
#include "esp32fota.h"
#include "WiFi.h"
#include "PubSubClient.h"
#include "ESPAsyncWebServer.h"            // https://github.com/me-no-dev/ESPAsyncWebServer
// download zip from above->Arduino IDE->menu->tab->sketch->include library->add ZIP library
#include "AsyncTCP.h"                     // https://github.com/me-no-dev/AsyncTCP
// download zip from above->Arduino IDE->menu->tab->sketch->include library->add ZIP library
#include "SPIFFS.h"
#include "ESPmDNS.h"
#include "HTTPClient.h"
#include "ESP32httpUpdate.h"
#include "Preferences.h"
 

// Initiate Preferences to save WiFi credentials to EEPROM
Preferences preferences;
boolean SavedWiFi = LOW;
boolean SavedCalibration = LOW;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//Variables to save values from HTML form
String ssid;
String pass;
String ip;
String gateway;
String subnet;
String mdns;
String dhcpcheck;


//Alternate Service WiFi Credentials
String ssid1 = "Implera";
String pass1 = "AdminSettings";


// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";
const char* subnetPath = "/subnet.txt";
const char* mdnsPath = "/mdns.txt";
const char* dhcpcheckPath = "/dhcpcheck.txt";

//next should become an input field for mdns dot local name in wifimanager
String mdnsdotlocalurl = "Dinamics";    // becomes http://electra.local     give each device a unique name
// const char* mdnsdotlocalurl = "living";  // becomes http://living.local      give each device a unique name
// const char* mdnsdotlocalurl = "kitchen"; // becomes http://kitchen.local     give each device a unique name
// const char* mdnsdotlocalurl = "garage";  // becomes http://garage.local      give each device a unique name
// on android phone use the bonjour browser app to see the .local devices on the network
// https://play.google.com/store/apps/details?id=de.wellenvogel.bonjourbrowser&hl=en&gl=US
// apple does mdns?
// my raspberry pi does mdns!
// windows ?


IPAddress localIP(0, 0, 0, 0);
IPAddress gatewayIP(0, 0, 0, 0);
IPAddress subnetMask(0, 0, 0, 0);



// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

// Set LED GPIO
int ledPin = 5;    // wemos uno sized esp32 board
// Stores LED state

String ledState;

long lastInfo5 = 0;
long lastInfo6 = 0;
long lastInfo7 = 0;
long lastInfo8 = 0;
long lastInfo9 = 0;
long lastInfo10 = 0;
long lastInfo11 = 0;
long lastInfo12 = 0;

long timer = 2000;
long timer1 = 50000;
long timer2 = 10000;
long timer3 = 10000;
long timer4 = 10000;
long timer5 = 50000;
long timer6 = 30000;
long timer7 = 10000;
long timer8 = 50000;
long timer9 = 100000;
long timer10 = 50000;
long timer11 = 50000;
long timer12 = 100000;


long now11;



// Second Core Settings//
TaskHandle_t Task1;

long lastInfo = 0;
long lastInfo1 = 0;
long lastInfo2 = 0;

// Change to your WiFi credentials
//const char *ssid = "Implera01";
//const char *password = "iMplera!";

// esp32fota esp32fota("<Type of Firme for this device>", <this version>);
esp32FOTA esp32FOTA("Dinamics", 3);

int FW_version = 3;


// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "mqshare.napolni.me";

#define S1 5
#define S2 18
#define S3 19
#define S4 21

#define LED_GREEN 12
#define LED_RED 14


#define ct_sensor_1 34 // ESP32 Wroom
#define ct_sensor_2 35
#define ct_sensor_3 32


volatile float Irms_1;
volatile float Irms_2;
volatile float Irms_3;

EnergyMonitor emon1; // Create an instance
EnergyMonitor emon2; // Create an instance
EnergyMonitor emon3; // Create an instance


String deviceName = "ESP32Dinamics-";
const char * chardeviceName;
char charid[23];
String idTopic;
String epochtimeTopic;
String prefix = "Dinamics/";
String fullTopic;
String dynamicTopic;
String BreakerTopic = "/breaker";
String FWversionTopic = "/fwversion";
String I1Topic = "/current1";
String I2Topic = "/current2";
String I3Topic = "/current3";
String Max_ITopic = "/max_charging_current";
String Charge_ITopic = "/charging_current";
String PhasesTopic = "/active_phases";
String EnableTopic = "/enable";

String TimeoutTopic = "/timeout";
String ResponseGCTopic = "/responseGC";
String ResponseG0Topic = "/responseG0";
String ResponseGGTopic = "/responseGG";
String ResponseGSTopic = "/responseGS";
String ResponseGUTopic = "/responseGU";
String ResponseFDTopic = "/responseFD";
String ResponseFETopic = "/responseFE";
String ResponseFSTopic = "/responseFS";
String ResponseSCTopic = "/responseSC";
String ResponseSTTopic = "/responseST";
String ResponseSHTopic = "/responseSH";
String StateTopic = "/state";
String ConnectionTopic = "/connection";
String DebugTopic = "/debug_log";
String SyncTimeTopic = "/synctime";
String StateChangeTopic = "/state_change";


const char * topica;
const char * charBreaker;
String sub_BreakerTopic;
const char * charUpdate;
String sub_UpdateTopic;
const char * charUpdateSpiffs;
String sub_UpdateSpiffsTopic;
const char * charStatus;
String sub_StatusTopic;
const char * charEnable;
String sub_EnableTopic;
const char * charChargeTimer;
String sub_ChargeTimerTopic;
const char * charEnergyLimit;
String sub_EnergyLimitTopic;
const char * charCurrentLimit;
String sub_CurrentLimitTopic;
const char * charCurrentCalibration;
String sub_CurrentCalibrationTopic;
const char * charDebugLVL;
String sub_DebugLVLTopic;
const char * charRAPI;
String sub_RAPITopic;
const char * charTimer;
String sub_TimerTopic;
const char * charTimer1;
String sub_Timer1Topic;
const char * charTimer2;
String sub_Timer2Topic;
const char * charTimer3;
String sub_Timer3Topic;
const char * charTimer4;
String sub_Timer4Topic;
const char * charTimer5;
String sub_Timer5Topic;
const char * charTimer6;
String sub_Timer6Topic;
const char * charTimer7;
String sub_Timer7Topic;
const char * charTimer8;
String sub_Timer8Topic;
const char * charTimer9;
String sub_Timer9Topic;
const char * charTimer10;
String sub_Timer10Topic;
const char * charTimer11;
String sub_Timer11Topic;
const char * charTimer12;
String sub_Timer12Topic;
const char * charDeleteSettings;
String sub_DeleteSettingsTopic;
const char * charPlugAndCharge;
String sub_PlugAndChargeTopic;


float calibration = 27.7;

boolean SS1;
boolean SS2;
boolean SS3;
boolean SS4;

boolean UpdateStart;
boolean UpdateSpiffs;
boolean ResponseStatus;
boolean SetupComplete = LOW;
boolean FWSentFlag = LOW;

float charge_current;
int16_t max_current;
int16_t MQTTmax_current;
int16_t set_current;
uint16_t breaker;
uint8_t active_phases;
String temp_message;
String TempValue;
const char * TempValueChar;
String debug;


String GC = "$GC";
String G0 = "$G0";
String GG = "$GG";
String GS = "$GS";
String GU = "$GU";
String FD = "$FD";
String FE = "$FE";
String FS = "$FS";
String SC = "$SC ";
String ST = "$S3 ";
String SH = "$SH ";
String ResponseMessage;
String ATMessage;


uint8_t State;

boolean SetChargeFlag;
boolean ChargeSetState;
boolean SetMQTTCurrentFlag;
boolean SetCurrentFlag;
boolean SetMQTTbreakerFlag;
boolean ConnectionTimeoutFlag = HIGH;
boolean ActiveTimeoutFlag = LOW;
boolean SetTimerFlag;
boolean SetEnergyLimitFlag;
boolean PhaseInfo;
boolean PAndC;


uint8_t tmp;
uint8_t MQTTBreaker;
uint8_t EnergyLimit;
uint8_t TimeLimit;
uint8_t debug_level;



uint16_t t1;
uint16_t c1 = 0;
uint16_t c2 = 0;
uint16_t c3 = 0;
uint16_t c4 = 0;
uint16_t c5 = 0;
uint16_t c6 = 0;


float average1Old;
float average2Old;
float average3Old;
int16_t max_currentOld;
float charge_currentOld;
uint8_t active_phasesOld;



const int NoRead1 = 10;
float total1 = 0;
float average1 = 0;
unsigned int readindex1 = 0;
float branja1[NoRead1];

const int NoRead2 = 10;
float total2 = 0;
float average2 = 0;
unsigned int readindex2 = 0;
float branja2[NoRead2];

const int NoRead3 = 10;
float total3 = 0;
float average3 = 0;
unsigned int readindex3 = 0;
float branja3[NoRead3];


float LastCurrent1;
float LastCurrent2;
float LastCurrent3;

uint8_t Current1Change;
uint8_t Current2Change;
uint8_t Current3Change;


// NTP server settings
const char* ntpServer = "pool.ntp.org";
unsigned long epochTime;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
long lastMsgRED = 0;
char msg[50];
int value = 0;

unsigned long getTime()
{
  time_t now;
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

//---------------------------------------------------------------------------------------------------------

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while (file.available()) {
    fileContent = file.readStringUntil('\n');
    break;
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
}

// Initialize WiFi
bool initWiFi() {
  if (ssid == "" /*|| ip == ""*/) {  // no ip // made it DHCP
    Serial.println("Undefined SSID wrong wifiroutername or wifirouterpassword");
    return false;
  }

  WiFi.mode(WIFI_STA);

  if (dhcpcheck == "on") {
    // dhcp

  } else {
    // fixed ip
    localIP.fromString(ip.c_str());
    gatewayIP.fromString(gateway.c_str());
    subnetMask.fromString(subnet.c_str());

    if (!WiFi.config(localIP, gatewayIP, subnetMask)) {
      Serial.println("STA Failed to configure");
      return false;
    }
  }

  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while (WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      return false;
    }
  }
  delay(500);
  Serial.println("");
  Serial.println(WiFi.localIP());

  if(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid1.c_str(), pass1.c_str());
    Serial.println("Connecting to Implera Service WiFi...");
  
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
  
    while (WiFi.status() != WL_CONNECTED) {
      currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        Serial.println("Failed to connect.");
        return false;
      }
    }
    delay(500);
    Serial.println("");
    Serial.println(WiFi.localIP());
  }
  

  if (!MDNS.begin(mdnsdotlocalurl.c_str())) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  MDNS.addService("http", "tcp", 80);

  Serial.print("http://");
  Serial.print(mdnsdotlocalurl);
  Serial.println(".lan");
  return true;
}

// Replaces placeholder with LED state value
// replaces the text between %match% in spiffs index.html on upload with actual variables
String processor(const String& var) {
  if (var == "STATE") {                 // in index.html noted as &STATE&
    if (tmp == 2) {
      ledState = "ON";
    }
    else if (tmp == 3){
      ledState = "OFF";
    }
    return ledState;
    return String();
  }
  else if (var == "CURRENT1"){
    String temp1String = String(average1, 1);
    return temp1String;
  }
  else if (var == "CURRENT2"){
    String temp2String = String(average2, 1);
    return temp2String;
  }
  else if (var == "CURRENT3"){
    String temp3String = String(average3, 1);
    return temp3String;
  }
  else if (var == "BREAKERS"){
    String temp4String = String(breaker);
    return temp4String;
  }
  else if (var == "MDNSNAME") {                  // in index.html noted as &MDNSNAME&
    return String(mdnsdotlocalurl);
  } else if (var == "IP") {                      // in index.html noted as &IP&
    return ssid+"<br>"+WiFi.localIP().toString() + " DHCP: " + dhcpcheck ;
  } else if (var == "GATEWAY") {                // in index.html noted as &GATEWAY&
    return WiFi.gatewayIP().toString();
  } else if (var == "SUBNET") {                  // in index.html noted as &SUBNET&
    return WiFi.subnetMask().toString() + "<br>DNS: " + WiFi.dnsIP().toString() + "<br>MAC: " + WiFi.macAddress();
  }

  return String();
}

//---------------------------------------------------------------------------------------------------------

void setup_wifi()
{
  WiFi.mode(WIFI_OFF);
  vTaskDelay(3000);
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_MODE_STA);
  vTaskDelay(3000);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid.c_str(), pass.c_str());
  int i;
  while (WiFi.status() != WL_CONNECTED && i<5) {
    vTaskDelay(500);
    i = i + 1;
    Serial.print(".");
  }
  if(i==5){
    ESP.restart();
  }
  vTaskDelay(3000);
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
  if (String(topic) == sub_BreakerTopic) {
    Serial.print("Set breaker from server ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    MQTTBreaker = temp_message.toInt();
    breaker = MQTTBreaker;
    SENDBreakerAlt();
  }

  if (String(topic) == sub_StatusTopic) {
    Serial.print("Status_inquiry ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    SENDCurrentsAlt();
  }

  if (String(topic) == sub_UpdateTopic) {
    Serial.print("New update JSON link: ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    UpdateStart = HIGH;
  }

  if (String(topic) == sub_UpdateSpiffsTopic) {
    Serial.print("New update SPIFFS link: ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    UpdateSpiffs = HIGH;
  }

  if (String(topic) == sub_EnableTopic) {
    Serial.print("Setting Enable ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    tmp = temp_message.toInt();
    SetChargeFlag = HIGH;
    ChargeChanger();
  }

  if (String(topic) == sub_TimerTopic) {
    Serial.print("Timer received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    TimeLimit = temp_message.toInt();
    SetTimerFlag = HIGH;
    SetTimer();
  }

  if (String(topic) == sub_EnergyLimitTopic) {
    Serial.print("EnergyLimit received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    EnergyLimit = temp_message.toInt();
    SetEnergyLimitFlag = HIGH;
    SetLimit();
  }

  if (String(topic) == sub_CurrentLimitTopic) {
    Serial.print("CurrentLimit received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    MQTTmax_current = temp_message.toInt();
    SetMQTTCurrentFlag = HIGH;
  }

  if (String(topic) == sub_CurrentCalibrationTopic) {
    Serial.print("Current calibration value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    int tempCalibration = temp_message.toInt();
    calibration = tempCalibration/10;
    SetCalibration();
  }

  if (String(topic) == sub_DebugLVLTopic) {
    Serial.print("Debug level received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
  }

  if (String(topic) == sub_RAPITopic) {
    Serial.print("RAPI command received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
  }

  if (String(topic) == sub_TimerTopic) {
    Serial.print("timer value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer1Topic) {
    Serial.print("Counter1 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer1 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer2Topic) {
    Serial.print("Counter2 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer2 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer3Topic) {
    Serial.print("Counter3 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer3 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer4Topic) {
    Serial.print("Counter4 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer4 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer5Topic) {
    Serial.print("Counter5 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer5 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer6Topic) {
    Serial.print("Counter6 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer6 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer7Topic) {
    Serial.print("Counter7 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer7 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer8Topic) {
    Serial.print("Counter8 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer8 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer9Topic) {
    Serial.print("Counter9 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer9 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer10Topic) {
    Serial.print("Counter10 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer10 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer11Topic) {
    Serial.print("Counter11 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer11 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_Timer12Topic) {
    Serial.print("Counter12 value received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    timer12 = temp_message.toDouble();
    SetTimers();
  }

  if (String(topic) == sub_DeleteSettingsTopic) {
    Serial.print("Delete settings received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    int vmesna = temp_message.toInt();
    if(vmesna == 1){
      DeleteSettings();
    }
  }

  if (String(topic) == sub_PlugAndChargeTopic) {
    Serial.print("Plug and Charge setting update received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    int vmesna = temp_message.toInt();
    if(vmesna == 1){
      PAndC = HIGH;
      SetPAC();
    }else{
      PAndC = LOW;
      SetPAC();
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  int i = 0;
  while (!client.connected() && i<10) {
    Serial.print("Attempting MQTT connection...");
    i=i+1;
    // Attempt to connect
    if(i==10){
      WiFiReconnect();
    }
    if (client.connect(chardeviceName, "sharelock" , "biciklekomplikovanije")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(charBreaker);
      client.subscribe(charUpdate);
      client.subscribe(charUpdateSpiffs);
      client.subscribe(charStatus);
      client.subscribe(charEnable);
      client.subscribe(charChargeTimer);
      client.subscribe(charEnergyLimit);
      client.subscribe(charCurrentLimit);
      client.subscribe(charCurrentCalibration);
      client.subscribe(charDebugLVL);
      client.subscribe(charRAPI);
      client.subscribe(charTimer);
      client.subscribe(charTimer1);
      client.subscribe(charTimer2);
      client.subscribe(charTimer3);
      client.subscribe(charTimer4);
      client.subscribe(charTimer5);
      client.subscribe(charTimer6);
      client.subscribe(charTimer7);
      client.subscribe(charTimer8);
      client.subscribe(charTimer9);
      client.subscribe(charTimer10);
      client.subscribe(charTimer11);
      client.subscribe(charTimer12);
      client.subscribe(charPlugAndCharge);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      vTaskDelay(5000);
    }
  }
}

void selectTopic(){
    topica = "";
    dynamicTopic = "";
    epochtimeTopic = getTime();
    dynamicTopic += prefix;
    dynamicTopic += idTopic;
    dynamicTopic += "/";
    dynamicTopic += epochtimeTopic;
}

void CheckWiFiCredentials(){
  preferences.begin("WiFiCred", true);
  String tempSSID = preferences.getString("ssid", "");
  uint8_t SSIDlength = tempSSID.length();
  debug += "$checking credentials in preferences$";
  if(SSIDlength > 1){
    SavedWiFi = HIGH;
    debug += "$found credentials in preferences$";
  }
  preferences.end();
}

void GetWiFiCredentials(){
  if(SavedWiFi == HIGH){
    preferences.begin("WiFiCred", true);
    ssid = preferences.getString("ssid", "");
    pass = preferences.getString("pass", "");
    ip = preferences.getString("ip", "");
    gateway = preferences.getString("gateway", "");
    subnet = preferences.getString("subnet", "");
    mdns = preferences.getString("mdns", "");
    dhcpcheck = preferences.getString("dhcpcheck", "");
    preferences.end();
    debug += "$read credentials from preferences$";
  }
}

void SetWiFiCredentials(){
  preferences.begin("WiFiCred", false);
  preferences.putString("ssid", ssid);
  preferences.putString("pass", pass);
  preferences.putString("ip", ip);
  preferences.putString("gateway", gateway);
  preferences.putString("subnet", subnet);
  preferences.putString("mdns", mdns);
  preferences.putString("dhcpcheck", dhcpcheck);
  preferences.end();
  debug += "$saving credentials to preferences$";
}

void DeleteWiFiCredentials(){
  preferences.begin("WiFiCred", false);
  preferences.clear();
  preferences.end();
  debug += "$deleting credentials from preferences$";
}

void CheckSettings(){
  preferences.begin("Settings", true);
  float tempCAL = preferences.getFloat("calibration", 0);
  debug += "$checking setting in preferences$";
  if(tempCAL > 1){
    SavedCalibration = HIGH;
    debug += "$found setting in preferences$";
  }
  preferences.end();
}

void GetSettings(){
  if(SavedCalibration == HIGH){
    preferences.begin("Settings", true);
    calibration = preferences.getFloat("calibration", 27.7);
    timer = preferences.getLong("timer", 2000);
    timer1 = preferences.getLong("timer1", 50000);
    timer2 = preferences.getLong("timer2", 10000);
    timer3 = preferences.getLong("timer3", 10000);
    timer4 = preferences.getLong("timer4", 10000);
    timer5 = preferences.getLong("timer5", 50000);
    timer6 = preferences.getLong("timer6", 30000);
    timer7 = preferences.getLong("timer7", 10000);
    timer8 = preferences.getLong("timer8", 50000);
    timer9 = preferences.getLong("timer9", 100000);
    timer10 = preferences.getLong("timer10", 50000);
    timer11 = preferences.getLong("timer11", 50000);
    timer12 = preferences.getLong("timer12", 100000);
    PAndC = preferences.getBool("pac", LOW);
    preferences.end();
    debug += "$read settings from preferences$";
  }
}

void SetCalibration(){
  preferences.begin("Settings", false);
  preferences.putFloat("calibration", calibration);
  preferences.end();
  debug += "$saving calibration to preferences$";
  ESP.restart();
}

void SetTimers(){
  preferences.begin("Settings", false);
  preferences.putLong("timer", timer);
  preferences.putLong("timer1", timer1);
  preferences.putLong("timer2", timer2);
  preferences.putLong("timer3", timer3);
  preferences.putLong("timer4", timer4);
  preferences.putLong("timer5", timer5);
  preferences.putLong("timer6", timer6);
  preferences.putLong("timer7", timer7);
  preferences.putLong("timer8", timer8);
  preferences.putLong("timer9", timer9);
  preferences.putLong("timer10", timer10);
  preferences.putLong("timer11", timer11);
  preferences.putLong("timer12", timer12);
  preferences.end();
  debug += "$saving timers to preferences$";
}

void SetPAC(){
  preferences.begin("Settings", false);
  preferences.putBool("pac", PAndC);
  preferences.end();
  debug += "$saving PAC setting to preferences$";
}

void DeleteSettings(){
  preferences.begin("Settings", false);
  preferences.clear();
  preferences.end();
  debug += "$deleting settings from preferences$";
  ESP.restart();
}


void setup() {
  // put your setup code here, to run once:

  xTaskCreatePinnedToCore(
                  Task1code,   /* Task function. */
                  "Task1",     /* name of task. */
                  70000,       /* Stack size of task */
                  NULL,        /* parameter of the task */
                  1,           /* priority of the task */
                  &Task1,      /* Task handle to keep track of created task */
                  0);          /* pin task to core 0 */

  analogReadResolution(ADC_BITS);
  analogSetAttenuation(ADC_6db);
  pinMode(S1, INPUT_PULLDOWN);
  pinMode(S2, INPUT_PULLDOWN);
  pinMode(S3, INPUT_PULLDOWN);
  pinMode(S4, INPUT_PULLDOWN);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Serial.begin(115200);
  
  
  initSPIFFS();

  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, HIGH);


  uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);
  snprintf(charid, 23, "%04X%08X", chip, (uint32_t)chipid);
  //Serial.println(charid);
  delay(200);
  idTopic = charid;
  Serial.println(idTopic);
  deviceName += idTopic;

  chardeviceName = deviceName.c_str();


  sub_BreakerTopic += prefix;
  sub_BreakerTopic += idTopic;
  sub_BreakerTopic += "/set_breaker";
  charBreaker = sub_BreakerTopic.c_str();
  sub_StatusTopic += prefix;
  sub_StatusTopic += idTopic;
  sub_StatusTopic += "/get_status";
  charStatus = sub_StatusTopic.c_str();
  sub_UpdateTopic += prefix;
  sub_UpdateTopic += idTopic;
  sub_UpdateTopic += "/set_update";
  charUpdate = sub_UpdateTopic.c_str();
  sub_UpdateSpiffsTopic += prefix;
  sub_UpdateSpiffsTopic += idTopic;
  sub_UpdateSpiffsTopic += "/set_update_spiffs";
  charUpdateSpiffs = sub_UpdateSpiffsTopic.c_str();
  sub_EnableTopic += prefix;
  sub_EnableTopic += idTopic;
  sub_EnableTopic += "/set_enable";
  charEnable = sub_EnableTopic.c_str();
  sub_ChargeTimerTopic += prefix;
  sub_ChargeTimerTopic += idTopic;
  sub_ChargeTimerTopic += "/set_t_limit";
  charChargeTimer = sub_ChargeTimerTopic.c_str();
  sub_EnergyLimitTopic += prefix;
  sub_EnergyLimitTopic += idTopic;
  sub_EnergyLimitTopic += "/set_e_limit";
  charEnergyLimit = sub_EnergyLimitTopic.c_str();
  sub_CurrentLimitTopic += prefix;
  sub_CurrentLimitTopic += idTopic;
  sub_CurrentLimitTopic += "/set_c_limit";
  charCurrentLimit = sub_CurrentLimitTopic.c_str();
  sub_CurrentCalibrationTopic += prefix;
  sub_CurrentCalibrationTopic += idTopic;
  sub_CurrentCalibrationTopic += "/set_calibration";
  charCurrentCalibration = sub_CurrentCalibrationTopic.c_str();
  sub_DebugLVLTopic += prefix;
  sub_DebugLVLTopic += idTopic;
  sub_DebugLVLTopic += "/set_debug";
  charDebugLVL = sub_DebugLVLTopic.c_str();
  sub_RAPITopic += prefix;
  sub_RAPITopic += idTopic;
  sub_RAPITopic += "/set_rapi";
  charRAPI = sub_RAPITopic.c_str();
  sub_TimerTopic += prefix;
  sub_TimerTopic += idTopic;
  sub_TimerTopic += "/set_timer";
  charTimer = sub_TimerTopic.c_str();
  sub_Timer1Topic += prefix;
  sub_Timer1Topic += idTopic;
  sub_Timer1Topic += "/set_timer1";
  charTimer1 = sub_Timer1Topic.c_str();
  sub_Timer2Topic += prefix;
  sub_Timer2Topic += idTopic;
  sub_Timer2Topic += "/set_timer2";
  charTimer2 = sub_Timer2Topic.c_str();
  sub_Timer3Topic += prefix;
  sub_Timer3Topic += idTopic;
  sub_Timer3Topic += "/set_timer3";
  charTimer3 = sub_Timer3Topic.c_str();
  sub_Timer4Topic += prefix;
  sub_Timer4Topic += idTopic;
  sub_Timer4Topic += "/set_timer4";
  charTimer4 = sub_Timer4Topic.c_str();
  sub_Timer5Topic += prefix;
  sub_Timer5Topic += idTopic;
  sub_Timer5Topic += "/set_timer5";
  charTimer5 = sub_Timer5Topic.c_str();
  sub_Timer6Topic += prefix;
  sub_Timer6Topic += idTopic;
  sub_Timer6Topic += "/set_timer6";
  charTimer6 = sub_Timer6Topic.c_str();
  sub_Timer7Topic += prefix;
  sub_Timer7Topic += idTopic;
  sub_Timer7Topic += "/set_timer7";
  charTimer7 = sub_Timer7Topic.c_str();
  sub_Timer8Topic += prefix;
  sub_Timer8Topic += idTopic;
  sub_Timer8Topic += "/set_timer8";
  charTimer8 = sub_Timer8Topic.c_str();
  sub_Timer9Topic += prefix;
  sub_Timer9Topic += idTopic;
  sub_Timer9Topic += "/set_timer9";
  charTimer9 = sub_Timer9Topic.c_str();
  sub_Timer10Topic += prefix;
  sub_Timer10Topic += idTopic;
  sub_Timer10Topic += "/set_timer10";
  charTimer10 = sub_Timer10Topic.c_str();
  sub_Timer11Topic += prefix;
  sub_Timer11Topic += idTopic;
  sub_Timer11Topic += "/set_timer11";
  charTimer11 = sub_Timer11Topic.c_str();
  sub_Timer12Topic += prefix;
  sub_Timer12Topic += idTopic;
  sub_Timer12Topic += "/set_timer12";
  charTimer12 = sub_Timer12Topic.c_str();
  sub_PlugAndChargeTopic += prefix;
  sub_PlugAndChargeTopic += idTopic;
  sub_PlugAndChargeTopic += "/set_plugandcharge";
  charPlugAndCharge = sub_PlugAndChargeTopic.c_str();



  CheckWiFiCredentials();
  if(SavedWiFi == LOW){
    // Load values saved in SPIFFS
    ssid = readFile(SPIFFS, ssidPath);
    Serial.println(ssid);
    debug += "$";
    debug += "ssid=";
    debug += ssid;
    debug += "$";
    pass = readFile(SPIFFS, passPath);
    Serial.println(pass);
    debug += "$";
    debug += "pass=";
    debug += pass;
    debug += "$";
    ip = readFile(SPIFFS, ipPath);
    Serial.println(ip);
    debug += "$";
    debug += "ip=";
    debug += ip;
    debug += "$";
    gateway = readFile(SPIFFS, gatewayPath);
    Serial.println(gateway);
    debug += "$";
    debug += "gateway=";
    debug += gateway;
    debug += "$";
    subnet = readFile(SPIFFS, subnetPath);
    Serial.println(subnet);
    debug += "$";
    debug += "subnet=";
    debug += subnet;
    debug += "$";
    mdnsdotlocalurl = readFile(SPIFFS, mdnsPath);
    Serial.println(mdnsdotlocalurl);
    debug += "$";
    debug += "mdnsdotlocalurl=";
    debug += mdnsdotlocalurl;
    debug += "$";
    dhcpcheck = readFile(SPIFFS, dhcpcheckPath);
    Serial.println(dhcpcheck);
    debug += "$";
    debug += "dhcpcheck=";
    debug += dhcpcheck;
    debug += "$";
  }else{
    GetWiFiCredentials();
  }


  CheckSettings();
  GetSettings();


  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  

  // Set GPIO ledPin as an OUTPUT
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);



      if (initWiFi()) {
      Serial.println("InitWiFi = HIGH");
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SPIFFS, "/index.html", "text/html", false, processor);
    });
    server.serveStatic("/", SPIFFS, "/");

    // Route to set GPIO state to HIGH
    server.on("/on", HTTP_GET, [](AsyncWebServerRequest * request) {
      SetChargeFlag = HIGH;
      tmp = 2;
      request->send(SPIFFS, "/index.html", "text/html", false, processor);
    });

    // Route to set GPIO state to LOW
    server.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
      SetChargeFlag = HIGH;
      tmp = 3;
      request->send(SPIFFS, "/index.html", "text/html", false, processor);
    });

    //  /status returns text 0 ro 1 for remote monitoring
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest * request) {
      int readval = tmp;
      request->send(200, "text/txt", String(readval));
    });

    //  /status returns text 0 ro 1 for remote monitoring
    server.on("/status1", HTTP_GET, [](AsyncWebServerRequest * request) {
      float readval1 = average1;
      request->send(200, "text/txt", String(readval1, 1));
    });

    //  /status returns text 0 ro 1 for remote monitoring
    server.on("/status2", HTTP_GET, [](AsyncWebServerRequest * request) {
      float readval2 = average2;
      request->send(200, "text/txt", String(readval2, 1));
    });

    //  /status returns text 0 ro 1 for remote monitoring
    server.on("/status3", HTTP_GET, [](AsyncWebServerRequest * request) {
      float readval3 = average3;
      request->send(200, "text/txt", String(readval3, 1));
    });

    //  /status returns text 0 ro 1 for remote monitoring
    server.on("/status4", HTTP_GET, [](AsyncWebServerRequest * request) {
      int readval4 = breaker;
      request->send(200, "text/txt", String(readval4));
    });


    //  /resetwifitoap
    server.on("/resetwifitoap", HTTP_GET, [](AsyncWebServerRequest * request) {
      SPIFFS.remove("/ssid.txt");
      SPIFFS.remove("/pass.txt");
      DeleteWiFiCredentials();
      request->send(200, "text/html", "<h1>deleted wifi credentials ssid.txt and pass.txt<br>Done.<br>ESP restart,<br>connect to AP access point ESP WIFI MANAGER <br>to configure wifi settings again<br><a href=\"http://192.168.4.1\">http://192.168.4.1</a></h1>");
      delay(5000);
      ESP.restart();
    });

    //  /reboot
    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(200, "text/html", "<h1>Huh, Reboot Electra, Restart ESP32<br><a href=\"http://" + WiFi.localIP().toString()  + "\">http://" + WiFi.localIP().toString() + "</a></h1>");
      delay(5000);
      ESP.restart();
    });

    server.on("/timer", HTTP_POST, [](AsyncWebServerRequest * request) {
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
//        if (p->isPost()) {
//          // HTTP POST ssid value
//          const char* PARAM_INPUT_20 = "off";                  // Search for parameter in HTTP POST request
//          if (p->name() == PARAM_INPUT_20) {
//            offdelay = p->value().toInt();
//            Serial.print("offdelay set to: ");
//            Serial.println(offdelay);
//            // Write file to save value
//            writeFile(SPIFFS, offdelayPath, offdelay.c_str());
//            offdelayint = offdelay.toInt();
//            Serial.println(offdelayint);
//          }
//        }
      }
      request->send(SPIFFS, "/index.html", "text/html", false, processor);
    });


    server.on("/list", HTTP_GET, [](AsyncWebServerRequest * request) {    // /list files in spiffs on webpage
      if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        debug += "$";
        debug += "An Error has occurred while mounting SPIFFS";
        debug += "$";
        return;
      }

      File root = SPIFFS.open("/");
      File file = root.openNextFile();
      String str = "";
      while (file) {
        str += " / ";
        str += file.name();
        str += "\r\n";
        file = root.openNextFile();
      }
      str += "\r\n";
      str += "\r\n";
      str += "totalBytes   ";
      str += SPIFFS.totalBytes();
      str += "\r\n";
      str += "usedBytes    ";
      str += SPIFFS.usedBytes();
      str += "\r\n";
      str += "freeBytes??? ";
      str += SPIFFS.totalBytes()-SPIFFS.usedBytes();
      str += "\r\n";
      request->send(200, "text/txt", str);
    });

    server.begin();
  }
  else {
    // Connect to Wi-Fi network with SSID and password == setup an AP AccessPoint for wifi direct connect
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    String broadcastintheair = String("Dinamics-") + charid;  // want a unique broadcast id for each device
    WiFi.softAP(broadcastintheair.c_str(), NULL);                                        // i do not know, strings and chars thing drive me nuts
                                                                                         // i have seen all errors possible, getting this working ;-)

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    debug += "$";
    debug += "IP address=";
    debug += IP;
    debug += "$";
    

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(SPIFFS, "/wifimanager.html", "text/html");
    });

    server.serveStatic("/", SPIFFS, "/");

    server.on("/", HTTP_POST, [](AsyncWebServerRequest * request) {
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          // HTTP POST ssid value
          const char* PARAM_INPUT_1 = "ssid";                  // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
//            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          const char* PARAM_INPUT_2 = "pass";                 // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
//            writeFile(SPIFFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          const char* PARAM_INPUT_3 = "ip";                   // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_3) {
            dhcpcheck = "off";
            writeFile(SPIFFS, dhcpcheckPath, "off");          //dhcp unchecked . if we recieve post with ip set dhcpcheck.txt file to off
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
            Serial.println(ip);
//            writeFile(SPIFFS, ipPath, ip.c_str());            // Write file to save value
          }
          // HTTP POST gateway value
          const char* PARAM_INPUT_4 = "gateway";              // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
            Serial.print("gateway Address set to: ");
            Serial.println(gateway);
//            writeFile(SPIFFS, gatewayPath, gateway.c_str());          // Write file to save value
          }

          // HTTP POST subnet value
          const char* PARAM_INPUT_5 = "subnet";               // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_5) {
            subnet = p->value().c_str();
            Serial.print("subnet Address set to: ");
            Serial.println(subnet);
//            writeFile(SPIFFS, subnetPath, subnet.c_str());            // Write file to save value
          }
          // HTTP POST mdns value
          const char* PARAM_INPUT_6 = "mdns";                 // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_6) {
            mdnsdotlocalurl = p->value().c_str();
            Serial.print("mdnsdotlocalurl Address set to: ");
            Serial.println(mdnsdotlocalurl);
//            writeFile(SPIFFS, mdnsPath, mdnsdotlocalurl.c_str());            // Write file to save value
          }
          // HTTP POST dhcp value on
          const char* PARAM_INPUT_7 = "dhcp";                // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_7) {
            dhcpcheck = p->value().c_str();
            Serial.print("dhcpcheck set to: ");
            Serial.println(dhcpcheck);
//            writeFile(SPIFFS, dhcpcheckPath, dhcpcheck.c_str());            // Write file to save value
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      if (dhcpcheck == "on") {
        ip = "dhcp ip adress";
      }
      SetWiFiCredentials();
      request->send(200, "text/html", "<h1>Done. ESP restart,<br> connect router <br>go to: <a href=\"http://" + ip + "\">" + ip + "</a><br><a href=\"http://" + mdnsdotlocalurl + ".lan\">http://" + mdnsdotlocalurl + ".lan</a></h1>");
      delay(5000);
      ESP.restart();
    });
    server.begin();
  }


  
  delay(200);
  client.setServer(mqtt_server, 31883);
  client.setCallback(callback);
  esp32FOTA.checkURL = "http://lockit.pro/ota/Dinamics/Dinamics.json";


  //init and get the time
  configTime(0, 0, ntpServer);


//  if (!client.connected()) {
//    if (!WiFi.status() == WL_CONNECTED){
//      setup_wifi();
//    }
//    reconnect();
//  }
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);

  delay(500);
  
  DIPswitchCalc();
//  SENDBreaker();
//  SENDFWversion();

  SendDebug();

  Serial.println(FW_version);
  
  SetupComplete = HIGH;
}

void loop() {
  // put your main code here, to run repeatedly:
  SENDFWversion();


  if (!client.connected()) {
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, LOW);
      if (!WiFi.status() == WL_CONNECTED){      
        WiFiReconnect();
  //        ESP.restart();  // test, da mi ne zginjajo
      }else{
        reconnect();
      }
  }else{
      digitalWrite(LED_RED, LOW);
  }

  
  if(UpdateStart == HIGH){
    if((WiFi.status() == WL_CONNECTED)) {
      boolean updatedNeeded = esp32FOTA.execHTTPcheck();
      if(updatedNeeded == HIGH){
        Serial.print("najden update");
        esp32FOTA.execOTA();
      }else{
        UpdateStart = LOW;
      }
    }
  }

  if((WiFi.status() == WL_CONNECTED)){
    if(UpdateSpiffs == HIGH) {
        Serial.println("Update SPIFFS...");
        debug += "$";
        debug += "SPIFFS update select";
        debug += "$";
        t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs("http://lockit.pro/ota/Dinamics/spiffs.bin");
        if(ret == HTTP_UPDATE_OK) {
          Serial.println("Update succesful");
          debug += "$";
          debug += "SPIFFS update succesful";
          debug += "$";
          UpdateSpiffs = LOW;
        }else{
          UpdateSpiffs = LOW;
        }
        switch(ret) {
                case HTTP_UPDATE_FAILED:
                    Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());         
                    debug += "$";
                    debug += "HTTP_UPDATE_FAILED Error ";
                    debug += ESPhttpUpdate.getLastError();
                    debug += " : ";
                    debug += ESPhttpUpdate.getLastErrorString();
                    debug += "$";
                    break;

                case HTTP_UPDATE_NO_UPDATES:
                    Serial.println("HTTP_UPDATE_NO_UPDATES");
                    debug += "$";
                    debug += "HTTP_UPDATE_NO_UPDATES";
                    debug += "$";
                    break;

                case HTTP_UPDATE_OK:
                    Serial.println("HTTP_UPDATE_OK");
                    debug += "$";
                    debug += "HTTP_UPDATE_OK";
                    debug += "$";
                    break;
            }
    }else{
      if(!SPIFFS.exists("/wifimanager.html")){
        UpdateSpiffs = HIGH;
      }
    }
  }


  total1 = total1 - branja1[readindex1];
  branja1[readindex1] = Irms_1;
  total1 = total1 + branja1[readindex1];
  readindex1 = readindex1 + 1;

  if (readindex1 >= NoRead1){
    readindex1 = 0;
  }

  average1 = total1 / NoRead1;

  total2 = total2 - branja2[readindex2];
  branja2[readindex2] = Irms_2;
  total2 = total2 + branja2[readindex2];
  readindex2 = readindex2 + 1;

  if (readindex2 >= NoRead2){
    readindex2 = 0;
  }

  average2 = total2 / NoRead2;

  total3 = total3 - branja3[readindex3];
  branja3[readindex3] = Irms_3;
  total3 = total3 + branja3[readindex3];
  readindex3 = readindex3 + 1;

  if (readindex3 >= NoRead3){
    readindex3 = 0;
  }

  average3 = total3 / NoRead3;


  Dovoljen_Tok();
  ChargeChanger();
  SetMQTTCurrent();
  CurrentFlagSet();
  SetCurrent();
//  client.loop();
  StopCharge();
  SendDebug();

  CatchStateChange();

  
    long now = millis();
    if (now - lastInfo > timer) {   //2000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo = now;
      SENDCurrents();
      digitalWrite(LED_GREEN, LOW);
    }
    long now5 = millis();
    if (now5 - lastInfo5 > timer5) {    // 50000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo5 = now5;
      CheckSetAmps();
      digitalWrite(LED_GREEN, LOW);
    }
    long now6 = millis();
    if (now6 - lastInfo6 > timer6) {     //30000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo6 = now6;
      CheckState();
      digitalWrite(LED_GREEN, LOW);
    } 
    long now7 = millis();
    if (now7 - lastInfo7 > timer7) {      //10000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo7 = now7;
      CheckCharge();
      digitalWrite(LED_GREEN, LOW);
    }
    long now8 = millis();
    if (now8 - lastInfo8 > timer8) {     //50000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo8 = now8;
      CheckStatus();
      digitalWrite(LED_GREEN, LOW);
    } 
    long now9 = millis();
    if (now9 - lastInfo9 > timer9) {    //100000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo9 = now9;
      CheckEnergy();
      digitalWrite(LED_GREEN, LOW);
    }
    long now10 = millis();
    if (now10 - lastInfo10 > timer10) {   //50000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo10 = now10;
      ConnectionAlert();
      digitalWrite(LED_GREEN, LOW);
    }    
    long now12 = millis();
    if (now12 - lastInfo12 > timer12) {   //100000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo12 = now12;
      SENDSyncClock();
      digitalWrite(LED_GREEN, LOW);
    } 
 client.loop();
 vTaskDelay(300);
}


void Dovoljen_Tok(){
  
  if(active_phases != 1 && active_phases != 2 && active_phases != 13){  
    if((average1 >= average2)&&(average1 >= average3)){
      max_current = (((breaker) - (average1) + (charge_current))/**0.98*/);
    }else if((average2 > average1)&&(average2 >= average3)){
      max_current = (((breaker) - (average2) + (charge_current))/**0.98*/);
    }else if((average3 > average1)&&(average3 > average2)){
      max_current = (((breaker) - (average3) + (charge_current))/**0.98*/);
    }
  }
  if(active_phases == 1){
    max_current = (((breaker) - (average1) + (charge_current))/**0.98*/);
  }
  if(active_phases == 2){
    if(average1 >= average2){
      max_current = (((breaker) - (average1) + (charge_current))/**0.98*/);
    }else if(average2 > average1){
      max_current = (((breaker) - (average2) + (charge_current))/**0.98*/);
    }
  }
  if(active_phases == 13){
    if(average1 >= average3){
      max_current = (((breaker) - (average1) + (charge_current))/**0.98*/);
    }else if(average3 > average1){
      max_current = (((breaker) - (average3) + (charge_current))/**0.98*/);
    }
  }
  if(max_current < 0){
    max_current = 0;
  }
}


void DIPswitchCalc(){
  SS1 = digitalRead(S1);
  SS2 = digitalRead(S2);
  SS3 = digitalRead(S3);
  SS4 = digitalRead(S4);

  if(SS1 == 0){
    if(SS2 == 0){
      if(SS3 == 0){
        if(SS4 == 0){
          breaker = 6;
        }else{
          breaker = 35;
        }
      }else{
        if(SS4 == 0){
          breaker = 19;
        }
      }
    }else{
      if(SS3 == 0){
        if(SS4 == 0){
          breaker = 12;
        }
      }else{
        if(SS4 == 0){
          breaker = 29;
        }
      }
    }
  }else{
    if(SS2 == 0){
      if(SS3 == 0){
        if(SS4 == 0){
          breaker = 10;
        }else{
          breaker = 45;
        }
      }else{
        if(SS4 == 0){
          breaker = 24;
        }
      }
    }else{
      if(SS3 == 0){
        if(SS4 == 0){
          breaker = 15;
        }
      }else{
        if(SS4 == 0){
          breaker = 32;
        }
      }
    }
  }
  Serial.println(breaker);
  debug += "$";
  debug += "DIP switch is set to = ";
  debug += breaker;
  debug += "$";
}

void SENDBreaker(){
  if (client.connected()){
    
    debug += "$";
    debug += "Sending breakers";
    debug += "$";
    
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
    topica = "";
    dynamicTopic = "";
  //  epochtimeTopic = getTime();
    dynamicTopic += prefix;
    dynamicTopic += idTopic;
  //  dynamicTopic += "/";
  //  dynamicTopic += epochtimeTopic;
    fullTopic = dynamicTopic;
    fullTopic += BreakerTopic;
    topica = fullTopic.c_str();
    TempValue = "";
    TempValue += breaker;
    TempValueChar = TempValue.c_str();
    client.publish(topica, TempValueChar, true);
//    delay(20);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
  }
}

void SENDBreakerAlt(){
  if (client.connected()){
    
    debug += "$";
    debug += "Sending breakers Alternate";
    debug += "$";
    
    topica = "";
    dynamicTopic = "";
  //  epochtimeTopic = getTime();
    dynamicTopic += prefix;
    dynamicTopic += idTopic;
  //  dynamicTopic += "/";
  //  dynamicTopic += epochtimeTopic;
    fullTopic = dynamicTopic;
    fullTopic += BreakerTopic;
    topica = fullTopic.c_str();
    TempValue = "";
    TempValue += breaker;
    TempValueChar = TempValue.c_str();
    client.publish(topica, TempValueChar, true);
//    delay(20);
  }
}

void SENDFWversion(){
  if (client.connected() && FWSentFlag == LOW){

    debug += "$";
    debug += "Sending FW version";
    debug += "$";
    
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
    topica = "";
    dynamicTopic = "";
    epochtimeTopic = getTime();
    dynamicTopic += prefix;
    dynamicTopic += idTopic;
    dynamicTopic += "/";
    dynamicTopic += epochtimeTopic;
    fullTopic = dynamicTopic;
    fullTopic += FWversionTopic;
    topica = fullTopic.c_str();
    TempValue = "";
    TempValue += FW_version;
    TempValueChar = TempValue.c_str();
    client.publish(topica, TempValueChar);
//    delay(20);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    FWSentFlag = HIGH;
  }
}

void SENDCurrents(){
  if (client.connected()){    
    float tempaverage = average1Old - average1;
    tempaverage = abs(tempaverage);
    if(tempaverage >= 0.15){
      debug += "$";
      debug += "Sending Current 1";
      debug += "$";
      topica = "";
      dynamicTopic = "";
    //  epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += I1Topic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += average1;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      average1Old = average1;
//      delay(20);
    }

    tempaverage = average2Old - average2;
    tempaverage = abs(tempaverage);
    if(tempaverage >= 0.15){
      debug += "$";
      debug += "Sending Current 2";
      debug += "$";
    //  dynamicTopic = prefix;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += I2Topic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += average2;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      average2Old = average2;
  //    delay(20);
    }

    tempaverage = average3Old - average3;
    tempaverage = abs(tempaverage);
    if(tempaverage >= 0.15){
      debug += "$";
      debug += "Sending Current 3";
      debug += "$";
    //  dynamicTopic = prefix;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += I3Topic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += average3;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      average3Old = average3;
  //    delay(20);
    }

    int tempmax_current = max_currentOld - max_current;
    tempmax_current = abs(tempmax_current);
    if(tempmax_current >= 0.15){
      debug += "$";
      debug += "Sending Max current";
      debug += "$";
    //  dynamicTopic = prefix;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += Max_ITopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += max_current;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      max_currentOld = max_current;
  //    delay(20);
    }


    float tempcharge_current = charge_currentOld - charge_current;
    tempcharge_current = abs(tempcharge_current);
    if(tempcharge_current >= 0.15){
      debug += "$";
      debug += "Sending Charge Current";
      debug += "$";
    //  dynamicTopic = prefix;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += Charge_ITopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += charge_current;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      charge_currentOld = charge_current;
  //    delay(20);
    }


    if(active_phasesOld != active_phases){
      debug += "$";
      debug += "Sending Active Phases";
      debug += "$";
    //  dynamicTopic = prefix;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += PhasesTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += active_phases;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      active_phasesOld = active_phases;
  //    delay(20);
    }
  }
}


void SENDSyncClock(){
  if (client.connected()){
    debug += "$";
    debug += "Sending Clock Sync Message";
    debug += "$";

    topica = "";
    dynamicTopic = "";
    epochtimeTopic = getTime();
    dynamicTopic += prefix;
    dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
    fullTopic = dynamicTopic;
    fullTopic += SyncTimeTopic;
    topica = fullTopic.c_str();
    TempValue = "";
    TempValue += epochtimeTopic;
    TempValueChar = TempValue.c_str();
    client.publish(topica, TempValueChar);
  }
}

void SENDCurrentsAlt(){
  if (client.connected()){

      debug += "$";
      debug += "Sending Currents Alternate";
      debug += "$";
      

      topica = "";
      dynamicTopic = "";
    //  epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += I1Topic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += average1;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      average1Old = average1;



    //  dynamicTopic = prefix;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += I2Topic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += average2;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      average2Old = average2;



    //  dynamicTopic = prefix;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += I3Topic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += average3;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      average3Old = average3;



    //  dynamicTopic = prefix;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += Max_ITopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += max_current;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      max_currentOld = max_current;


    //  dynamicTopic = prefix;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += Charge_ITopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += charge_current;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      charge_currentOld = charge_current;



    //  dynamicTopic = prefix;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += PhasesTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += active_phases;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      active_phasesOld = active_phases;
  }
}


void ConnectionAlert(){
  if((ConnectionTimeoutFlag == HIGH) && (ActiveTimeoutFlag == LOW)){
    ActiveTimeoutFlag = HIGH;
    debug += "$";
    debug += "Connection Alert No Connection";
    debug += "$";
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //   epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += ConnectionTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue = "CON_ERROR";
      TempValueChar = TempValue.c_str();    
      client.publish(topica, TempValueChar, true);
//      delay(20);
    }
  }
  if((ConnectionTimeoutFlag == LOW) && (ActiveTimeoutFlag == HIGH)){
    ActiveTimeoutFlag = LOW;
    debug += "$";
    debug += "Connection Alert Connection OK";
    debug += "$";
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //   epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += ConnectionTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue = "OK";
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar, true);
//      delay(20);
    }
  }
}

void CheckState(){
  if(ConnectionTimeoutFlag == LOW){
    ResponseStatus = LOW;
    debug += "$";
    debug += "Checking State RAPI = G0";
    debug += "$";
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(G0);
    while(ResponseStatus == LOW && t1 < 8000){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(50);
    }
    if(t1 > 10){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
//        epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
//        dynamicTopic += "/";
//        dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += TimeoutTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += t1;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
//        delay(20);
      }
    }
    if(t1 > 7999){
        ConnectionTimeoutFlag = HIGH;
        if (client.connected()){
          fullTopic = dynamicTopic;
          fullTopic += StateTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += "2";
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar);
//          delay(20);
        }
      }else{
        ConnectionTimeoutFlag = LOW;
        if (client.connected()){
          topica = "";
          dynamicTopic = "";
      //    epochtimeTopic = getTime();
          dynamicTopic += prefix;
          dynamicTopic += idTopic;
      //    dynamicTopic += "/";
      //    dynamicTopic += epochtimeTopic;
          fullTopic = dynamicTopic;
          fullTopic += ResponseG0Topic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += ResponseMessage;
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar);
//          delay(20);
        }
    
        int index = ResponseMessage.indexOf(" ");
        ResponseMessage.remove(0, index+1);
        ResponseMessage.remove(1);
        State = ResponseMessage.toInt();

        if (client.connected()){
          fullTopic = dynamicTopic;
          fullTopic += StateTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += State;
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar, true);
//          delay(20);
        }
      }
      t1 = 0;
  }
}

void CheckStatus(){
  if(ConnectionTimeoutFlag == LOW){
    debug += "$";
    debug += "Checking Status RAPI = GS";
    debug += "$";
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(GS);
    while(ResponseStatus == LOW && t1 < 8000){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(50);
    }
    if(t1 > 10){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
//        epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
//        dynamicTopic += "/";
//        dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += TimeoutTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += t1;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
//        delay(20);
      }
    }
    if(t1 > 7999){
        ConnectionTimeoutFlag = HIGH;
      }else{
        ConnectionTimeoutFlag = LOW;
    }
    t1 = 0;
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //    epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += ResponseGSTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += ResponseMessage;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
//      delay(20);
    }
  }
}


void CheckSetAmps(){
  if(ConnectionTimeoutFlag == LOW){
    debug += "$";
    debug += "Checking Set AMPS RAPI = GC";
    debug += "$";
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(GC);
    while(ResponseStatus == LOW && t1 < 8000){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(50);
    }
    if(t1 > 10){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
//        epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
//        dynamicTopic += "/";
//        dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += TimeoutTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += t1;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
//        delay(20);
      }
    }
    if(t1 > 7999){
        ConnectionTimeoutFlag = HIGH;
      }else{
        ConnectionTimeoutFlag = LOW;
    }
    t1 = 0;
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //    epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += ResponseGCTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += ResponseMessage;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
//      delay(20);
    }
  }
}

void CheckCharge(){
//  if(ConnectionTimeoutFlag == LOW){
    ResponseStatus = LOW;
    debug += "$";
    debug += "Checking Charge RAPI = GG";
    debug += "$";
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(GG);
    while(ResponseStatus == LOW && t1 < 8000){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(50);
    }
    if(t1 > 10){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
//        epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
//        dynamicTopic += "/";
//        dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += TimeoutTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += t1;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
//        delay(20);
      }
    }
    if(t1 > 7999){
        ConnectionTimeoutFlag = HIGH;
      }else{
        ConnectionTimeoutFlag = LOW;
    }
    t1 = 0;
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //    epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += ResponseGGTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += ResponseMessage;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
//      delay(20);
    }


    int index = ResponseMessage.indexOf(" ");
    ResponseMessage.remove(0, index+1);
    index = ResponseMessage.indexOf(" ");
    ResponseMessage.remove(index);
    uint32_t charge_current_mA = ResponseMessage.toInt();
    charge_current = charge_current_mA / 1000.0;

//    Serial.print("C1 je :");
//    Serial.println(c1);
    if((PhaseInfo == LOW) && (c1 > 2)){
//      Serial.println("Phase = LOW in c1 > 2");
      if(charge_current > 5){
        CheckPhaseChange();
      }
      if(c1 == 3){
        c1 = 0;
      }
    }
    c1 = c1+1;
    if(c1 == 4){
        c1 = 0;
    }
//  }
}

void CheckEnergy(){
  if(ConnectionTimeoutFlag == LOW){
    ResponseStatus = LOW;
    debug += "$";
    debug += "Checking Energy RAPI = GU";
    debug += "$";
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(GU);
    while(ResponseStatus == LOW && t1 < 8000){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(50);
    }
    if(t1 > 10){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
//        epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
//        dynamicTopic += "/";
//        dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += TimeoutTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += t1;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
//        delay(20);
      }
    }
    if(t1 > 7999){
        ConnectionTimeoutFlag = HIGH;
      }else{
        ConnectionTimeoutFlag = LOW;
    }
    t1 = 0;
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //    epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += ResponseGUTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += ResponseMessage;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
//      delay(20);
    }
  }
}


void TurnOn(){
  if(ConnectionTimeoutFlag == LOW){
    debug += "$";
    debug += "Setting enable --> PowerOn ";
    debug += "$";
    ChargeSetState = HIGH;
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    tmp = 2;
    Serial.println(FE);
    SaveLastCurrents();
    while(ResponseStatus == LOW && t1 < 8000){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
        ATMessage = ResponseMessage;
      }
      t1 = t1 + 1;
      delayMicroseconds(50);
    }
    if(t1 > 10){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
//        epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
//        dynamicTopic += "/";
//        dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += TimeoutTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += t1;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
//        delay(20);
      }
    }
    if(t1 > 7999){
        ConnectionTimeoutFlag = HIGH;
    }else{
        ConnectionTimeoutFlag = LOW;
//        tmp = 2;    PRESTAVIL GOR, RAZMISLI IN DAJ NAZAJ TU IN ZGORAJ ZBRIŠI
        if (client.connected()){
          topica = "";
          dynamicTopic = "";
      //    epochtimeTopic = getTime();
          dynamicTopic += prefix;
          dynamicTopic += idTopic;
      //    dynamicTopic += "/";
      //    dynamicTopic += epochtimeTopic;
          fullTopic = dynamicTopic;
          fullTopic += EnableTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue = "2";
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar, true);
//          delay(20);
        }
        PhaseInfo = LOW;
        c1 = 0;
    }
    t1 = 0;
    
/*    int index = ATMessage.indexOf("$AT");
    if(index > 1){
      ATMessage.remove(0, index);
      ResponseMessage.remove(index);
    }*/
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //    epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += ResponseFETopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += ResponseMessage;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
//      delay(20);
    }
/*    if(index > 1){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
    //    epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
    //    dynamicTopic += "/";
    //    dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += StateChangeTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += ATMessage;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
  //      delay(20);
      }
    }*/
  }
}
void TurnOff(){
  if(ConnectionTimeoutFlag == LOW){
    debug += "$";
    debug += "Setting enable --> PowerOff ";
    debug += "$";
    ChargeSetState = LOW;
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(FD);
    while(ResponseStatus == LOW && t1 < 8000){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
        ATMessage = ResponseMessage;
      }
      t1 = t1 + 1;
      delayMicroseconds(50);
    }
    if(t1 > 10){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
//        epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
//        dynamicTopic += "/";
//        dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += TimeoutTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += t1;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
//        delay(20);
      }
    }
    if(t1 > 7999){
        ConnectionTimeoutFlag = HIGH;
      }else{
        ConnectionTimeoutFlag = LOW;
        if (client.connected()){
          topica = "";
          dynamicTopic = "";
      //    epochtimeTopic = getTime();
          dynamicTopic += prefix;
          dynamicTopic += idTopic;
      //    dynamicTopic += "/";
      //    dynamicTopic += epochtimeTopic;
          fullTopic = dynamicTopic;
          fullTopic += EnableTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue = "1";
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar, true);
//          delay(20);
        }
        active_phases = 0;
    }
    t1 = 0;

/*    int index = ATMessage.indexOf("$AT");
    if(index > 1){
      ATMessage.remove(0, index);
      ResponseMessage.remove(index);
    }*/
    
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //    epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += ResponseFDTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += ResponseMessage;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
//      delay(20);
    }
 /*   if(index > 1){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
    //    epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
    //    dynamicTopic += "/";
    //    dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += StateChangeTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += ATMessage;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
  //      delay(20);
      }
    }*/
  }
}
void TurnSleep(){
  if(ConnectionTimeoutFlag == LOW){
    debug += "$";
    debug += "Setting enable --> Sleep ";
    debug += "$";
    ChargeSetState = LOW;
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    tmp = 3;
    Serial.println(FS);
    while(ResponseStatus == LOW && t1 < 8000){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
        ATMessage = ResponseMessage;
      }
      t1 = t1 + 1;
      delayMicroseconds(50);
    }
    if(t1 > 10){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
//        epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
//        dynamicTopic += "/";
//        dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += TimeoutTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += t1;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
//        delay(20);
      }
    }
    if(t1 > 7999){
        ConnectionTimeoutFlag = HIGH;
      }else{
        ConnectionTimeoutFlag = LOW;
        tmp = 3;
        if (client.connected()){
          topica = "";
          dynamicTopic = "";
      //    epochtimeTopic = getTime();
          dynamicTopic += prefix;
          dynamicTopic += idTopic;
      //    dynamicTopic += "/";
      //    dynamicTopic += epochtimeTopic;
          fullTopic = dynamicTopic;
          fullTopic += EnableTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue = "3";
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar, true);
//          delay(20);
        }
        active_phases = 0;
    }
    t1 = 0;

/*    int index = ATMessage.indexOf("$AT");
    if(index > 1){
      ATMessage.remove(0, index);
      ResponseMessage.remove(index);
    }*/
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //    epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += ResponseFSTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += ResponseMessage;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
//      delay(20);
    }
 /*   if(index > 1){
      if (client.connected()){
        topica = "";
        dynamicTopic = "";
    //    epochtimeTopic = getTime();
        dynamicTopic += prefix;
        dynamicTopic += idTopic;
    //    dynamicTopic += "/";
    //    dynamicTopic += epochtimeTopic;
        fullTopic = dynamicTopic;
        fullTopic += StateChangeTopic;
        topica = fullTopic.c_str();
        TempValue = "";
        TempValue += ATMessage;
        TempValueChar = TempValue.c_str();
        client.publish(topica, TempValueChar);
  //      delay(20);
      }
    }*/
  }
}


void ChargeChanger(){
  if(ConnectionTimeoutFlag == LOW){
    if(SetChargeFlag == HIGH){
      debug += "$";
      debug += "Changing enable/charging state to : ";
      debug += tmp;
      debug += "$";
      switch(tmp){
        case 1:
          TurnOff();
        break;
        case 2:
          c2 = 0;
          c3 = 0;
          c4 = 0;
          c5 = 0; 
          TurnOn();
        break;
        case 3:
          TurnSleep();
        break;
      }
      SetChargeFlag = LOW;
    }
  }
}


void SetMQTTCurrent(){
  if(ConnectionTimeoutFlag == LOW){
    if((SetMQTTCurrentFlag == HIGH) && (MQTTmax_current < max_current)){
        debug += "$";
        debug += "Set new charging current received from MQTT : ";
        debug += MQTTmax_current;
        debug += "$";
        ResponseStatus = LOW;
        while(Serial.available() > 0) {
          char t = Serial.read();
        }
        ResponseMessage = "";
        TempValue = "";
        TempValue += SC;
        TempValue += MQTTmax_current;
        Serial.println(TempValue);
        while(ResponseStatus == LOW && t1 < 8000){
          if(Serial.available()){
            ResponseStatus = HIGH;
            ResponseMessage = Serial.readString();
            ATMessage = ResponseMessage;
          }
          t1 = t1 + 1;
          delayMicroseconds(50);
        }
        if(t1 > 10){
          if (client.connected()){
            topica = "";
            dynamicTopic = "";
    //        epochtimeTopic = getTime();
            dynamicTopic += prefix;
            dynamicTopic += idTopic;
    //        dynamicTopic += "/";
    //        dynamicTopic += epochtimeTopic;
            fullTopic = dynamicTopic;
            fullTopic += TimeoutTopic;
            topica = fullTopic.c_str();
            TempValue = "";
            TempValue += t1;
            TempValueChar = TempValue.c_str();
            client.publish(topica, TempValueChar);
  //          delay(20);
          }
        }
        if(t1 > 7999){
          ConnectionTimeoutFlag = HIGH;
        }else{
          ConnectionTimeoutFlag = LOW;
          t1 = 0;
          
/*          int index = ATMessage.indexOf("$AT");
          if(index > 1){
            ATMessage.remove(0, index);
            ResponseMessage.remove(index);
          }*/
          if (client.connected()){
            topica = "";
            dynamicTopic = "";
        //    epochtimeTopic = getTime();
            dynamicTopic += prefix;
            dynamicTopic += idTopic;
        //    dynamicTopic += "/";
        //    dynamicTopic += epochtimeTopic;
            fullTopic = dynamicTopic;
            fullTopic += ResponseSCTopic;
            topica = fullTopic.c_str();
            TempValue = "";
            TempValue += ResponseMessage;
            TempValueChar = TempValue.c_str();
            client.publish(topica, TempValueChar);
  //          delay(20);
          }
 /*         if(index > 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
          //    epochtimeTopic = getTime();
              dynamicTopic += prefix;
              dynamicTopic += idTopic;
          //    dynamicTopic += "/";
          //    dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += StateChangeTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += ATMessage;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
        //      delay(20);
            }
          }*/
          SetMQTTCurrentFlag = LOW;
          SENDBreakerAlt();
        }
    }
    if(SetMQTTCurrentFlag == HIGH){
      SetMQTTCurrentFlag = LOW;
    }
  }
}

void SetCurrent(){
//  Serial.println("Set Current function");
  if(SetCurrentFlag == HIGH){
//    Serial.println("Set Current flag is HIGH");
      debug += "$";
      debug += "Set new charging current : ";
      debug += max_current;
      debug += "$";
      ResponseStatus = LOW;
      c5 = 0;
      t1 = 0;
      while(Serial.available() > 0) {
        char t = Serial.read();
      }
      if(MQTTmax_current < max_current){
        max_current = MQTTmax_current;
      }
      ResponseMessage = "";
      TempValue = "";
      TempValue += SC;
      TempValue += max_current;
      Serial.println(TempValue);
      while(ResponseStatus == LOW && t1 < 8000){
        if(Serial.available()){
          ResponseStatus = HIGH;
          ResponseMessage = Serial.readString();
          ATMessage = ResponseMessage;
        }
        t1 = t1 + 1;
        delayMicroseconds(50);
      }
      if(t1 > 10){
        if (client.connected()){
          topica = "";
          dynamicTopic = "";
  //        epochtimeTopic = getTime();
          dynamicTopic += prefix;
          dynamicTopic += idTopic;
  //        dynamicTopic += "/";
  //        dynamicTopic += epochtimeTopic;
          fullTopic = dynamicTopic;
          fullTopic += TimeoutTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += t1;
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar);
//          delay(20);
        }
      }
      if(t1 > 7999){
        ConnectionTimeoutFlag = HIGH;
      }else{
        ConnectionTimeoutFlag = LOW;
        t1 = 0;

/*        int index = ATMessage.indexOf("$AT");
        if(index > 1){
          ATMessage.remove(0, index);
          ResponseMessage.remove(index);
        }*/
        if (client.connected()){
          topica = "";
          dynamicTopic = "";
      //    epochtimeTopic = getTime();
          dynamicTopic += prefix;
          dynamicTopic += idTopic;
      //    dynamicTopic += "/";
      //    dynamicTopic += epochtimeTopic;
          fullTopic = dynamicTopic;
          fullTopic += ResponseSCTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += ResponseMessage;
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar);
//          delay(20);
        }
 /*       if(index > 1){
          if (client.connected()){
              topica = "";
              dynamicTopic = "";
          //    epochtimeTopic = getTime();
              dynamicTopic += prefix;
              dynamicTopic += idTopic;
          //    dynamicTopic += "/";
          //    dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += StateChangeTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += ATMessage;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
        //      delay(20);
            }
        }*/
        SetCurrentFlag = LOW;
        set_current = max_current;
      }
  }
}

void CurrentFlagSet(){
  if(max_current < set_current){
    SetCurrentFlag = HIGH;
    debug += "$";
    debug += "Max current Calculated < Max current Set --> Lower charging power";
    debug += "$";
    c5 = 0;
  }
  if((max_current) > (set_current + 1)){
    c5 = c5 +1;
//    Serial.print("naracunan tok je vecji kot ga dovolimo : ");
//    Serial.println(c5);
    if(c5 == 50){
      Serial.println("c5 je 50, povecujem moc");
      debug += "$";
      debug += "c5 je 50, povecujem moc";
      debug += "$";
      SetCurrentFlag = HIGH;
      c5 = 0;
    }
  }
}

void SetTimer(){
  if(ConnectionTimeoutFlag == LOW){
    if(SetTimerFlag == HIGH){
        debug += "$";
        debug += "Setting time Limit :";
        debug += TimeLimit;
        debug += "$";
        ResponseStatus = LOW;
        while(Serial.available() > 0) {
          char t = Serial.read();
        }
        ResponseMessage = "";
        TempValue = "";
        TempValue += ST;
        TempValue += TimeLimit;
        Serial.println(TempValue);
        while(ResponseStatus == LOW && t1 < 8000){
          if(Serial.available()){
            ResponseStatus = HIGH;
            ResponseMessage = Serial.readString();
          }
          t1 = t1 + 1;
          delayMicroseconds(50);
        }
        if(t1 > 10){
          if (client.connected()){
            topica = "";
            dynamicTopic = "";
    //        epochtimeTopic = getTime();
            dynamicTopic += prefix;
            dynamicTopic += idTopic;
    //        dynamicTopic += "/";
    //        dynamicTopic += epochtimeTopic;
            fullTopic = dynamicTopic;
            fullTopic += TimeoutTopic;
            topica = fullTopic.c_str();
            TempValue = "";
            TempValue += t1;
            TempValueChar = TempValue.c_str();
            client.publish(topica, TempValueChar);
  //          delay(20);
          }
        }
        if(t1 > 7999){
          ConnectionTimeoutFlag = HIGH;
        }else{
          ConnectionTimeoutFlag = LOW;
          SetTimerFlag = LOW;
        }
        t1 = 0;
        if (client.connected()){
          topica = "";
          dynamicTopic = "";
      //    epochtimeTopic = getTime();
          dynamicTopic += prefix;
          dynamicTopic += idTopic;
      //    dynamicTopic += "/";
      //    dynamicTopic += epochtimeTopic;
          fullTopic = dynamicTopic;
          fullTopic += ResponseSTTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += ResponseMessage;
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar);
  //        delay(20);
        }
    }  
  }
}

void SetLimit(){
  if(ConnectionTimeoutFlag == LOW){
    if(SetEnergyLimitFlag == HIGH){
        debug += "$";
        debug += "Setting energy Limit :";
        debug += EnergyLimit;
        debug += "$";
        ResponseStatus = LOW;
        while(Serial.available() > 0) {
          char t = Serial.read();
        }
        ResponseMessage = "";
        TempValue = "";
        TempValue += SH;
        TempValue += EnergyLimit;
        Serial.println(TempValue);
        while(ResponseStatus == LOW && t1 < 8000){
          if(Serial.available()){
            ResponseStatus = HIGH;
            ResponseMessage = Serial.readString();
          }
          t1 = t1 + 1;
          delayMicroseconds(50);
        }
        if(t1 > 10){
          if (client.connected()){
            topica = "";
            dynamicTopic = "";
    //        epochtimeTopic = getTime();
            dynamicTopic += prefix;
            dynamicTopic += idTopic;
    //        dynamicTopic += "/";
    //        dynamicTopic += epochtimeTopic;
            fullTopic = dynamicTopic;
            fullTopic += ResponseSHTopic;
            topica = fullTopic.c_str();
            TempValue = "";
            TempValue += t1;
            TempValueChar = TempValue.c_str();
            client.publish(topica, TempValueChar);
  //          delay(20);
          }
        }
        if(t1 > 7999){
          ConnectionTimeoutFlag = HIGH;
        }else{
          ConnectionTimeoutFlag = LOW;
          SetEnergyLimitFlag = LOW;
        }
        t1 = 0;
        if (client.connected()){
          topica = "";
          dynamicTopic = "";
      //    epochtimeTopic = getTime();
          dynamicTopic += prefix;
          dynamicTopic += idTopic;
      //    dynamicTopic += "/";
      //    dynamicTopic += epochtimeTopic;
          fullTopic = dynamicTopic;
          fullTopic += ResponseSHTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += ResponseMessage;
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar);
  //        delay(20);
        }
    }    
  }
}

void SaveLastCurrents(){
  LastCurrent1 = average1;
  debug += "$";
  debug += "Last saved current 1 :";
  debug += LastCurrent1;
  debug += "$";
  LastCurrent2 = average2;
  debug += "$";
  debug += "Last saved current 2 :";
  debug += LastCurrent2;
  debug += "$";
  LastCurrent3 = average3;
  debug += "$";
  debug += "Last saved current 3 :";
  debug += LastCurrent3;
  debug += "$";
}

void CheckPhaseChange(){
  if(PhaseInfo == LOW){
//    Serial.println("PhaseCheck");
    debug += "$";
    debug += "PhaseCheck";
    debug += "$";
    Current1Change = average1 - LastCurrent1 + 3;
    Current2Change = average2 - LastCurrent2 + 3;
    Current3Change = average3 - LastCurrent3 + 3;
//    Serial.print("changes :  ");
//    Serial.print(Current1Change);
//    Serial.print("  ;  ");
//    Serial.print(Current2Change);
//    Serial.print("  ;  ");
//    Serial.println(Current3Change);
//    Serial.print("charge current :  ");
//    Serial.println(charge_current);
    if(charge_current > 6){
      debug += "$";
      debug += "Charge current > 6A";
      debug += "$";
      if(Current1Change >= charge_current){
        active_phases = 1;
        debug += "$";
        debug += "Current change on phase1 detected";
        debug += "$";
        if(Current2Change >= charge_current){
          active_phases = 12;
          debug += "$";
          debug += "Current change on phase2 detected";
          debug += "$";
          if(Current3Change >= charge_current){
            active_phases = 123;
            debug += "$";
            debug += "Current change on phase3 detected";
            debug += "$";
          }
        }else if(Current3Change >= charge_current){
          active_phases = 13;
          debug += "$";
          debug += "Current change on phase3 detected";
          debug += "$";
        }
      }else if(Current2Change >= charge_current){
        active_phases = 2;
        debug += "$";
        debug += "Current change on phase2 detected";
        debug += "$";
        if(Current3Change >= charge_current){
          active_phases = 23;
          debug += "$";
          debug += "Current change on phase3 detected";
          debug += "$";
        }
      }else if(Current3Change >= charge_current){
        active_phases = 3;
        debug += "$";
        debug += "Current change on phase3 detected";
        debug += "$";
      }
    }else if(charge_current < 0.5){
      debug += "$";
      debug += "Charge current < 0.5A";
      debug += "$";
      if(Current2Change >= 5){
        active_phases = 20;
        debug += "$";
        debug += "Current change on phase2 >=5A detected";
        debug += "$";
        if(Current3Change >= 5){
          active_phases = 23;
          debug += "$";
          debug += "Current change on phase3 >=5A detected";
          debug += "$";
        }
      }else if(Current3Change >= 5){
        active_phases = 30;
        debug += "$";
        debug += "Current change on phase3 >=5A detected";
        debug += "$";
      }
    }else{
      active_phases = 99;
      debug += "$";
      debug += "Charging current is between 0.5A - 6A";
      debug += "$";
    }
    if(active_phases != 0){
      PhaseInfo = HIGH;
      debug += "$";
      debug += "Phases calculated";
      debug += "$";
    }
  }
}


void StopCharge(){
  if(ConnectionTimeoutFlag == LOW){
    if(ChargeSetState == HIGH && PAndC == LOW){
      if(charge_current > 0.5){
        c2 = 0;
      }else{
        c2 = c2+1;
        if(c2 > timer1){
          Serial.println("Izklop, premali tok");
          debug += "$";
          debug += "Izklop, premali tok";
          debug += "$";
          TurnSleep();
        }
      }
      if((State != 0) && (State != 2)){
        c6 = 0;
      }else{
        c6 = c6 + 1;
        if(c6 > timer2){
          Serial.println("Izklop, state");
          debug += "$";
          debug += "Izklop, state";
          debug += "$";
          TurnSleep();
        }
      }
      if(charge_current <= max_current + 0.2){
        c3 = 0; 
      }else{
        c3 = c3+1;
        if(c3 > timer3){
          Serial.println("Izklop, prevelik tok");
          debug += "$";
          debug += "Izklop, prevelik tok";
          debug += "$";
          TurnSleep();
        }     
      }
      if(max_current > 3){
        c4 = 0;
      }else{
        c4 = c4+1;
        if(c4 > timer4){
          Serial.println("Izklop, ni na voljo dovolj toka");
          debug += "$";
          debug += "Izklop, ni na voljo dovolj toka";
          debug += "$";
          TurnSleep();
        }
      }
    }
  }
}


void SendDebug(){
  if (client.connected()){
    if(debug.length() > 3){
      const char * DebugChar;
      topica = "";
      dynamicTopic = "";
  //    epochtimeTopic = getTime();
      dynamicTopic += prefix;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += DebugTopic;
      topica = fullTopic.c_str();
      DebugChar = debug.c_str();
      client.publish(topica, DebugChar);
  //    delay(20);
      debug = "";
    }
  }
}

void CatchStateChange(){
  if(Serial.available()){
    ResponseMessage = Serial.readString();
    if (client.connected()){
          topica = "";
          dynamicTopic = "";
      //    epochtimeTopic = getTime();
          dynamicTopic += prefix;
          dynamicTopic += idTopic;
      //    dynamicTopic += "/";
      //    dynamicTopic += epochtimeTopic;
          fullTopic = dynamicTopic;
          fullTopic += StateChangeTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += ResponseMessage;
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar);
    }
    int index = ATMessage.indexOf(" ");
    ATMessage.remove(0, index+1);
    ATMessage.remove(index+4);
    Serial.print("Catch state izrez je : ");
    Serial.println(ATMessage);
    if(ChargeSetState == HIGH && PAndC == LOW){
      if(ATMessage != "03 03"){
        TurnSleep();
      }
    }
  }
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Connected to AP successfully!");
  debug += "$";
  debug += "Connected to AP successfully!";
  debug += "$";
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  debug += "$";
  debug += "WiFi connected, IP address: ";
  debug += WiFi.localIP();
  debug += ", RSSI:  ";
  debug += WiFi.RSSI();
  debug += "$";
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(ssid.c_str(), pass.c_str());
  debug += "$";
  debug += "Disconnected from WiFi access point,  WiFi lost connection. Reason: ";
  debug += info.disconnected.reason;
  debug += "$";
}

void WiFiReconnect(){
  now11 = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (now11 - lastInfo11 >= timer11)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    debug += "$";
    debug += "Reconnecting to WiFi...";
    debug += "$";
    WiFi.disconnect();
    WiFi.reconnect();
    lastInfo11 = now11;
  }
}

void WiFiConnect(){
  WiFi.disconnect();
  WiFi.begin(ssid.c_str(), pass.c_str());
}

void WiFiConnect1(){
  WiFi.disconnect();
  WiFi.begin(ssid1.c_str(), pass1.c_str());
}


void Task1code( void * pvParameters ){
  while(SetupComplete == LOW){
    vTaskDelay(100);
  }
  pinMode(ct_sensor_1, INPUT_PULLUP);
  pinMode(ct_sensor_2, INPUT_PULLUP);
  pinMode(ct_sensor_3, INPUT_PULLUP);
  emon1.current(ct_sensor_1, calibration); // Current: input pin, calibration 102.1 = Arduino pro mini
  vTaskDelay(50);
  emon2.current(ct_sensor_2, calibration);
  vTaskDelay(50);
  emon3.current(ct_sensor_3, calibration);

  for(;;){

    Irms_1 = emon1.calcIrms(1480);  // Calculate Irms only  1480
    Irms_2 = emon2.calcIrms(1480);
    Irms_3 = emon3.calcIrms(1480);
    vTaskDelay(20);
  } 
}
