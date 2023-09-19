#include "EmonLib.h" // Include Emon Library
#include "math.h"
#include "driver/adc.h"
#include "esp32FOTA.hpp"
#include "WiFi.h"
#include "PubSubClient.h"
#include "ESPAsyncWebServer.h"            // https://github.com/me-no-dev/ESPAsyncWebServer
// download zip from above->Arduino IDE->menu->tab->sketch->include library->add ZIP library
#include "AsyncTCP.h"                     // https://github.com/me-no-dev/AsyncTCP
// download zip from above->Arduino IDE->menu->tab->sketch->include library->add ZIP library
#include "SPIFFS.h"
//#include "LittleFS.h"
#include "ESPmDNS.h"
#include "HTTPClient.h"
#include "ESP32httpUpdate.h"
#include "Preferences.h"
#include "ESP32-targz.h" // http://github.com/tobozo/ESP32-targz



#define RXD2 17
#define TXD2 16

#define RX2LORA 25
#define TX2LORA 33
 

// Initiate Preferences to save WiFi credentials to EEPROM
Preferences preferences;
bool SavedWiFi = LOW;
bool SavedCalibration = LOW;

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


//WiFi found bools
  bool networkFound = false;
  bool networkFound1 = false;


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


long lastInfo5 = 0;
long lastInfo6 = 0;
long lastInfo7 = 0;
long lastInfo8 = 0;
long lastInfo9 = 0;
long lastInfo10 = 0;
long lastInfo11 = 0;
long lastInfo12 = 0;

long timer;
long timer1;
long timer2;
long timer3;
long timer4;
long timer5;
long timer6;
long timer7;
long timer8;
long timer9;
long timer10;
long timer11;
long timer12;
long timer13;
long timer14;

uint8_t TimersFactorOff;
uint8_t LoRaCERetries;
uint8_t LoRaCERetryCount = 3;
long Requestmillis;

long NegAmpTime;
long nowNegAmp;



long TimeoutTime;
bool TimeoutTimeSet = LOW;
bool COMused = LOW;

int vmesna;
uint16_t r = 0;



// Second Core Settings//
TaskHandle_t Task1;

long lastInfo = 0;
long lastInfo1 = 0;
long lastInfo2 = 0;

// Change to your WiFi credentials
const char *ssid1 = "Implera";
const char *pass1 = "AdminSettings";

// esp32fota esp32fota("<Type of Firme for this device>", <this version>);
esp32FOTA esp32FOTA;

String FW_versionStr = "0.2.9";

#define FOTA_URL "http://lockit.pro/ota/DinamicsHW2/DinamicsHW2.json"
const char *firmware_name = "DinamicsHW2";
const bool check_signature = false;
const bool disable_security = true;

int firmware_version_major = 0;
int firmware_version_minor = 2;
int firmware_version_patch = 8;


// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";
const char* mqtt_server = "mqshare.napolni.me";

#define S1 5
#define S2 18
#define S3 19
#define S4 21

#define LED_GREEN 12
#define LED_RED 14


#define ct_sensor_1 32 // ESP32 Wroom
#define ct_sensor_2 35
#define ct_sensor_3 34


volatile float Irms_1;
volatile float Irms_2;
volatile float Irms_3;

EnergyMonitor emon1; // Create an instance
EnergyMonitor emon2; // Create an instance
EnergyMonitor emon3; // Create an instance


String deviceName = "Dinamics-";
const char * chardeviceName;
char charid[23];
String idTopic;
String epochtimeTopic;
const char* prefixTopic = "Dinamics/";
String fullTopic;
String dynamicTopic;
const char* BreakerTopic = "/breaker";
const char* FWversionTopic = "/fwversion";
const char* I1Topic = "/current1";
const char* I2Topic = "/current2";
const char* I3Topic = "/current3";
const char* Max_ITopic = "/max_charging_current";
const char* Charge_ITopic = "/charging_current";
const char* PowerTopic = "/power";
const char* EnergyTopic = "/energy";
const char* PhasesTopic = "/active_phases";
const char* NoOfPhasesTopic = "/no_of_phases";
const char* EnableTopic = "/enable";

const char* TimeoutTopic = "/timeout";
const char* ResponseGCTopic = "/responseGC";
const char* ResponseG0Topic = "/responseG0";
const char* ResponseGGTopic = "/responseGG";
const char* ResponseGSTopic = "/responseGS";
const char* ResponseGUTopic = "/responseGU";
const char* ResponseFDTopic = "/responseFD";
const char* ResponseFETopic = "/responseFE";
const char* ResponseFSTopic = "/responseFS";
const char* ResponseSCTopic = "/responseSC";
const char* ResponseSTTopic = "/responseST";
const char* ResponseSHTopic = "/responseSH";
const char* StateTopic = "/state";
const char* ConnectionTopic = "/connection";
const char* DebugTopic = "/debug_log";
const char* SyncTimeTopic = "/synctime";
const char* StateChangeTopic = "/state_change";
const char* SendSettingsTopic = "/settings";
const char* SendTimersTopic = "/timers";
const char* SendWiFiTopic = "/wifi";
const char* SendDebugTopic = "/debug";
const char* RapiTopic = "/rapi_response";
const char* PandCTopic = "/plugandcharge";
const char* ipTopic = "/ip";
const char* ResponseLCDEraseTopic = "/responseEraseLCD";
const char* FreeRAMTopic = "/freeRAM";


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
const char * charCurrentMinLimit;
String sub_CurrentMinLimitTopic;
const char * charCurrentCalibration;
String sub_CurrentCalibrationTopic;
const char * charDebug;
String sub_DebugTopic;
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
const char * charTimer13;
String sub_Timer13Topic;
const char * charTimer14;
String sub_Timer14Topic;
const char * charTFO;
String sub_TFOTopic;
const char * charDeleteSettings;
String sub_DeleteSettingsTopic;
const char * charDeleteWifi;
String sub_DeleteWifiTopic;
const char * charPlugAndCharge;
String sub_PlugAndChargeTopic;
const char * charNoWANPandC;
String sub_NoWANPandCTopic;
const char * charNegativeAmperage;
String sub_NegativeAmperageTopic;
const char * charAdjust;
String sub_AdjustTopic;
const char * charAutoUpdate;
String sub_AutoUpdateTopic;
const char * charGetSettings;
String sub_GetSettingsTopic;
const char * charGetTimers;
String sub_GetTimersTopic;
const char * charGetWiFi;
String sub_GetWiFiTopic;
const char * charGetDebug;
String sub_GetDebugTopic;
const char * charLCDon;
String sub_LCDonTopic;
const char * charLCDErase;
String sub_LCDEraseTopic;
const char * charCTEnable;
String sub_CTEnableTopic;
const char * charDinamicsEnable;
String sub_DinamicsEnableTopic;
const char * charLoRa;
String sub_LoRaTopic;


float calibration = 54.4;

bool SS1;
bool SS2;
bool SS3;
bool SS4;

bool debug1;
bool debug1_MQTT;
bool debug2;
bool debug2_MQTT;
bool debug3;
bool debug3_MQTT;
bool debug4;
bool debug4_MQTT;
bool debug5;
bool debug5_MQTT;
bool debug6;
bool debug6_MQTT;
bool debug7;
bool debug7_MQTT;
bool debug8;
bool debug8_MQTT;
bool debug9;
bool debug9_MQTT;
bool debug10;
bool debug10_MQTT;
bool debug11;
bool debug11_MQTT;
bool debug12;
bool debug12_MQTT;
bool debug13;
bool debug13_MQTT;
bool debug14;
bool debug14_MQTT;
bool debug15;
bool debug15_MQTT;
bool debug16;
bool debug16_MQTT;
bool debug17;
bool debug17_MQTT;
bool debug18;
bool debug18_MQTT;
bool debug19;
bool debug19_MQTT;
bool debug20;
bool debug20_MQTT;

float charge_current;
int16_t max_current;
int16_t MQTTmax_current;
int8_t min_current;
int16_t set_current;
uint16_t breaker;
uint8_t active_phases;
uint8_t noofphases;
String temp_message;
String TempValue;
const char * TempValueChar;
uint32_t energy;
float power;
String debuglog;
String RAPI;
int CTEnable;
uint16_t EnableState;


const char* GC = "$GC";
const char* G0 = "$G0";
const char* GG = "$GG";
const char* GS = "$GS";
const char* GU = "$GU";
const char* FD = "$FD";
const char* FE = "$FE";
const char* FS = "$FS";
const char* SC = "$SC ";
const char* ST = "$S3 ";
const char* SH = "$SH ";
String ResponseMessage;
String ResponseMessageTemp;
String ResponseMessageAsync;
String ATMessage;
String LastCom;


uint8_t State;
uint8_t OldState;

bool SetChargeFlag = HIGH;
bool ChargeSetState;
bool SetMQTTCurrentFlag;
bool SetCurrentFlag;
bool SetMQTTbreakerFlag;
bool ConnectionTimeoutFlag = HIGH;
bool ActiveTimeoutFlag = LOW;
bool SetTimerFlag;
bool SetEnergyLimitFlag;
bool PhaseInfo;
bool PAndC;
bool SendSettings = LOW;
bool SendTimers = LOW;
bool SendWiFi = LOW;
bool SendDebug = LOW;
bool TranslateDebug = LOW;
bool UpdateStart = LOW;
bool UpdateSpiffs = LOW;
bool ResponseStatus;
bool SetupComplete = LOW;
bool FWSentFlag = LOW;
bool ipSentFlag = LOW;
bool AskRAPI = LOW;
bool PowerOn=LOW;
bool ImpleraAdjust;
bool NegativeAmperage;
bool NegAmpFlag = LOW;
bool AutoUpdate;
bool SetChargeSettingsFlag;
bool RestoreChargeSettingsFlag;
bool SetWiFiCredsFlag;
bool DeleteWiFiCredsFlag;
bool SetAutoUpdateFlag;
bool NegAmpOnceFlag = LOW;
bool NoWANPandCActive = LOW;
bool NoWANPandC;
bool LCDon;
bool LCDEraseFlag;
bool DinamicsActive;
bool ResponseAsyncFlag;
bool LoRa;
bool ResponseStatusRAPIF = HIGH;
bool ResponseStatusSetLimit = HIGH;
bool ResponseStatusCheckState = HIGH;
bool ResponseStatusCheckStatus = HIGH;
bool ResponseStatusCheckSetAmps = HIGH;
bool ResponseStatusCheckCharge = HIGH;
bool ResponseStatusCheckEnergy = HIGH;
bool ResponseStatusTurnOn = HIGH;
bool ResponseStatusTurnOff = HIGH;
bool ResponseStatusTurnSleep = HIGH;
bool ResponseStatusSetMQTTCurrent = HIGH;
bool ResponseStatusSetCurrent = HIGH;
bool ResponseStatusSetTimer = HIGH;

int PowerStatus;


uint8_t tmp;
uint8_t MQTTBreaker;
uint8_t EnergyLimit;
uint8_t TimeLimit;



uint16_t t1;
uint16_t c1 = 0;
uint16_t c2 = 0;
uint16_t c3 = 0;
uint16_t c4 = 0;
uint16_t c5 = 0;
uint16_t c6 = 0;


uint32_t wifi_reconnects = 0;


float average1Old;
float average2Old;
float average3Old;
int16_t max_currentOld;
float charge_currentOld;
uint8_t active_phasesOld;
float powerOld;
uint32_t energyOld;



const int NoRead1 = 3;
float total1 = 0;
float average1 = 0;
unsigned int readindex1 = 0;
float branja1[NoRead1];

const int NoRead2 = 3;
float total2 = 0;
float average2 = 0;
unsigned int readindex2 = 0;
float branja2[NoRead2];

const int NoRead3 = 3;
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
    if(debug2 == 1){
      Serial.println("Failed to obtain time");
    }
    return(0);
  }
  time(&now);
  return now;
}

//---------------------------------------------------------------------------------------------------------

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    if(debug2 == 1){
      Serial.println("An error has occurred while mounting SPIFFS");
    }
  }
  if(debug2 == 1){
    Serial.println("SPIFFS mounted successfully");
  }
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char * path) {
  if(debug2 == 1){
    Serial.printf("Reading file: %s\r\n", path);
  }

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    if(debug2 == 1){
      Serial.println("- failed to open file for reading");
    }
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
/*void writeFile(fs::FS &fs, const char * path, const char * message) {
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
}*/

// Initialize WiFi
bool initWiFi() {
  WiFi.mode(WIFI_STA);

  if (dhcpcheck == "on") {
    // dhcp

  } else {
    // fixed ip
    localIP.fromString(ip.c_str());
    gatewayIP.fromString(gateway.c_str());
    subnetMask.fromString(subnet.c_str());

    if (!WiFi.config(localIP, gatewayIP, subnetMask)) {
      if(debug2 == 1){
        Serial.println("STA Failed to configure");
      }
      return false;
    }
  }

  Serial.println("Scanning for networks...");
  int n = WiFi.scanNetworks();
  Serial.println("Scan complete");


  for (int i = 0; i < n; i++) {
    if (WiFi.SSID(i) == ssid) {
      networkFound = true;
      break;
    }else if(WiFi.SSID(i) == ssid1){
      networkFound1 = true;
      break;
    }
  }
  
  if(networkFound == true){
    WiFi.begin(ssid.c_str(), pass.c_str());
    if(debug2 == 1){
      Serial.println("Connecting to WiFi...");
    }
  
    unsigned long currentMillis = millis();
    previousMillis = currentMillis;
  
    while (WiFi.status() != WL_CONNECTED) {
      currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        if(debug2 == 1){
          Serial.println("Failed to connect.");
        }      
        return false;
      }
    }
    delay(500);
    if(debug2 == 1){
      Serial.println("");
      Serial.println(WiFi.localIP());
    }
    ip = WiFi.localIP().toString();
    gateway = WiFi.gatewayIP().toString();
    if(ip.length()>0){
      ipSentFlag = LOW;
    }
  }else if(networkFound1 == true){
    if(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid1, pass1);
      if(debug2 == 1){
        Serial.println("Connecting to Implera Service WiFi...");
      }
    
      unsigned long currentMillis = millis();
      previousMillis = currentMillis;
    
      while (WiFi.status() != WL_CONNECTED) {
        currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
          if(debug2 == 1){
            Serial.println("Failed to connect.");
          }
          return false;
        }
      }
      delay(500);
      if(debug2 == 1){
        Serial.println("");
        Serial.println(WiFi.localIP());
      }
      ip = WiFi.localIP().toString();
      gateway = WiFi.gatewayIP().toString();
      if(ip.length()>0){
        ipSentFlag = LOW;
      }
    }
  }else{
    if(debug2 == 1){
      Serial.println("Undefined SSID wrong wifiroutername or wifirouterpassword");
    }
    return false;
  }
  

//  if (!MDNS.begin("dynamics")) {
//    Serial.println("Error setting up MDNS responder!");
////    return;
//  }
//
//  MDNS.addService("http", "tcp", 80);

//  Serial.print("http://");
//  Serial.print(mdnsdotlocalurl);
//  Serial.println(".lan");
  return true;
}

//---------------------------------------------------------------------------------------------------------

void callback(char* topic, byte* message, unsigned int length) {
  if(debug2 == 1){
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
  }
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    if(debug2 == 1){
      Serial.print((char)message[i]);
    }
    messageTemp += (char)message[i];
  }
  if(debug2 == 1){
    Serial.println();
  }

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == sub_BreakerTopic) {
    if(debug3 == 1){
      Serial.print("Set breaker from server ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    MQTTBreaker = temp_message.toInt();
    breaker = MQTTBreaker;
    SetBreaker();
    SENDBreakerAlt();
  }

  vTaskDelay(10);

  if (String(topic) == sub_StatusTopic) {
    if(debug3 == 1){
      Serial.print("Status_inquiry ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    SENDCurrentsAlt();
  }

  vTaskDelay(10);

  if (String(topic) == sub_UpdateTopic) {
    if(debug3 == 1){
      Serial.print("New update JSON link: ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    UpdateStart = HIGH;
  }

  vTaskDelay(10);

  if (String(topic) == sub_UpdateSpiffsTopic) {
    if(debug3 == 1){
      Serial.print("New update SPIFFS link: ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    UpdateSpiffs = HIGH;
  }

  vTaskDelay(10);

  if (String(topic) == sub_EnableTopic) {
    if(debug3 == 1){
      Serial.print("Setting Enable ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    tmp = temp_message.toInt();
    SetChargeFlag = HIGH;
    ChargeChanger();
  }

  vTaskDelay(10);

  if (String(topic) == sub_ChargeTimerTopic) {
    if(debug3 == 1){
      Serial.print("Timer received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    TimeLimit = temp_message.toInt();
    SetTimerFlag = HIGH;
    SetTimer();
  }

  vTaskDelay(10);

  if (String(topic) == sub_EnergyLimitTopic) {
    if(debug3 == 1){
      Serial.print("EnergyLimit received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    EnergyLimit = temp_message.toInt();
    SetEnergyLimitFlag = HIGH;
    SetLimit();
  }

  vTaskDelay(10);

  if (String(topic) == sub_CurrentLimitTopic) {
    if(debug3 == 1){
      Serial.print("CurrentLimit received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    MQTTmax_current = temp_message.toInt();
    SetMQTTCurrentFlag = HIGH;
    SetMaxMQTTCurrent();
  }

  vTaskDelay(10);

  if (String(topic) == sub_CurrentMinLimitTopic) {
    if(debug3 == 1){
      Serial.print("CurrentMinLimit received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    min_current = temp_message.toInt();
    SetMinCurrent();
  }

  vTaskDelay(10);

  if (String(topic) == sub_CurrentCalibrationTopic) {
    if(debug3 == 1){
        Serial.print("Current calibration value received ");
        Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    calibration = temp_message.toInt();
    calibration = calibration/10;
    SetCalibration();
  }

  vTaskDelay(10);

  if (String(topic) == sub_RAPITopic) {
    if(debug3 == 1){
      Serial.print("RAPI command received ");
      Serial.println(messageTemp);
    }
    RAPI = messageTemp;
    AskRAPI = HIGH;
    AskRAPIF(); 
  }

  vTaskDelay(10);

  if (String(topic) == sub_TimerTopic) {
    if(debug3 == 1){
      Serial.print("timer value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer1Topic) {
    if(debug3 == 1){
      Serial.print("Counter1 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer1 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer2Topic) {
    if(debug3 == 1){
      Serial.print("Counter2 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer2 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer3Topic) {
    if(debug3 == 1){
      Serial.print("Counter3 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer3 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer4Topic) {
    if(debug3 == 1){
      Serial.print("Counter4 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer4 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer5Topic) {
    if(debug3 == 1){
      Serial.print("Counter5 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer5 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer6Topic) {
    if(debug3 == 1){
      Serial.print("Counter6 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer6 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer7Topic) {
    if(debug3 == 1){
      Serial.print("Counter7 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer7 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer8Topic) {
    if(debug3 == 1){
      Serial.print("Counter8 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer8 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer9Topic) {
    if(debug3 == 1){
      Serial.print("Counter9 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer9 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer10Topic) {
    if(debug3 == 1){
      Serial.print("Counter10 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer10 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer11Topic) {
    if(debug3 == 1){
      Serial.print("Counter11 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer11 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer12Topic) {
    if(debug3 == 1){
      Serial.print("Counter12 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer12 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer13Topic) {
    if(debug3 == 1){
      Serial.print("Counter13 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer13 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);

  if (String(topic) == sub_Timer14Topic) {
    if(debug3 == 1){
      Serial.print("Counter14 value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    timer14 = temp_message.toDouble();
    SetTimers();
  }

  vTaskDelay(10);


  if (String(topic) == sub_TFOTopic) {
    if(debug3 == 1){
      Serial.print("TFO value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    TimersFactorOff = temp_message.toInt();
    SetTimersFactor();
  }

  vTaskDelay(10);

  if (String(topic) == sub_DeleteSettingsTopic) {
    if(debug3 == 1){
      Serial.print("Delete settings received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    int vmesna = temp_message.toInt();
    if(vmesna == 1){
      DeleteSettings();
    }
  }

  vTaskDelay(10);

  if (String(topic) == sub_DeleteWifiTopic) {
    if(debug3 == 1){
      Serial.print("Delete wifi credentials received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    int vmesna = temp_message.toInt();
    if(vmesna == 1){
      DeleteWiFiCredsFlag = HIGH;
    }
  }

  vTaskDelay(10);

  if (String(topic) == sub_PlugAndChargeTopic) {
    if(debug3 == 1){
      Serial.print("Plug and Charge setting update received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    vmesna = temp_message.toInt();
    RespondPandC();
    if(vmesna == 1){
      PAndC = HIGH;
      SetPAC();
    }else{
      PAndC = LOW;
      SetPAC();
    }
  }

  vTaskDelay(10);

  if (String(topic) == sub_NoWANPandCTopic) {
    if(debug3 == 1){
      Serial.print("No WAN Plug and Charge setting update received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    vmesna = temp_message.toInt();
    if(vmesna == 1){
      NoWANPandC = HIGH;
      SetNWPC();
    }else{
      NoWANPandC = LOW;
      SetNWPC();
    }
  }

  vTaskDelay(10);

  if (String(topic) == sub_NegativeAmperageTopic) {
    if(debug3 == 1){
      Serial.print("NEgative amperage setting update received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    vmesna = temp_message.toInt();
    if(vmesna == 1){
      NegativeAmperage = HIGH;
      SetNegativeAmperage();
    }else{
      NegativeAmperage = LOW;
      SetNegativeAmperage();
    }
  }

  vTaskDelay(10);

  if (String(topic) == sub_AdjustTopic) {
    if(debug3 == 1){
      Serial.print("Adjust setting update received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    vmesna = temp_message.toInt();
    if(vmesna == 1){
      ImpleraAdjust = HIGH;
      SetAdjust();
    }else{
      ImpleraAdjust = LOW;
      SetAdjust();
    }
  }

  vTaskDelay(10);

  if (String(topic) == sub_AutoUpdateTopic) {
    if(debug3 == 1){
      Serial.print("Auto update setting update received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    vmesna = temp_message.toInt();
    if(vmesna == 1){
      AutoUpdate = HIGH;
      SetAutoUpdate();
    }else{
      AutoUpdate = LOW;
      SetAutoUpdate();
    }
  }

  vTaskDelay(10);

  if (String(topic) == sub_DebugTopic) {
    if(debug3 == 1){
      Serial.print("Debug value received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    debuglog = temp_message;
    TranslateDebug = HIGH;
    TranslateDebugF();
  }

  vTaskDelay(10);

  if (String(topic) == sub_GetSettingsTopic) {
    if(debug3 == 1){
      Serial.print("Request settings received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    SendSettings = HIGH;
    SendSettingsF();
  }

  vTaskDelay(10);

  if (String(topic) == sub_GetTimersTopic) {
    if(debug3 == 1){
      Serial.print("Request timers received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    SendTimers = HIGH;
    SendTimersF();
  }

  vTaskDelay(10);

  if (String(topic) == sub_GetWiFiTopic) {
    if(debug3 == 1){
      Serial.print("Request WiFi credentials received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    SendWiFi = HIGH;
    SendWiFiF();
  }

  vTaskDelay(10);

  if (String(topic) == sub_GetDebugTopic) {
    if(debug3 == 1){
      Serial.print("Request Debug settings received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    SendDebug = HIGH;
    SendDebugF();
  }

  vTaskDelay(10);

  if (String(topic) == sub_LCDonTopic) {
    if(debug3 == 1){
      Serial.print("Set LCD power received ");
      Serial.println(messageTemp);
    }
    temp_message = messageTemp;
    vmesna = temp_message.toInt();
    if(vmesna == 1){
      LCDon = HIGH;
      SetLCDon();
    }else{
      LCDon = LOW;
      SetLCDon();
    }
  }

  vTaskDelay(10);

  if (String(topic) == sub_LCDEraseTopic) {
    if(debug3 == 1){
      Serial.print("LCD erase received ");
      Serial.println(messageTemp);
    }
    LCDEraseFlag = HIGH;
  }

  vTaskDelay(10);

  if (String(topic) == sub_CTEnableTopic) {
    if(debug3 == 1){
      Serial.print("CT enable received ");
      Serial.println(messageTemp);
    }
    CTEnable = messageTemp.toInt();
    SetCTEnable();
  }

  vTaskDelay(10);

  if (String(topic) == sub_DinamicsEnableTopic) {
    if(debug3 == 1){
      Serial.print("Dinamics enable received ");
      Serial.println(messageTemp);
    }
    DinamicsActive = messageTemp.toInt();
    SetDinamicsEnable();
  }

  vTaskDelay(10);

  if (String(topic) == sub_LoRaTopic) {
    if(debug3 == 1){
      Serial.print("LoRa settings received ");
      Serial.println(messageTemp);
    }
    LoRa = messageTemp.toInt();
    SetLoRa();
  }
}

void reconnect() {
  // Loop until we're reconnected
  int i = 0;
  while (!client.connected() && i<10) {
    if(debug2 == 1){
      Serial.print("Attempting MQTT connection...");
    }
    i=i+1;
    r=r+1;
    vTaskDelay(300);
    // Attempt to connect
    if(i==10){
      WiFiReconnect();
    }
    if (client.connect(chardeviceName, "sharelock" , "biciklekomplikovanije")) {
      if(debug2 == 1){
        Serial.println("connected");
      }
      // Subscribe
      client.subscribe(charBreaker);
      client.subscribe(charUpdate);
      client.subscribe(charUpdateSpiffs);
      client.subscribe(charStatus);
      client.subscribe(charEnable);
      client.subscribe(charChargeTimer);
      client.subscribe(charEnergyLimit);
      vTaskDelay(10);
      client.subscribe(charCurrentLimit);
      client.subscribe(charCurrentMinLimit);
      client.subscribe(charCurrentCalibration);
      client.subscribe(charDebug);
      client.subscribe(charRAPI);
      client.subscribe(charTimer);
      vTaskDelay(10);
      client.subscribe(charTimer1);
      client.subscribe(charTimer2);
      client.subscribe(charTimer3);
      client.subscribe(charTimer4);
      client.subscribe(charTimer5);
      client.subscribe(charTimer6);
      vTaskDelay(10);
      client.subscribe(charTimer7);
      client.subscribe(charTimer8);
      client.subscribe(charTimer9);
      client.subscribe(charTimer10);
      client.subscribe(charTimer11);
      client.subscribe(charTimer12);
      client.subscribe(charTimer13);
      client.subscribe(charTimer14);
      vTaskDelay(10);
      client.subscribe(charTFO);
      client.subscribe(charPlugAndCharge);
      client.subscribe(charNoWANPandC);
      client.subscribe(charAdjust);
      client.subscribe(charAutoUpdate);
      client.subscribe(charGetSettings);
      client.subscribe(charDeleteSettings);
      client.subscribe(charDeleteWifi);
      client.subscribe(charGetWiFi);
      client.subscribe(charGetDebug);
      vTaskDelay(10);
      client.subscribe(charNegativeAmperage);
      client.subscribe(charLCDon);
      client.subscribe(charLCDErase);
      client.subscribe(charGetTimers);
      client.subscribe(charCTEnable);
      client.subscribe(charDinamicsEnable);
      client.subscribe(charLoRa);
    } else {
      if(debug2 == 1){
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
      // Wait 5 seconds before retrying
      vTaskDelay(5000);
    }
  }
}

void selectTopic(){
    topica = "";
    dynamicTopic = "";
    epochtimeTopic = getTime();
    dynamicTopic += prefixTopic;
    dynamicTopic += idTopic;
    dynamicTopic += "/";
    dynamicTopic += epochtimeTopic;
}

void GetRuntimeSettings(){
  preferences.begin("RuntimeSets", true);
  PowerOn = preferences.getBool("poweron", LOW);
  preferences.end();
  if(PowerOn == HIGH){
    tmp=2;
  }else{
    tmp=3;
  }
}

void SetRuntimeSettings(){
  preferences.begin("RuntimeSets", false);
  preferences.putBool("poweron", PowerOn);
  preferences.end();
}

void CheckWiFiCredentials(){
  preferences.begin("WiFiCred", true);
  String tempSSID = preferences.getString("ssid", "");
  uint8_t SSIDlength = tempSSID.length();
  debuglog += "$checking credentials in preferences$";
  if(SSIDlength > 1){
    SavedWiFi = HIGH;
    debuglog += "$found credentials in preferences$";
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
    debuglog += "$read credentials from preferences$";
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
  debuglog += "$saving credentials to preferences$";
}

void DeleteWiFiCredentials(){
  preferences.begin("WiFiCred", false);
  preferences.clear();
  preferences.end();
  debuglog += "$deleting credentials from preferences$";
}

void SetChargeSettings(){
  preferences.begin("Settings", false);
  preferences.putBool("pac", PAndC);
  preferences.putInt("MQTTmax_current", MQTTmax_current);
  preferences.putInt("breaker", breaker);
  preferences.putInt("min_current", min_current);
  preferences.putBool("NegativeAmperage", NegativeAmperage); 
  preferences.putBool("ImpleraAdjust", ImpleraAdjust);  
  preferences.end();
  debuglog += "$saving chargeing settings to preferences$";
}

void RestoreChargeSettings(){
  PAndC = HIGH;
  MQTTmax_current = 16;
  breaker = 16;
  min_current = 6;
  NegativeAmperage = LOW;
  ImpleraAdjust = HIGH;
  preferences.begin("Settings", false);
  preferences.putBool("pac", PAndC);
  preferences.putInt("MQTTmax_current", MQTTmax_current);
  preferences.putInt("breaker", breaker);
  preferences.putInt("min_current", min_current);
  preferences.putBool("NegativeAmperage", NegativeAmperage); 
  preferences.putBool("ImpleraAdjust", ImpleraAdjust);
  preferences.end();
  debuglog += "$restoring chargeing settings in preferences$";
}

void CheckSettings(){
  preferences.begin("Settings", true);
  float tempCAL = preferences.getFloat("calibration", 27.7);
  debuglog += "$checking setting in preferences$";
  if(tempCAL > 1){
    SavedCalibration = HIGH;
    debuglog += "$found setting in preferences$";
  }
  preferences.end();
}

void GetSettings(){
  if(SavedCalibration == HIGH){
    preferences.begin("Settings", true);
    breaker = preferences.getInt("breaker", 16);
    calibration = preferences.getFloat("calibration", 27.7);
    timer = preferences.getLong("timer", 2000);
    timer1 = preferences.getLong("timer1", 1000);
    timer2 = preferences.getLong("timer2", 1000);
    timer3 = preferences.getLong("timer3", 10000);
    timer4 = preferences.getLong("timer4", 100);
    timer5 = preferences.getLong("timer5", 200000);
    timer6 = preferences.getLong("timer6", 30000);
    timer7 = preferences.getLong("timer7", 30000);
    timer8 = preferences.getLong("timer8", 200000);
    timer9 = preferences.getLong("timer9", 120000);
    timer10 = preferences.getLong("timer10", 5000);
    timer11 = preferences.getLong("timer11", 10000);
    timer12 = preferences.getLong("timer12", 100000);
    timer13 = preferences.getLong("timer13", 8000);
    timer14 = preferences.getLong("timer14", 10000);
    TimersFactorOff = preferences.getInt("TFO", 30);
    PAndC = preferences.getBool("pac", HIGH);
    NoWANPandC = preferences.getBool("nwpc", HIGH);
    MQTTmax_current = preferences.getInt("MQTTmax_current", 16);
    min_current = preferences.getInt("min_current", 6);
    NegativeAmperage = preferences.getBool("NegativeAmperage", LOW);
    ImpleraAdjust = preferences.getBool("ImpleraAdjust", HIGH);
    AutoUpdate = preferences.getBool("AutoUpdate", HIGH);
    LCDon = preferences.getBool("LCDon", HIGH);
    CTEnable = preferences.getInt("CTEnable", 0);
    DinamicsActive = preferences.getBool("DinamicsActive", 0);
    LoRa = preferences.getBool("LoRa", LOW);
    preferences.end();
    debuglog += "$read settings from preferences$";
  }
}

void SetCalibration(){
  preferences.begin("Settings", false);
  preferences.putFloat("calibration", calibration);
  preferences.end();
  debuglog += "$saving calibration to preferences$";
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
  preferences.putLong("timer13", timer13);
  preferences.putLong("timer14", timer14);
  preferences.end();
  debuglog += "$saving timers to preferences$";
}

void SetTimersFactor(){
  preferences.begin("Settings", false);
  preferences.putInt("TFO", TimersFactorOff);
  preferences.end();
  debuglog += "$saving timers factor off to preferences$";
}

void SetCTEnable(){
  preferences.begin("Settings", false);
  preferences.putInt("CTEnable", CTEnable);
  preferences.end();
  debuglog += "$saving active CTs to preferences$";
  ESP.restart();
}

void SetDinamicsEnable(){
  preferences.begin("Settings", false);
  preferences.putBool("DinamicsActive", DinamicsActive);
  preferences.end();
  debuglog += "$saving dinamics toggle to preferences$";
}

void SetBreaker(){
  preferences.begin("Settings", false);
  preferences.putInt("breaker", breaker);
  preferences.end();
  debuglog += "$saving breaker settings to preferences$";
}

void SetMaxMQTTCurrent(){
  preferences.begin("Settings", false);
  preferences.putInt("MQTTmax_current", MQTTmax_current);
  preferences.end();
  debuglog += "$saving MQTTmax_current settings to preferences$";
}

void SetMinCurrent(){
  preferences.begin("Settings", false);
  preferences.putInt("min_current", min_current);
  preferences.end();
  debuglog += "$saving min_current settings to preferences$";
}

void SetPAC(){
  preferences.begin("Settings", false);
  preferences.putBool("pac", PAndC);
  preferences.end();
  debuglog += "$saving PAC setting to preferences$";
}

void SetNWPC(){
  preferences.begin("Settings", false);
  preferences.putBool("nwpc", NoWANPandC);
  preferences.end();
  debuglog += "$saving NWPC setting to preferences$";
}

void SetNegativeAmperage(){
  preferences.begin("Settings", false);
  preferences.putBool("NegativeAmperage", NegativeAmperage);
  preferences.end();
  debuglog += "$saving NegativeAmperage setting to preferences$";
}

void SetAdjust(){
  preferences.begin("Settings", false);
  preferences.putBool("ImpleraAdjust", ImpleraAdjust);
  preferences.end();
  debuglog += "$saving Adjust setting to preferences$";
}

void SetAutoUpdate(){
  preferences.begin("Settings", false);
  preferences.putBool("AutoUpdate", AutoUpdate);
  preferences.end();
  debuglog += "$saving Adjust setting to preferences$";
}

void SetLCDon(){
  preferences.begin("Settings", false);
  preferences.putFloat("LCDon", LCDon);
  preferences.end();
  debuglog += "$saving LCD presence to preferences$";
}

void SetLoRa(){
  preferences.begin("Settings", false);
  preferences.putBool("LoRa", LoRa);
  preferences.end();
  debuglog += "$saving LoRa setting to preferences$";
  ESP.restart();
}

void DeleteSettings(){
  preferences.begin("Settings", false);
  preferences.clear();
  preferences.end();
  debuglog += "$deleting settings from preferences$";
  ESP.restart();
}

void SendSettingsF(){
  if(debug4 == 1){
    Serial.println("Send Settings function called");
  }
  if(SendSettings == HIGH){
    if(debug4 == 1){
      Serial.println("Sending Settings");
    }
    TempValue = "{";
    TempValue += "\"FW\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += FW_versionStr;
    TempValue += "\",";
    TempValue += "\"B\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += breaker;
    TempValue += "\",";
    TempValue += "\"MAXC\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += MQTTmax_current;
    TempValue += "\",";
    TempValue += "\"MINC\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += min_current;
    TempValue += "\",";
    TempValue += "\"C\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += calibration;
    TempValue += "\",";
    TempValue += "\"P&C\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += PAndC;
    TempValue += "\",";
    TempValue += "\"NWPC\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += NoWANPandC;
    TempValue += "\",";
    TempValue += "\"Neg_Amp\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += NegativeAmperage;
    TempValue += "\",";
    TempValue += "\"Adjust\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += ImpleraAdjust;
    TempValue += "\",";
    TempValue += "\"AutoUpdate\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += AutoUpdate;
    TempValue += "\",";
    TempValue += "\"LCD\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += LCDon;
    TempValue += "\",";
    TempValue += "\"TFO\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += TimersFactorOff;
    TempValue += "\",";
    TempValue += "\"CTE\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += CTEnable;
    TempValue += "\",";
    TempValue += "\"DA\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += DinamicsActive;
    TempValue += "\",";
    TempValue += "\"LoRa\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += LoRa;
    TempValue += "\"";
    TempValue += "}";
    
    topica = "";
      dynamicTopic = "";
    //  epochtimeTopic = getTime();
      dynamicTopic += prefixTopic;
      dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += SendSettingsTopic;
      topica = fullTopic.c_str();
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
  }
  SendSettings = LOW;
}

void SendTimersF(){
  if(debug4 == 1){
    Serial.println("Send Timers function called");
  }
  if(SendTimers == HIGH){
    if(debug4 == 1){
      Serial.println("Sending Timers");
    }
    TempValue = "{";
    TempValue += "\"T\"";
    TempValue += ":";
    TempValue += "[";
    TempValue += timer;
    TempValue += ",";
    TempValue += timer1;
    TempValue += ",";
    TempValue += timer2;
    TempValue += ",";
    TempValue += timer3;
    TempValue += ",";
    TempValue += timer4;
    TempValue += ",";
    TempValue += timer5;
    TempValue += ",";
    TempValue += timer6;
    TempValue += ",";
    TempValue += timer7;
    TempValue += ",";
    TempValue += timer8;
    TempValue += ",";
    TempValue += timer9;
    TempValue += ",";
    TempValue += timer10;
    TempValue += ",";
    TempValue += timer11;
    TempValue += ",";
    TempValue += timer12;
    TempValue += ",";
    TempValue += timer13;
    TempValue += ",";
    TempValue += timer14;
    TempValue += "]";
    TempValue += "}";
    
    topica = "";
      dynamicTopic = "";
    //  epochtimeTopic = getTime();
      dynamicTopic += prefixTopic;
      dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += SendTimersTopic;
      topica = fullTopic.c_str();
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
  }
  SendTimers = LOW;
}

void SendWiFiF(){
  if(debug4 == 1){
    Serial.println("Send WiFi function called");
  }
  if(SendWiFi == HIGH){
    if(debug4 == 1){
      Serial.println("Sending WiFi");
    }
    TempValue = "{";
    TempValue += "\"ssid\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += ssid;
    TempValue += "\",";
    TempValue += "\"pass\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += pass;
    TempValue += "\",";
    TempValue += "\"ssid1\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += ssid1;
    TempValue += "\",";
    TempValue += "\"pass1\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += pass1;
    TempValue += "\",";
    TempValue += "\"ip\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += ip;
    TempValue += "\",";
    TempValue += "\"gateway\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += gateway;
    TempValue += "\",";
    TempValue += "\"subnet\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += subnet;
    TempValue += "\",";
    TempValue += "\"mdns\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += mdns;
    TempValue += "\",";
    TempValue += "\"dhcpcheck\"";
    TempValue += ":";
    TempValue += "\"";
    TempValue += dhcpcheck;
    TempValue += "\"";
    TempValue += "}";
    
    topica = "";
      dynamicTopic = "";
    //  epochtimeTopic = getTime();
      dynamicTopic += prefixTopic;
      dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += SendWiFiTopic;
      topica = fullTopic.c_str();
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
  }
  SendWiFi = LOW;
}

void GetDebug(){
    preferences.begin("Debug", true);
    debug1 = preferences.getBool("debug1", 0);
    debug1_MQTT = preferences.getBool("debug1_MQTT", 1);
    debug2 = preferences.getBool("debug2", 1);
    debug2_MQTT = preferences.getBool("debug2_MQTT", 1);
    debug3 = preferences.getBool("debug3", 0);
    debug3_MQTT = preferences.getBool("debug3_MQTT", 1);
    debug4 = preferences.getBool("debug4", 0);
    debug4_MQTT = preferences.getBool("debug4_MQTT", 1);
    debug5 = preferences.getBool("debug5", 0);
    debug5_MQTT = preferences.getBool("debug5_MQTT", 1);
    debug6 = preferences.getBool("debug6", 0);
    debug6_MQTT = preferences.getBool("debug6_MQTT", 1);
    debug7 = preferences.getBool("debug7", 0);
    debug7_MQTT = preferences.getBool("debug7_MQTT", 1);
    debug8 = preferences.getBool("debug8", 0);
    debug8_MQTT = preferences.getBool("debug8_MQTT", 1);
    debug9 = preferences.getBool("debug9", 0);
    debug9_MQTT = preferences.getBool("debug9_MQTT", 1);
    debug10 = preferences.getBool("debug10", 0);
    debug10_MQTT = preferences.getBool("debug10_MQTT", 1);
    debug11 = preferences.getBool("debug11", 0);
    debug11_MQTT = preferences.getBool("debug11_MQTT", 0);
    debug12 = preferences.getBool("debug12", 0);
    debug12_MQTT = preferences.getBool("debug12_MQTT", 0);
    debug13 = preferences.getBool("debug13", 0);
    debug13_MQTT = preferences.getBool("debug13_MQTT", 0);
    debug14 = preferences.getBool("debug14", 0);
    debug14_MQTT = preferences.getBool("debug14_MQTT", 0);
    debug15 = preferences.getBool("debug15", 0);
    debug15_MQTT = preferences.getBool("debug15_MQTT", 0);
    debug16 = preferences.getBool("debug16", 0);
    debug16_MQTT = preferences.getBool("debug16_MQTT", 0);
    debug17 = preferences.getBool("debug17", 0);
    debug17_MQTT = preferences.getBool("debug17_MQTT", 0);
    debug18 = preferences.getBool("debug18", 0);
    debug18_MQTT = preferences.getBool("debug18_MQTT", 0);
    debug19 = preferences.getBool("debug19", 0);
    debug19_MQTT = preferences.getBool("debug19_MQTT", 0);
    debug20 = preferences.getBool("debug20", 0);
    debug20_MQTT = preferences.getBool("debug20_MQTT", 0);
    preferences.end();
    debuglog += "$read debug settings from preferences$";
}

void SetDebug(){
  preferences.begin("Debug", false);
  preferences.putBool("debug1", debug1);
  preferences.putBool("debug1_MQTT", debug1_MQTT);
  preferences.putBool("debug2", debug2);
  preferences.putBool("debug2_MQTT", debug2_MQTT);
  preferences.putBool("debug3", debug3);
  preferences.putBool("debug3_MQTT", debug3_MQTT);
  preferences.putBool("debug4", debug4);
  preferences.putBool("debug4_MQTT", debug4_MQTT);
  preferences.putBool("debug5", debug5);
  preferences.putBool("debug5_MQTT", debug5_MQTT);
  preferences.putBool("debug6", debug6);
  preferences.putBool("debug6_MQTT", debug6_MQTT);
  preferences.putBool("debug7", debug7);
  preferences.putBool("debug7_MQTT", debug7_MQTT);
  preferences.putBool("debug8", debug8);
  preferences.putBool("debug8_MQTT", debug8_MQTT);
  preferences.putBool("debug9", debug9);
  preferences.putBool("debug9_MQTT", debug9_MQTT);
  preferences.putBool("debug10", debug10);
  preferences.putBool("debug10_MQTT", debug10_MQTT);
  preferences.putBool("debug11", debug11);
  preferences.putBool("debug11_MQTT", debug11_MQTT);
  preferences.putBool("debug12", debug12);
  preferences.putBool("debug12_MQTT", debug12_MQTT);
  preferences.putBool("debug13", debug13);
  preferences.putBool("debug13_MQTT", debug13_MQTT);
  preferences.putBool("debug14", debug14);
  preferences.putBool("debug14_MQTT", debug14_MQTT);
  preferences.putBool("debug15", debug15);
  preferences.putBool("debug15_MQTT", debug15_MQTT);
  preferences.putBool("debug16", debug16);
  preferences.putBool("debug16_MQTT", debug16_MQTT);
  preferences.putBool("debug17", debug17);
  preferences.putBool("debug17_MQTT", debug17_MQTT);
  preferences.putBool("debug18", debug18);
  preferences.putBool("debug18_MQTT", debug18_MQTT);
  preferences.putBool("debug19", debug19);
  preferences.putBool("debug19_MQTT", debug19_MQTT);
  preferences.putBool("debug20", debug20);
  preferences.putBool("debug20_MQTT", debug20_MQTT);
  preferences.end();
  debuglog += "$saving debug seetings to preferences$";
}

void DeleteDebug(){
  preferences.begin("Debug", false);
  preferences.clear();
  preferences.end();
  debuglog += "$deleting debug settings from preferences$";
  ESP.restart();
}

void SendDebugF(){
  if(debug4 == 1){
    Serial.println("Send Debug Settings function called");
  }
  if(SendDebug == HIGH){
    if(debug4 == 1){
      Serial.println("Sending Debug Settings");
    }
    TempValue = "{";
    TempValue += "\"Debugs\"";
    TempValue += ":";
    TempValue += "[";
    TempValue += debug1;
    TempValue += ",";
    TempValue += debug1_MQTT;
    TempValue += ",";
    TempValue += debug2;
    TempValue += ",";
    TempValue += debug2_MQTT;
    TempValue += ",";
    TempValue += debug3;
    TempValue += ",";
    TempValue += debug3_MQTT;
    TempValue += ",";
    TempValue += debug4;
    TempValue += ",";
    TempValue += debug4_MQTT;
    TempValue += ",";
    TempValue += debug5;
    TempValue += ",";
    TempValue += debug5_MQTT;
    TempValue += ",";
    TempValue += debug6;
    TempValue += ",";
    TempValue += debug6_MQTT;
    TempValue += ",";
    TempValue += debug7;
    TempValue += ",";
    TempValue += debug7_MQTT;
    TempValue += ",";
    TempValue += debug8;
    TempValue += ",";
    TempValue += debug8_MQTT;
    TempValue += ",";
    TempValue += debug9;
    TempValue += ",";
    TempValue += debug9_MQTT;
    TempValue += ",";
    TempValue += debug10;
    TempValue += ",";
    TempValue += debug10_MQTT;
    TempValue += ",";
    TempValue += debug11;
    TempValue += ",";
    TempValue += debug11_MQTT;
    TempValue += ",";
    TempValue += debug12;
    TempValue += ",";
    TempValue += debug12_MQTT;
    TempValue += ",";
    TempValue += debug13;
    TempValue += ",";
    TempValue += debug13_MQTT;
    TempValue += ",";
    TempValue += debug14;
    TempValue += ",";
    TempValue += debug14_MQTT;
    TempValue += ",";
    TempValue += debug15;
    TempValue += ",";
    TempValue += debug15_MQTT;
    TempValue += ",";
    TempValue += debug16;
    TempValue += ",";
    TempValue += debug16_MQTT;
    TempValue += ",";
    TempValue += debug17;
    TempValue += ",";
    TempValue += debug17_MQTT;
    TempValue += ",";
    TempValue += debug18;
    TempValue += ",";
    TempValue += debug18_MQTT;
    TempValue += ",";
    TempValue += debug19;
    TempValue += ",";
    TempValue += debug19_MQTT;
    TempValue += ",";
    TempValue += debug20;
    TempValue += ",";
    TempValue += debug20_MQTT;
    TempValue += "]";
    TempValue += "}";
    
    topica = "";
      dynamicTopic = "";
    //  epochtimeTopic = getTime();
      dynamicTopic += prefixTopic;
      dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += SendDebugTopic;
      topica = fullTopic.c_str();
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
  }
  SendDebug = LOW;
}

void TranslateDebugF(){
  if(TranslateDebug == HIGH){
    int charNum = debuglog.length();
    if(charNum == 40){
      debug1 = debuglog.substring(0,1).toInt();
      debug1_MQTT = debuglog.substring(1,2).toInt();
      debug2 = debuglog.substring(2,3).toInt();
      debug2_MQTT = debuglog.substring(3,4).toInt();
      debug3 = debuglog.substring(4,5).toInt();
      debug3_MQTT = debuglog.substring(5,6).toInt();
      debug4 = debuglog.substring(6,7).toInt();
      debug4_MQTT = debuglog.substring(7,8).toInt();
      debug5 = debuglog.substring(8,9).toInt();
      debug5_MQTT = debuglog.substring(9,10).toInt();
      debug6 = debuglog.substring(10,11).toInt();
      debug6_MQTT = debuglog.substring(11,12).toInt();
      debug7 = debuglog.substring(12,13).toInt();
      debug7_MQTT = debuglog.substring(13,14).toInt();
      debug8 = debuglog.substring(14,15).toInt();
      debug8_MQTT = debuglog.substring(15,16).toInt();
      debug9 = debuglog.substring(16,17).toInt();
      debug9_MQTT = debuglog.substring(17,18).toInt();
      debug10 = debuglog.substring(18,19).toInt();
      debug10_MQTT = debuglog.substring(19,20).toInt();
      debug11 = debuglog.substring(20,21).toInt();
      debug11_MQTT = debuglog.substring(21,22).toInt();
      debug12 = debuglog.substring(22,23).toInt();
      debug12_MQTT = debuglog.substring(23,24).toInt();
      debug13 = debuglog.substring(24,25).toInt();
      debug13_MQTT = debuglog.substring(25,26).toInt();
      debug14 = debuglog.substring(26,27).toInt();
      debug14_MQTT = debuglog.substring(27,28).toInt();
      debug15 = debuglog.substring(28,29).toInt();
      debug15_MQTT = debuglog.substring(29,30).toInt();
      debug16 = debuglog.substring(30,31).toInt();
      debug16_MQTT = debuglog.substring(31,32).toInt();
      debug17 = debuglog.substring(32,33).toInt();
      debug17_MQTT = debuglog.substring(33,34).toInt();
      debug18 = debuglog.substring(34,35).toInt();
      debug18_MQTT = debuglog.substring(35,36).toInt();
      debug19 = debuglog.substring(36,37).toInt();
      debug19_MQTT = debuglog.substring(37,38).toInt();
      debug20 = debuglog.substring(38,39).toInt();
      debug20_MQTT = debuglog.substring(39).toInt();
      SetDebug();
    }
    TranslateDebug == LOW;
  }
}


void setup() {
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(115200);

//  Serial2.println("$F0 0");
//  delay(50);
////  Serial.println("$FP 0 0       ");
////  delay(50);
////  Serial.println("$FP 6 0       ");
////  delay(50);
////  Serial.println("$FP 11 0      ");
////  Serial.flush();
////  delay(60);
////  Serial.println("$FP 0 1       ");
////  delay(50);
////  Serial.println("$FP 6 1       ");
////  delay(50);
////  Serial.println("$FP 11 1      ");
//  LCDEraseFlag = HIGH;
//  EraseLCDText();
  
  delay(500);
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
  delay(200);
  
  
  initSPIFFS();

  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, HIGH);


  uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);
  snprintf(charid, 23, "%04X%08X", chip, (uint32_t)chipid);
  //Serial.println(charid);
  delay(200);
  idTopic = charid;
  if(debug2 == 1){
    Serial.println(idTopic);
  }
  deviceName += idTopic;

  chardeviceName = deviceName.c_str();


  sub_BreakerTopic += prefixTopic;
  sub_BreakerTopic += idTopic;
  sub_BreakerTopic += "/set_breaker";
  charBreaker = sub_BreakerTopic.c_str();
  sub_StatusTopic += prefixTopic;
  sub_StatusTopic += idTopic;
  sub_StatusTopic += "/get_status";
  charStatus = sub_StatusTopic.c_str();
  sub_UpdateTopic += prefixTopic;
  sub_UpdateTopic += idTopic;
  sub_UpdateTopic += "/set_update";
  charUpdate = sub_UpdateTopic.c_str();
  sub_UpdateSpiffsTopic += prefixTopic;
  sub_UpdateSpiffsTopic += idTopic;
  sub_UpdateSpiffsTopic += "/set_update_spiffs";
  charUpdateSpiffs = sub_UpdateSpiffsTopic.c_str();
  sub_EnableTopic += prefixTopic;
  sub_EnableTopic += idTopic;
  sub_EnableTopic += "/set_enable";
  charEnable = sub_EnableTopic.c_str();
  sub_ChargeTimerTopic += prefixTopic;
  sub_ChargeTimerTopic += idTopic;
  sub_ChargeTimerTopic += "/set_t_limit";
  charChargeTimer = sub_ChargeTimerTopic.c_str();
  sub_EnergyLimitTopic += prefixTopic;
  sub_EnergyLimitTopic += idTopic;
  sub_EnergyLimitTopic += "/set_e_limit";
  charEnergyLimit = sub_EnergyLimitTopic.c_str();
  sub_CurrentLimitTopic += prefixTopic;
  sub_CurrentLimitTopic += idTopic;
  sub_CurrentLimitTopic += "/set_c_limit";
  charCurrentLimit = sub_CurrentLimitTopic.c_str();
  sub_CurrentMinLimitTopic += prefixTopic;
  sub_CurrentMinLimitTopic += idTopic;
  sub_CurrentMinLimitTopic += "/set_c_min_limit";
  charCurrentMinLimit = sub_CurrentMinLimitTopic.c_str();
  sub_CurrentCalibrationTopic += prefixTopic;
  sub_CurrentCalibrationTopic += idTopic;
  sub_CurrentCalibrationTopic += "/set_calibration";
  charCurrentCalibration = sub_CurrentCalibrationTopic.c_str();

  vTaskDelay(20);
  
  sub_DebugTopic += prefixTopic;
  sub_DebugTopic += idTopic;
  sub_DebugTopic += "/set_debug";
  charDebug = sub_DebugTopic.c_str();
  sub_RAPITopic += prefixTopic;
  sub_RAPITopic += idTopic;
  sub_RAPITopic += "/rapi_request";
  charRAPI = sub_RAPITopic.c_str();
  sub_TimerTopic += prefixTopic;
  sub_TimerTopic += idTopic;
  sub_TimerTopic += "/set_timer";
  charTimer = sub_TimerTopic.c_str();
  sub_Timer1Topic += prefixTopic;
  sub_Timer1Topic += idTopic;
  sub_Timer1Topic += "/set_timer1";
  charTimer1 = sub_Timer1Topic.c_str();
  sub_Timer2Topic += prefixTopic;
  sub_Timer2Topic += idTopic;
  sub_Timer2Topic += "/set_timer2";
  charTimer2 = sub_Timer2Topic.c_str();
  sub_Timer3Topic += prefixTopic;
  sub_Timer3Topic += idTopic;
  sub_Timer3Topic += "/set_timer3";
  charTimer3 = sub_Timer3Topic.c_str();
  sub_Timer4Topic += prefixTopic;
  sub_Timer4Topic += idTopic;
  sub_Timer4Topic += "/set_timer4";
  charTimer4 = sub_Timer4Topic.c_str();
  sub_Timer5Topic += prefixTopic;
  sub_Timer5Topic += idTopic;
  sub_Timer5Topic += "/set_timer5";
  charTimer5 = sub_Timer5Topic.c_str();
  sub_Timer6Topic += prefixTopic;
  sub_Timer6Topic += idTopic;
  sub_Timer6Topic += "/set_timer6";
  charTimer6 = sub_Timer6Topic.c_str();
  sub_Timer7Topic += prefixTopic;
  sub_Timer7Topic += idTopic;
  sub_Timer7Topic += "/set_timer7";
  charTimer7 = sub_Timer7Topic.c_str();
  sub_Timer8Topic += prefixTopic;
  sub_Timer8Topic += idTopic;
  sub_Timer8Topic += "/set_timer8";
  charTimer8 = sub_Timer8Topic.c_str();
  sub_Timer9Topic += prefixTopic;
  sub_Timer9Topic += idTopic;
  sub_Timer9Topic += "/set_timer9";
  charTimer9 = sub_Timer9Topic.c_str();
  sub_Timer10Topic += prefixTopic;
  sub_Timer10Topic += idTopic;
  sub_Timer10Topic += "/set_timer10";
  charTimer10 = sub_Timer10Topic.c_str();
  sub_Timer11Topic += prefixTopic;
  sub_Timer11Topic += idTopic;
  sub_Timer11Topic += "/set_timer11";
  charTimer11 = sub_Timer11Topic.c_str();
  sub_Timer12Topic += prefixTopic;
  sub_Timer12Topic += idTopic;
  sub_Timer12Topic += "/set_timer12";
  charTimer12 = sub_Timer12Topic.c_str();
  sub_Timer13Topic += prefixTopic;
  sub_Timer13Topic += idTopic;
  sub_Timer13Topic += "/set_timer13";
  charTimer13 = sub_Timer13Topic.c_str();
  sub_Timer14Topic += prefixTopic;
  sub_Timer14Topic += idTopic;
  sub_Timer14Topic += "/set_timer14";
  charTimer14 = sub_Timer14Topic.c_str();

  vTaskDelay(20);


  sub_TFOTopic += prefixTopic;
  sub_TFOTopic += idTopic;
  sub_TFOTopic += "/set_TFO";
  charTFO = sub_TFOTopic.c_str();
  sub_PlugAndChargeTopic += prefixTopic;
  sub_PlugAndChargeTopic += idTopic;
  sub_PlugAndChargeTopic += "/set_plugandcharge";
  charPlugAndCharge = sub_PlugAndChargeTopic.c_str();
  sub_NoWANPandCTopic += prefixTopic;
  sub_NoWANPandCTopic += idTopic;
  sub_NoWANPandCTopic += "/set_nwpc";
  charNoWANPandC = sub_NoWANPandCTopic.c_str();
  sub_NegativeAmperageTopic += prefixTopic;
  sub_NegativeAmperageTopic += idTopic;
  sub_NegativeAmperageTopic += "/set_neg_amp";
  charNegativeAmperage = sub_NegativeAmperageTopic.c_str();
  sub_AdjustTopic += prefixTopic;
  sub_AdjustTopic += idTopic;
  sub_AdjustTopic += "/set_adjust";
  charAdjust = sub_AdjustTopic.c_str();
  sub_AutoUpdateTopic += prefixTopic;
  sub_AutoUpdateTopic += idTopic;
  sub_AutoUpdateTopic += "/set_autoupdate";
  charAutoUpdate = sub_AutoUpdateTopic.c_str();
  sub_GetSettingsTopic += prefixTopic;
  sub_GetSettingsTopic += idTopic;
  sub_GetSettingsTopic += "/get_settings";
  charGetSettings = sub_GetSettingsTopic.c_str();
  sub_GetTimersTopic += prefixTopic;
  sub_GetTimersTopic += idTopic;
  sub_GetTimersTopic += "/get_timers";
  charGetTimers = sub_GetTimersTopic.c_str();
  sub_DeleteSettingsTopic += prefixTopic;
  sub_DeleteSettingsTopic += idTopic;
  sub_DeleteSettingsTopic += "/delete_settings";
  charDeleteSettings = sub_DeleteSettingsTopic.c_str();
  sub_DeleteWifiTopic += prefixTopic;
  sub_DeleteWifiTopic += idTopic;
  sub_DeleteWifiTopic += "/delete_wifi";
  charDeleteWifi = sub_DeleteWifiTopic.c_str();
  sub_GetWiFiTopic += prefixTopic;
  sub_GetWiFiTopic += idTopic;
  sub_GetWiFiTopic += "/get_wifi";
  charGetWiFi = sub_GetWiFiTopic.c_str();
  sub_GetDebugTopic += prefixTopic;
  sub_GetDebugTopic += idTopic;
  sub_GetDebugTopic += "/get_debug";
  charGetDebug = sub_GetDebugTopic.c_str();
  sub_LCDonTopic += prefixTopic;
  sub_LCDonTopic += idTopic;
  sub_LCDonTopic += "/set_lcd";
  charLCDon = sub_LCDonTopic.c_str();
  sub_LCDEraseTopic += prefixTopic;
  sub_LCDEraseTopic += idTopic;
  sub_LCDEraseTopic += "/erase_lcd";
  charLCDErase = sub_LCDEraseTopic.c_str();
  sub_CTEnableTopic += prefixTopic;
  sub_CTEnableTopic += idTopic;
  sub_CTEnableTopic += "/set_cte";
  charCTEnable = sub_CTEnableTopic.c_str();
  sub_DinamicsEnableTopic += prefixTopic;
  sub_DinamicsEnableTopic += idTopic;
  sub_DinamicsEnableTopic += "/set_dinamics";
  charDinamicsEnable = sub_DinamicsEnableTopic.c_str();
  sub_LoRaTopic += prefixTopic;
  sub_LoRaTopic += idTopic;
  sub_LoRaTopic += "/set_lora";
  charLoRa = sub_LoRaTopic.c_str();



  vTaskDelay(20);
  delay(200);



  CheckWiFiCredentials();
  if(SavedWiFi == LOW){
    // Load values saved in SPIFFS
    ssid = readFile(SPIFFS, ssidPath);
    if(debug2 == 1){
      Serial.println(ssid);
    }
    debuglog += "$";
    debuglog += "ssid=";
    debuglog += ssid;
    debuglog += "$";
    pass = readFile(SPIFFS, passPath);
    if(debug2 == 1){
      Serial.println(pass);
    }
    debuglog += "$";
    debuglog += "pass=";
    debuglog += pass;
    debuglog += "$";
    ip = readFile(SPIFFS, ipPath);
    if(debug2 == 1){
      Serial.println(ip);
    }
    debuglog += "$";
    debuglog += "ip=";
    debuglog += ip;
    debuglog += "$";
    gateway = readFile(SPIFFS, gatewayPath);
    if(debug2 == 1){
      Serial.println(gateway);
    }
    debuglog += "$";
    debuglog += "gateway=";
    debuglog += gateway;
    debuglog += "$";
    subnet = readFile(SPIFFS, subnetPath);
    if(debug2 == 1){
      Serial.println(subnet);
    }
    debuglog += "$";
    debuglog += "subnet=";
    debuglog += subnet;
    debuglog += "$";
    mdnsdotlocalurl = readFile(SPIFFS, mdnsPath);
    if(debug2 == 1){
      Serial.println(mdnsdotlocalurl);
    }
    debuglog += "$";
    debuglog += "mdnsdotlocalurl=";
    debuglog += mdnsdotlocalurl;
    debuglog += "$";
    dhcpcheck = readFile(SPIFFS, dhcpcheckPath);
    if(debug2 == 1){
      Serial.println(dhcpcheck);
    }
    debuglog += "$";
    debuglog += "dhcpcheck=";
    debuglog += dhcpcheck;
    debuglog += "$";
  }else{
    GetWiFiCredentials();
  }

  CheckSettings();
  GetSettings();
  GetDebug();
  GetRuntimeSettings();


  if(LoRa == HIGH){
    Serial2.begin(115200, SERIAL_8N1, RX2LORA, TX2LORA);
    timer13 = timer13*1.5;
    delay(50);
    Serial.println("LoRa communication enabled");
  }else{
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
    delay(50);
    Serial.println("Cable communication enabled");
  }

  delay(200);
  

  {
    auto cfg = esp32FOTA.getConfig();
    cfg.name = firmware_name;
    cfg.manifest_url = FOTA_URL;
    cfg.sem = SemverClass(firmware_version_major, firmware_version_minor, firmware_version_patch);
    cfg.check_sig = check_signature;
    cfg.unsafe = disable_security;
    // cfg.root_ca = MyRootCA;
    // cfg.pub_key = MyRSAKey;
    esp32FOTA.setConfig(cfg);
    esp32FOTA.setCertFileSystem(nullptr);
  }





      if (initWiFi()) {
        if(debug2 == 1){
          Serial.println("InitWiFi = HIGH");
        }
      
        // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/Implera-Dynamics.html.jgz", "text/html", false);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });
    server.serveStatic("/", SPIFFS, "/");
    server.on("/power", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(power, 1));
    });
    server.on("/current", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(charge_current, 1));
    });
    server.on("/current1", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(average1, 1));
    });
    server.on("/current2", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(average2, 1));
    });
    server.on("/current3", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(average3, 1));
    });
    server.on("/powerbutton", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(PowerStatus));
    });
    server.on("/setpower", HTTP_GET, [](AsyncWebServerRequest *request){
      if(PowerStatus != 2){
        if(tmp == 2){
          tmp = 3;
          SetChargeFlag = HIGH;
        }else{
          tmp = 2;
          SetChargeFlag = HIGH;
        }
      }
      request->send(200, "text/txt", "OK");
    });
    server.on("/getNegative", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(NegAmpOnceFlag));
    });
    server.on("/setNegative", HTTP_GET, [](AsyncWebServerRequest *request){
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          // HTTP POST ssid value
          const char* PARAM_INPUT_1 = "state";                  // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_1) {
            NegAmpOnceFlag = p->value().toInt();
//            Serial.print("Auto Update set to: ");
//            Serial.println(AutoUpdate);
          }
        }
      }
      request->send(200, "text/html", "OK");
    });

    // Route for charging settings /Charging-Settings.html web page
    server.on("/charging-settings", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/Charging-Settings.html.jgz", "text/html", false);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });
    server.on("/pandc", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(PAndC));
    });
    server.on("/max", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(MQTTmax_current));
    });
    server.on("/breaker", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(breaker));
    });
    server.on("/min", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(min_current));
    });
    server.on("/neg_amp", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(NegativeAmperage));
    });
    server.on("/adjust", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(ImpleraAdjust));
    });
    server.on("/setCharge", HTTP_POST, [](AsyncWebServerRequest *request){
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          // HTTP POST ssid value
          const char* PARAM_INPUT_1 = "PandC";                  // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_1) {
            PAndC = p->value().toInt();
            if(debug5 == 1){
              Serial.print("PandC set to: ");
              Serial.println(PAndC);
            }
            // Write file to save value
//            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          vTaskDelay(20);
          // HTTP POST pass value
          const char* PARAM_INPUT_2 = "rVMax";                 // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_2) {
            MQTTmax_current = p->value().toInt();
            if(debug5 == 1){
              Serial.print("Max charging current set to: ");
              Serial.println(MQTTmax_current);
            }
            // Write file to save value
//            writeFile(SPIFFS, passPath, pass.c_str());
          }
          vTaskDelay(20);
          // HTTP POST ip value
          const char* PARAM_INPUT_3 = "rVB";                   // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_3) {
            breaker = p->value().toInt();
            if(debug5 == 1){
              Serial.print("Breakers set to: ");
              Serial.println(breaker);
            }
//            writeFile(SPIFFS, ipPath, ip.c_str());            // Write file to save value
          }
          vTaskDelay(20);
          // HTTP POST gateway value
          const char* PARAM_INPUT_4 = "rVMin";              // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_4) {
            min_current = p->value().toInt();
            if(debug5 == 1){
              Serial.print("Min charging current set to: ");
              Serial.println(min_current);
            }
//            writeFile(SPIFFS, gatewayPath, gateway.c_str());          // Write file to save value
          }
          vTaskDelay(20);
          // HTTP POST subnet value
          const char* PARAM_INPUT_5 = "NegAmp";               // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_5) {
            NegativeAmperage = p->value().toInt();
            if(debug5 == 1){
              Serial.print("Negative Amperage set to: ");
              Serial.println(NegativeAmperage);
            }
//            writeFile(SPIFFS, subnetPath, subnet.c_str());            // Write file to save value
          }
          vTaskDelay(20);
          // HTTP POST subnet value
          const char* PARAM_INPUT_6 = "Adj";               // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_6) {
            ImpleraAdjust = p->value().toInt();
            if(debug5 == 1){
              Serial.print("Adjust set to: ");
              Serial.println(ImpleraAdjust);
            }
//            writeFile(SPIFFS, subnetPath, subnet.c_str());            // Write file to save value
          }
        }
      }
      SetChargeSettingsFlag = HIGH;
      request->send(200, "text/html", "<h1>Charging settings saved</h1>");
    }); 
    server.on("/restoreCharge", HTTP_GET, [](AsyncWebServerRequest * request){
      RestoreChargeSettingsFlag = HIGH;
      request->send(200, "text/html", "<h1>Restoring charging settings.</h1>");
    });
    
    // Route for wifi settings /WiFi-Settings.html web page
    server.on("/wifi-settings", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/WiFi-Settings.html.jgz", "text/html", false);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });
    server.on("/SSID", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", ssid);
    });
    server.on("/PASS", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", pass);
    });
    server.on("/DHCP", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", dhcpcheck);
    });
    server.on("/IP", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", ip);
    });
    server.on("/GWIP", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", gateway);
    });
    server.on("/setWiFi", HTTP_POST, [](AsyncWebServerRequest *request){
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          // HTTP POST ssid value
          const char* PARAM_INPUT_1 = "ssid";                  // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
//            Serial.print("SSID set to: ");
//            Serial.println(ssid);
            // Write file to save value
//            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          vTaskDelay(50);
          // HTTP POST pass value
          const char* PARAM_INPUT_2 = "pass";                 // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
//            Serial.print("Password set to: ");
//            Serial.println(pass);
            // Write file to save value
//            writeFile(SPIFFS, passPath, pass.c_str());
          }
          vTaskDelay(50);
          // HTTP POST ip value
          const char* PARAM_INPUT_3 = "ip";                   // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_3) {
            dhcpcheck = "off";
//            writeFile(SPIFFS, dhcpcheckPath, "off");          //dhcp unchecked . if we recieve post with ip set dhcpcheck.txt file to off
            ip = p->value().c_str();
//            Serial.print("IP Address set to: ");
//            Serial.println(ip);
//            writeFile(SPIFFS, ipPath, ip.c_str());            // Write file to save value
          }
          vTaskDelay(50);
          // HTTP POST gateway value
          const char* PARAM_INPUT_4 = "gwip";              // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
//            Serial.print("gateway Address set to: ");
//            Serial.println(gateway);
//            writeFile(SPIFFS, gatewayPath, gateway.c_str());          // Write file to save value
          }
          vTaskDelay(50);
          // HTTP POST subnet value
          const char* PARAM_INPUT_5 = "subnet";               // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_5) {
            subnet = p->value().c_str();
//            Serial.print("subnet Address set to: ");
//            Serial.println(subnet);
//            writeFile(SPIFFS, subnetPath, subnet.c_str());            // Write file to save value
          }
          vTaskDelay(50);
          // HTTP POST dhcp value on
          const char* PARAM_INPUT_6 = "dhcp";                // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_6) {
            dhcpcheck = p->value().c_str();
//            Serial.print("dhcpcheck set to: ");
//            Serial.println(dhcpcheck);
//            writeFile(SPIFFS, dhcpcheckPath, dhcpcheck.c_str());            // Write file to save value
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      if (dhcpcheck == "on") {
        ip = "dhcp ip adress";
      }
      SetWiFiCredsFlag = HIGH;
      request->send(200, "text/html", "<h1>Done. ESP will restart shortly.</h1>");
    }); 
    server.on("/restoreWiFi", HTTP_GET, [](AsyncWebServerRequest * request){
      DeleteWiFiCredsFlag = HIGH;
      request->send(200, "text/html", "<h1>Deleting stored WiFi credentials<br>Done.<br>ESP restart,<br>connect to AP access point Dynamics-" + idTopic + "<br>to configure wifi settings again<br><a href=\"http://192.168.4.1\">http://192.168.4.1</a></h1>");
    });
    
    // Route for system settings /System-Settings.html web page
    server.on("/system-settings", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/System-Settings.html.jgz", "text/html", false);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });
    server.on("/AutoUpdate", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(AutoUpdate));
    });
    server.on("/setAU", HTTP_GET, [](AsyncWebServerRequest *request){
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          // HTTP POST ssid value
          const char* PARAM_INPUT_1 = "state";                  // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_1) {
            AutoUpdate = p->value().toInt();
//            Serial.print("Auto Update set to: ");
//            Serial.println(AutoUpdate);
          }
        }
      }
      SetAutoUpdateFlag = HIGH;
      request->send(200, "text/html", "<h1>OK</h1>");
    });
    server.on("/updatefw", HTTP_GET, [](AsyncWebServerRequest * request){
      UpdateStart = HIGH;
      request->send(200, "text/html", "<h1>Updating firmware.<br>Wait for a few minutes.<br>Implera Dynamics will restart when done.<br>Refresh the webpage after few minutes</h1>");
    });
    server.on("/updatewp", HTTP_GET, [](AsyncWebServerRequest * request){
      UpdateSpiffs = HIGH;
      request->send(200, "text/html", "<h1>Updating webpage.<br>Wait for a few minutes.<br>Implera Dynamics will restart when done.<br>Refresh the webpage after few minutes</h1>");
    });

    // Route for about /About.html web page
    server.on("/about", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/About.html.jgz", "text/html", false);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });
    server.on("/SN", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", idTopic);
    });
    server.on("/FW", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", FW_versionStr);
    });


/*
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
        debuglog += "$";
        debuglog += "An Error has occurred while mounting SPIFFS";
        debuglog += "$";
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

        */

    server.begin();
  }
  else {
    // Connect to Wi-Fi network with SSID and password == setup an AP AccessPoint for wifi direct connect
    WiFi.disconnect(true); //Disable STA
    if(debug2 == 1){
      Serial.println("Setting AP (Access Point)");
    }
    // NULL sets an open Access Point
    String broadcastintheair = String("Dinamics-") + charid;  // want a unique broadcast id for each device
    WiFi.softAP(broadcastintheair.c_str(), NULL);                                        // i do not know, strings and chars thing drive me nuts
                                                                                         // i have seen all errors possible, getting this working ;-)

    IPAddress IP = WiFi.softAPIP();
    if(debug2 == 1){
      Serial.print("AP IP address: ");
      Serial.println(IP);
    }
    debuglog += "$";
    debuglog += "IP address=";
    debuglog += IP;
    debuglog += "$";

    String addressIP = WiFi.localIP().toString();


        // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/Implera-Dynamics.html.jgz", "text/html", false);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });
    server.serveStatic("/", SPIFFS, "/");
    server.on("/power", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(power, 1));
    });
    server.on("/current", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(charge_current, 1));
    });
    server.on("/current1", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(average1, 1));
    });
    server.on("/current2", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(average2, 1));
    });
    server.on("/current3", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(average3, 1));
    });
    server.on("/powerbutton", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(PowerStatus));
    });
    server.on("/setpower", HTTP_GET, [](AsyncWebServerRequest *request){
      if(tmp == 2){
        tmp = 3;
        SetChargeFlag = HIGH;
      }else{
        tmp = 2;
        SetChargeFlag = HIGH;
      }
      request->send(200, "text/txt", "OK");
    });
    server.on("/getNegative", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(NegAmpOnceFlag));
    });
    server.on("/setNegative", HTTP_GET, [](AsyncWebServerRequest *request){
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          // HTTP POST ssid value
          const char* PARAM_INPUT_1 = "state";                  // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_1) {
            NegAmpOnceFlag = p->value().toInt();
//            Serial.print("Auto Update set to: ");
//            Serial.println(AutoUpdate);
          }
        }
      }
      request->send(200, "text/html", "OK");
    });

    // Route for charging settings /Charging-Settings.html web page
    server.on("/charging-settings", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/Charging-Settings.html.jgz", "text/html", false);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });
    server.on("/pandc", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(PAndC));
    });
    server.on("/max", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(MQTTmax_current));
    });
    server.on("/breaker", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(breaker));
    });
    server.on("/min", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(min_current));
    });
    server.on("/neg_amp", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(NegativeAmperage));
    });
    server.on("/adjust", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(ImpleraAdjust));
    });
    server.on("/setCharge", HTTP_POST, [](AsyncWebServerRequest *request){
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          // HTTP POST ssid value
          const char* PARAM_INPUT_1 = "PandC";                  // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_1) {
            PAndC = p->value().toInt();
//            Serial.print("PandC set to: ");
//            Serial.println(PAndC);
            // Write file to save value
//            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          vTaskDelay(20);
          // HTTP POST pass value
          const char* PARAM_INPUT_2 = "rVMax";                 // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_2) {
            MQTTmax_current = p->value().toInt();
//            Serial.print("Max charging current set to: ");
//            Serial.println(MQTTmax_current);
            // Write file to save value
//            writeFile(SPIFFS, passPath, pass.c_str());
          }
          vTaskDelay(20);
          // HTTP POST ip value
          const char* PARAM_INPUT_3 = "rVB";                   // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_3) {
            breaker = p->value().toInt();
//            Serial.print("Breakers set to: ");
//            Serial.println(breaker);
//            writeFile(SPIFFS, ipPath, ip.c_str());            // Write file to save value
          }
          vTaskDelay(20);
          // HTTP POST gateway value
          const char* PARAM_INPUT_4 = "rVMin";              // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_4) {
            min_current = p->value().toInt();
//            Serial.print("Min charging current set to: ");
//            Serial.println(min_current);
//            writeFile(SPIFFS, gatewayPath, gateway.c_str());          // Write file to save value
          }
          vTaskDelay(20);
          // HTTP POST subnet value
          const char* PARAM_INPUT_5 = "NegAmp";               // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_5) {
            NegativeAmperage = p->value().toInt();
//            Serial.print("Negative Amperage set to: ");
//            Serial.println(NegativeAmperage);
//            writeFile(SPIFFS, subnetPath, subnet.c_str());            // Write file to save value
          }
          vTaskDelay(20);
          // HTTP POST subnet value
          const char* PARAM_INPUT_6 = "Adj";               // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_6) {
            ImpleraAdjust = p->value().toInt();
//            Serial.print("Adjust set to: ");
//            Serial.println(ImpleraAdjust);
//            writeFile(SPIFFS, subnetPath, subnet.c_str());            // Write file to save value
          }
        }
      }
      SetChargeSettingsFlag = HIGH;
      request->send(200, "text/html", "<h1>Charging settings saved</h1>");
    }); 
    server.on("/restoreCharge", HTTP_GET, [](AsyncWebServerRequest * request){
      RestoreChargeSettingsFlag = HIGH;
      request->send(200, "text/html", "<h1>Restoring charging settings.</h1>");
    });
    
    // Route for wifi settings /WiFi-Settings.html web page
    server.on("/wifi-settings", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/WiFi-Settings.html.jgz", "text/html", false);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });
    server.on("/SSID", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", ssid);
    });
    server.on("/PASS", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", pass);
    });
    server.on("/DHCP", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", dhcpcheck);
    });
    server.on("/IP", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", ip);
    });
    server.on("/GWIP", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", ip);
    });
    server.on("/setWiFi", HTTP_POST, [](AsyncWebServerRequest *request){
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          // HTTP POST ssid value
          const char* PARAM_INPUT_1 = "ssid";                  // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
//            Serial.print("SSID set to: ");
//            Serial.println(ssid);
            // Write file to save value
//            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          vTaskDelay(50);
          // HTTP POST pass value
          const char* PARAM_INPUT_2 = "pass";                 // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
//            Serial.print("Password set to: ");
//            Serial.println(pass);
            // Write file to save value
//            writeFile(SPIFFS, passPath, pass.c_str());
          }
          vTaskDelay(50);
          // HTTP POST ip value
          const char* PARAM_INPUT_3 = "ip";                   // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_3) {
            dhcpcheck = "off";
//            writeFile(SPIFFS, dhcpcheckPath, "off");          //dhcp unchecked . if we recieve post with ip set dhcpcheck.txt file to off
            ip = p->value().c_str();
//            Serial.print("IP Address set to: ");
//            Serial.println(ip);
//            writeFile(SPIFFS, ipPath, ip.c_str());            // Write file to save value
          }
          vTaskDelay(50);
          // HTTP POST gateway value
          const char* PARAM_INPUT_4 = "gwip";              // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_4) {
            gateway = p->value().c_str();
//            Serial.print("gateway Address set to: ");
//            Serial.println(gateway);
//            writeFile(SPIFFS, gatewayPath, gateway.c_str());          // Write file to save value
          }
          vTaskDelay(50);
          // HTTP POST subnet value
          const char* PARAM_INPUT_5 = "subnet";               // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_5) {
            subnet = p->value().c_str();
//            Serial.print("subnet Address set to: ");
//            Serial.println(subnet);
//            writeFile(SPIFFS, subnetPath, subnet.c_str());            // Write file to save value
          }
          vTaskDelay(50);
          // HTTP POST dhcp value on
          const char* PARAM_INPUT_6 = "dhcp";                // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_6) {
            dhcpcheck = p->value().c_str();
//            Serial.print("dhcpcheck set to: ");
//            Serial.println(dhcpcheck);
//            writeFile(SPIFFS, dhcpcheckPath, dhcpcheck.c_str());            // Write file to save value
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      if (dhcpcheck == "on") {
        ip = "dhcp ip adress";
      }
      SetWiFiCredsFlag = HIGH;
      request->send(200, "text/html", "<h1>Done. ESP will restart shortly.</h1>");
    }); 
    server.on("/restoreWiFi", HTTP_GET, [](AsyncWebServerRequest * request){
      DeleteWiFiCredsFlag = HIGH;
      request->send(200, "text/html", "<h1>Deleting stored WiFi credentials<br>Done.<br>ESP restart,<br>connect to AP access point Dynamics-" + idTopic + "<br>to configure wifi settings again<br><a href=\"http://192.168.4.1\">http://192.168.4.1</a></h1>");
    });
    
    // Route for system settings /System-Settings.html web page
    server.on("/system-settings", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/System-Settings.html.jgz", "text/html", false);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });
    server.on("/AutoUpdate", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", String(AutoUpdate));
    });
    server.on("/setAU", HTTP_GET, [](AsyncWebServerRequest *request){
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          // HTTP POST ssid value
          const char* PARAM_INPUT_1 = "state";                  // Search for parameter in HTTP POST request
          if (p->name() == PARAM_INPUT_1) {
            AutoUpdate = p->value().toInt();
//            Serial.print("Auto Update set to: ");
//            Serial.println(AutoUpdate);
            // Write file to save value
//            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      SetAutoUpdateFlag = HIGH;
      request->send(200, "text/html", "<h1>OK</h1>");
    });

    // Route for about /About.html web page
    server.on("/about", HTTP_GET, [](AsyncWebServerRequest * request) {
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/About.html.jgz", "text/html", false);
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });
    server.on("/SN", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", idTopic);
    });
    server.on("/FW", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/txt", FW_versionStr);
    });
    
/*
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

    */
    server.begin();
  }


  delay(200);
  vTaskDelay(300);
  client.setServer(mqtt_server, 31883);
  client.setCallback(callback);


  //init and get the time
  configTime(0, 0, ntpServer);


  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);

  vTaskDelay(500);
  

  SendDebugF2();

  if(debug2 == 1){
    Serial.println(FW_versionStr);
  }

  delay(100);
  
  if(PAndC == LOW && PowerOn == LOW){
    TurnSleep();
  }
  
  
  SetupComplete = HIGH;
  delay(100);
}

void loop() {
  // put your main code here, to run repeatedly:
  SENDFWversion();

  client.loop();

  SENDip();


  if(ConnectionTimeoutFlag == HIGH && TimeoutTimeSet == LOW){
    TimeoutTime = millis();
    TimeoutTimeSet = HIGH;
    vTaskDelay(20);
  }
  long TimeoutTimeCurrent = millis();
  if(TimeoutTimeSet == HIGH && TimeoutTimeCurrent-TimeoutTime > timer14){
    TimeoutTimeSet = LOW;
    ConnectionTimeoutFlag = LOW;
    vTaskDelay(20);
  }

//  Serial.println("loop");
  if(!client.connected()){
      digitalWrite(LED_RED, HIGH);
      digitalWrite(LED_GREEN, LOW);
      if(NoWANPandC == HIGH && PAndC == LOW){
        PAndC = HIGH;
        NoWANPandCActive = HIGH;
      }
//      Serial.println("client not connected");
      if(WiFi.status() != WL_CONNECTED){
//        Serial.println("wifi not connected");      
        WiFiReconnect();
        vTaskDelay(20);
        wifi_reconnects = wifi_reconnects + 5;
  //        ESP.restart();  // test, da mi ne zginjajo
      }else if(WiFi.status() == WL_CONNECTED){
        vTaskDelay(20);
        reconnect();
        r = r+1;
      }
  }else{
    wifi_reconnects = 0;
    if(ip == "0.0.0.0"){
      ip = WiFi.localIP().toString();
    }
    if(NoWANPandCActive == HIGH){
      NoWANPandCActive = LOW;
      PAndC = LOW;
    }
    client.loop();
    vTaskDelay(20);
    digitalWrite(LED_RED, LOW);
  }

  
  if(UpdateStart == HIGH){
    if((WiFi.status() == WL_CONNECTED)) {
      bool updatedNeeded = esp32FOTA.execHTTPcheck();
      if(updatedNeeded == HIGH){
        if(debug6 == 1){
          Serial.print("najden update");
        }
        server.end();
        esp32FOTA.execOTA();
      }else{
        UpdateStart = LOW;
      }
    }
  }

  if((WiFi.status() == WL_CONNECTED)){
    client.loop();
    if(UpdateSpiffs == HIGH) {
        if(debug6 == 1){
          Serial.println("checking for wp update");
        }
        bool updatedNeeded = esp32FOTA.execHTTPcheck();
        if(updatedNeeded == HIGH){
          server.end();
//          SPIFFS.end();
//          SPIFFS.format();
          if(debug6 == 1){
            Serial.println("Update SPIFFS...");
          }
          debuglog += "$";
          debuglog += "SPIFFS update select";
          debuglog += "$";
          esp32FOTA.execOTA();
          UpdateSpiffs = LOW;
        }else{
          UpdateSpiffs = LOW;
        }
    }else{
      if(!SPIFFS.exists("/Implera-Dynamics.html.jgz")){
        UpdateSpiffs = HIGH;
      }
    }
  }


  Averaging();


  if(SetChargeSettingsFlag == HIGH){
    if(debug6 == 1){
      Serial.println("saving charging settings");
    }
    SetChargeSettings();
    SetChargeSettingsFlag = LOW;
  }

  if(RestoreChargeSettingsFlag == HIGH){
    if(debug6 == 1){
      Serial.println("restore charging settings");
    }
    RestoreChargeSettings();
    SetChargeSettingsFlag = LOW;
  }

  if(SetWiFiCredsFlag == HIGH){
    SetWiFiCredentials();
    SetWiFiCredsFlag = LOW;
    ESP.restart();    
  }

  if(DeleteWiFiCredsFlag == HIGH){
    DeleteWiFiCredentials();
    SetWiFiCredsFlag = LOW;
    ESP.restart(); 
  }

  if(SetAutoUpdateFlag == HIGH){
    SetAutoUpdate();
    SetAutoUpdateFlag = LOW;
  }

  client.loop();


  Dovoljen_Tok();
  ChargeChanger();
  vTaskDelay(5);
  SetMQTTCurrent();
  vTaskDelay(5);
  CurrentFlagSet();
  SetCurrent();
  vTaskDelay(5);
  client.loop();
  vTaskDelay(5);
  StopCharge();
  ConnectionAlert();
  vTaskDelay(5);
  PowerStatusChanger();
  vTaskDelay(5);
  NegativeAmperageSet();
  vTaskDelay(5);
  EraseLCDText();
  PACChargeCheck();
  

  CatchStateChange();
  ResponseHandleRAPIF();
  ResponseHandleSetLimit();
  ResponseHandleCheckState();
  ResponseHandleCheckStatus();
  ResponseHandleCheckSetAmps();
  ResponseHandleCheckCharge();
  ResponseHandleCheckEnergy();
  ResponseHandleTurnOn();
  ResponseHandleTurnOff();
  ResponseHandleTurnSleep();
  ResponseHandleSetMQTTCurrent();
  ResponseHandleSetCurrent();
  ResponseHandleSetTimer();
  
    long now = millis();
    if (now - lastInfo > timer) {   //2000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo = now;
      vTaskDelay(20);
      SENDCurrents();
      digitalWrite(LED_GREEN, LOW);
    }
    now = millis();
    if (((now - lastInfo5 > timer5) && PowerOn == HIGH) || (PowerOn == LOW && (now - lastInfo5 > timer5*TimersFactorOff*10))) {    // 200000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo5 = now;
      vTaskDelay(20);
      CheckSetAmps();
      digitalWrite(LED_GREEN, LOW);
    }
    now = millis();
    if (((now - lastInfo6 > timer6) && PowerOn == LOW) || (PowerOn == HIGH && (now - lastInfo6 > timer6*TimersFactorOff))) {     //30000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo6 = now;
      vTaskDelay(20);
      CheckState();
      digitalWrite(LED_GREEN, LOW);
    } 
    now = millis();
    if (((now - lastInfo7 > timer7) && PowerOn == HIGH) || (PowerOn == LOW && (now - lastInfo7 > timer7*TimersFactorOff*10))) {      //30000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo7 = now;
      vTaskDelay(20);
      CheckCharge();
      digitalWrite(LED_GREEN, LOW);
    }
    now = millis();
    if (((now - lastInfo8 > timer8) && PowerOn == HIGH) || (PowerOn == LOW && (now - lastInfo8 > timer8*TimersFactorOff*10))) {     //200000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo8 = now;
      vTaskDelay(20);
      CheckStatus();
      digitalWrite(LED_GREEN, LOW);
    } 
    client.loop();
    vTaskDelay(100);
    now = millis();
    if (((now - lastInfo9 > timer9) && PowerOn == HIGH) || (PowerOn == LOW && (now - lastInfo9 > timer9*TimersFactorOff*10))) {    //120000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo9 = now;
      vTaskDelay(20);
      CheckEnergy();
      digitalWrite(LED_GREEN, LOW);
    }
    now = millis();
    if ((now - lastInfo10 > timer10) || (debuglog.length() > 190)) {   //2000
      String debugWhole = debuglog;
      digitalWrite(LED_GREEN, HIGH);
      while(debugWhole.length() > 195){
        debuglog = debugWhole.substring(0, 195);
        debugWhole.remove(0, 195);
        SendDebugF2();
      }
      lastInfo10 = now;
      vTaskDelay(20);
      SendDebugF2();
      digitalWrite(LED_GREEN, LOW);
    }
    now = millis();
    if (now - lastInfo12 > timer12) {   //100000
      digitalWrite(LED_GREEN, HIGH);
      lastInfo12 = now;
      vTaskDelay(20);
      SENDSyncClock();
      digitalWrite(LED_GREEN, LOW);
    } 
 client.loop();
 vTaskDelay(150);

}

void Averaging(){
  if(calibration > 5){
      switch (CTEnable){
        case 1:
          total1 = total1 - branja1[readindex1];
          branja1[readindex1] = Irms_1;
          total1 = total1 + branja1[readindex1];
          readindex1 = readindex1 + 1;
        
          if (readindex1 >= NoRead1){
            readindex1 = 0;
          }
        
          vTaskDelay(5);

          average1 = total1 / NoRead1;
          average2 = average1;
          average3 = average1;
          break;
        case 2:
          total2 = total2 - branja2[readindex2];
          branja2[readindex2] = Irms_2;
          total2 = total2 + branja2[readindex2];
          readindex2 = readindex2 + 1;
        
          if (readindex2 >= NoRead2){
            readindex2 = 0;
          }
        
          vTaskDelay(5);

          average2 = total2 / NoRead2;
          average1 = charge_current;
          average3 = average2;
          break;
        case 3:
          total3 = total3 - branja3[readindex3];
          branja3[readindex3] = Irms_3;
          total3 = total3 + branja3[readindex3];
          readindex3 = readindex3 + 1;
        
          if (readindex3 >= NoRead3){
            readindex3 = 0;
          }
        
          vTaskDelay(5);

          average3 = total3 / NoRead3;
          average1 = charge_current;
          average2 = average3;
          break;
        case 12:
          total1 = total1 - branja1[readindex1];
          branja1[readindex1] = Irms_1;
          total1 = total1 + branja1[readindex1];
          readindex1 = readindex1 + 1;
        
          if (readindex1 >= NoRead1){
            readindex1 = 0;
          }
        
          vTaskDelay(5);

          total2 = total2 - branja2[readindex2];
          branja2[readindex2] = Irms_2;
          total2 = total2 + branja2[readindex2];
          readindex2 = readindex2 + 1;
        
          if (readindex2 >= NoRead2){
            readindex2 = 0;
          }
        
          vTaskDelay(5);

          average1 = total1 / NoRead1;
          average2 = total2 / NoRead2;
          average3 = average2;
          break;
        case 23:
          total2 = total2 - branja2[readindex2];
          branja2[readindex2] = Irms_2;
          total2 = total2 + branja2[readindex2];
          readindex2 = readindex2 + 1;
        
          if (readindex2 >= NoRead2){
            readindex2 = 0;
          }
        
          vTaskDelay(5);

          total3 = total3 - branja3[readindex3];
          branja3[readindex3] = Irms_3;
          total3 = total3 + branja3[readindex3];
          readindex3 = readindex3 + 1;
        
          if (readindex3 >= NoRead3){
            readindex3 = 0;
          }
        
          vTaskDelay(5);

          average2 = total2 / NoRead2;
          average3 = total3 / NoRead3;
          average1 = charge_current;
          break;
        case 13:
          total1 = total1 - branja1[readindex1];
          branja1[readindex1] = Irms_1;
          total1 = total1 + branja1[readindex1];
          readindex1 = readindex1 + 1;
        
          if (readindex1 >= NoRead1){
            readindex1 = 0;
          }
        
          vTaskDelay(5);
         
          total3 = total3 - branja3[readindex3];
          branja3[readindex3] = Irms_3;
          total3 = total3 + branja3[readindex3];
          readindex3 = readindex3 + 1;
        
          if (readindex3 >= NoRead3){
            readindex3 = 0;
          }
        
          vTaskDelay(5);

          average1 = total1 / NoRead1;
          average3 = total3 / NoRead3;
          average2 = average3;
          break;
        case 123:
          total1 = total1 - branja1[readindex1];
          branja1[readindex1] = Irms_1;
          total1 = total1 + branja1[readindex1];
          readindex1 = readindex1 + 1;
        
          if (readindex1 >= NoRead1){
            readindex1 = 0;
          }
        
          vTaskDelay(5);

          total2 = total2 - branja2[readindex2];
          branja2[readindex2] = Irms_2;
          total2 = total2 + branja2[readindex2];
          readindex2 = readindex2 + 1;
        
          if (readindex2 >= NoRead2){
            readindex2 = 0;
          }
        
          vTaskDelay(5);

          total3 = total3 - branja3[readindex3];
          branja3[readindex3] = Irms_3;
          total3 = total3 + branja3[readindex3];
          readindex3 = readindex3 + 1;
        
          if (readindex3 >= NoRead3){
            readindex3 = 0;
          }
        
          vTaskDelay(5);

          average1 = total1 / NoRead1;
          average2 = total2 / NoRead2;
          average3 = total3 / NoRead3;
          break;
      }
      if(NegAmpFlag == HIGH){
        average1 = -average1;
        average2 = -average2;
        average3 = -average3;
      }
  }
}

void EraseLCDText(){
  if(LCDEraseFlag == HIGH){
    if(ConnectionTimeoutFlag == LOW){
    ResponseStatus = LOW;
    debuglog += "$";
    debuglog += "Erase LCD = FP";
    debuglog += "$";
    if(Serial2.available() > 0){
      CatchStateChange();
    }
    ResponseMessage = "";
    String LCDEraser;
    Serial2.println("$FP 0 0       ");
//    LastCom = "EraseLCD($FP)";
    t1 = 0;
    while(ResponseStatus == LOW && t1 < timer13){
      if(Serial2.available()){
        ResponseStatus = HIGH;
        ResponseMessageTemp = Serial2.readString();
        int startIndex = -1;
        for (int i = 0; i < ResponseMessageTemp.length(); i++) {
          if (ResponseMessageTemp.charAt(i) == '$') {
            if (startIndex >= 0) {
              ResponseMessage = ResponseMessageTemp.substring(startIndex, i);
              // do something with the single message
            }
            startIndex = i;
          }
        }
        if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
          ResponseMessageAsync = ResponseMessageTemp.substring(startIndex);
          CatchStateChange();
          // do something with the last single message
        }
      }
      t1 = t1 + 1;
      delayMicroseconds(5);
    }
    Serial2.println("$FP 6 0       ");
    LastCom = "EraseLCD($FP)";
    ResponseStatus = LOW;
    t1 = 0;
    while(ResponseStatus == LOW && t1 < timer13){
      if(Serial2.available()){
        ResponseStatus = HIGH;
        ResponseMessageTemp = Serial2.readString();
        int startIndex = -1;
        for (int i = 0; i < ResponseMessageTemp.length(); i++) {
          if (ResponseMessageTemp.charAt(i) == '$') {
            if (startIndex >= 0) {
              ResponseMessage = ResponseMessageTemp.substring(startIndex, i);
              // do something with the single message
            }
            startIndex = i;
          }
        }
        if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
          ResponseMessageAsync = ResponseMessageTemp.substring(startIndex);
          CatchStateChange();
          // do something with the last single message
        }
      }
      t1 = t1 + 1;
      delayMicroseconds(5);
    }
    Serial2.println("$FP 11 0      ");
    ResponseStatus = LOW;
    t1 = 0;
    while(ResponseStatus == LOW && t1 < timer13){
      if(Serial2.available()){
        ResponseStatus = HIGH;
        ResponseMessageTemp = Serial2.readString();
        int startIndex = -1;
        for (int i = 0; i < ResponseMessageTemp.length(); i++) {
          if (ResponseMessageTemp.charAt(i) == '$') {
            if (startIndex >= 0) {
              ResponseMessage = ResponseMessageTemp.substring(startIndex, i);
              // do something with the single message
            }
            startIndex = i;
          }
        }
        if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
          ResponseMessageAsync = ResponseMessageTemp.substring(startIndex);
          CatchStateChange();
          // do something with the last single message
        }
      }
      t1 = t1 + 1;
      delayMicroseconds(5);
    }
    Serial2.println("$FP 0 1       ");
    ResponseStatus = LOW;
    t1 = 0;
    while(ResponseStatus == LOW && t1 < timer13){
      if(Serial2.available()){
        ResponseStatus = HIGH;
        ResponseMessageTemp = Serial2.readString();
        int startIndex = -1;
        for (int i = 0; i < ResponseMessageTemp.length(); i++) {
          if (ResponseMessageTemp.charAt(i) == '$') {
            if (startIndex >= 0) {
              ResponseMessage = ResponseMessageTemp.substring(startIndex, i);
              // do something with the single message
            }
            startIndex = i;
          }
        }
        if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
          ResponseMessageAsync = ResponseMessageTemp.substring(startIndex);
          CatchStateChange();
          // do something with the last single message
        }
      }
      t1 = t1 + 1;
      delayMicroseconds(5);
    }
    Serial2.println("$FP 6 1       ");
    ResponseStatus = LOW;
    t1 = 0;
    while(ResponseStatus == LOW && t1 < timer13){
      if(Serial2.available()){
        ResponseStatus = HIGH;
        ResponseMessageTemp = Serial2.readString();
        int startIndex = -1;
        for (int i = 0; i < ResponseMessageTemp.length(); i++) {
          if (ResponseMessageTemp.charAt(i) == '$') {
            if (startIndex >= 0) {
              ResponseMessage = ResponseMessageTemp.substring(startIndex, i);
              // do something with the single message
            }
            startIndex = i;
          }
        }
        if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
          ResponseMessageAsync = ResponseMessageTemp.substring(startIndex);
          CatchStateChange();
          // do something with the last single message
        }
      }
      t1 = t1 + 1;
      delayMicroseconds(5);
    }
    Serial2.println("$FP 11 1       ");
    LastCom = "EraseLCD($FP)";
    ResponseStatus = LOW;
    t1 = 0;
    while(ResponseStatus == LOW && t1 < timer13){
      if(Serial2.available()){
        ResponseStatus = HIGH;
        ResponseMessageTemp = Serial2.readString();
        int startIndex = -1;
        for (int i = 0; i < ResponseMessageTemp.length(); i++) {
          if (ResponseMessageTemp.charAt(i) == '$') {
            if (startIndex >= 0) {
              ResponseMessage = ResponseMessageTemp.substring(startIndex, i);
              // do something with the single message
            }
            startIndex = i;
          }
        }
        if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
          ResponseMessageAsync = ResponseMessageTemp.substring(startIndex);
          CatchStateChange();
          // do something with the last single message
        }
      }
      t1 = t1 + 1;
      delayMicroseconds(5);
    }
    if(t1 > timer13/2){
      if(debug1_MQTT == 1){
        if (client.connected()){
          topica = "";
          dynamicTopic = "";
  //        epochtimeTopic = getTime();
          dynamicTopic += prefixTopic;
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
    }
    if(t1 > timer13 - 1){
      if(LoRaCERetryCount == LoRaCERetries){
        if(debug1 == 1){
          Serial.print("timeout flag ON, t1 = ");
          Serial.println(t1);
        }
        ConnectionTimeoutFlag = HIGH;
      }else{
        LoRaCERetries = LoRaCERetries + 1;
      }
     }else{
        LoRaCERetries = 0;
        if(debug1 == 1){
          Serial.print("timeout flag OFF, t1 = ");
          Serial.println(t1);
        }
        ConnectionTimeoutFlag = LOW;
        if(debug2_MQTT == 1){
          if (client.connected()){
            topica = "";
            dynamicTopic = "";
        //    epochtimeTopic = getTime();
            dynamicTopic += prefixTopic;
            dynamicTopic += idTopic;
        //    dynamicTopic += "/";
        //    dynamicTopic += epochtimeTopic;
            fullTopic = dynamicTopic;
            fullTopic += ResponseLCDEraseTopic;
            topica = fullTopic.c_str();
            TempValue = "";
            TempValue += ResponseMessage;
            TempValueChar = TempValue.c_str();
            client.publish(topica, TempValueChar);
  //          delay(20);
          }
        }
        LCDEraseFlag = LOW;
      }
      t1 = 0;
  }
  }
}

void PowerStatusChanger(){
  if(ActiveTimeoutFlag == HIGH){
    PowerStatus = 2;
  }else{
    PowerStatus = PowerOn;
  }
}

void CalcPower(){
  power = charge_current*230;
  switch(active_phases){
    case 0:
      noofphases = 0;
     break;
    case 1:
      noofphases = 1;
     break;
    case 2:
      noofphases = 1;
     break;
    case 3:
      noofphases = 1;
     break;
    case 12:
      power = power*2;
      noofphases = 2;
     break;
    case 13:
      power = power*2;
      noofphases = 2;
     break;
    case 23:
      power = power*2;
      noofphases = 2;
     break;
    case 123:
      power = power*3;
      noofphases = 3;
     break;
  }
}

void CalcEnergy(){
  switch(active_phases){
    case 12:
      energy = energy*2;
     break;
    case 13:
      energy = energy*2;
     break;
    case 23:
      energy = energy*2;
     break;
    case 123:
      energy = energy*3;
     break;
  }
}

void NegativeAmperageSet(){
  if(NegativeAmperage == HIGH || NegAmpOnceFlag == HIGH){
    nowNegAmp = millis();
    if(nowNegAmp - NegAmpTime < 30000){
      NegAmpFlag = HIGH;
    }else{
      NegAmpFlag = LOW;
      NegAmpOnceFlag = LOW;
    }
  }
}


void Dovoljen_Tok(){
  if(DinamicsActive == HIGH){
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
    if(max_current > MQTTmax_current){
      max_current = MQTTmax_current;
    }
  }else{
    max_current = MQTTmax_current;
  }
}


void SENDBreaker(){
  if (client.connected()){
    
    debuglog += "$";
    debuglog += "Sending breakers";
    debuglog += "$";
    
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
    topica = "";
    dynamicTopic = "";
  //  epochtimeTopic = getTime();
    dynamicTopic += prefixTopic;
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
    
    debuglog += "$";
    debuglog += "Sending breakers Alternate";
    debuglog += "$";
    
    topica = "";
    dynamicTopic = "";
  //  epochtimeTopic = getTime();
    dynamicTopic += prefixTopic;
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

    debuglog += "$";
    debuglog += "Sending FW version";
    debuglog += "$";
    
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
    topica = "";
    dynamicTopic = "";
    epochtimeTopic = getTime();
    dynamicTopic += prefixTopic;
    dynamicTopic += idTopic;
    dynamicTopic += "/";
    dynamicTopic += epochtimeTopic;
    fullTopic = dynamicTopic;
    fullTopic += FWversionTopic;
    topica = fullTopic.c_str();
    TempValue = "";
    TempValue += FW_versionStr;
    TempValue += " ";
    TempValue += "HW2";
    TempValueChar = TempValue.c_str();
    client.publish(topica, TempValueChar);
//    delay(20);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    FWSentFlag = HIGH;
  }
}

void SENDip(){
  if (client.connected() && ipSentFlag == LOW){

    debuglog += "$";
    debuglog += "Sending ip";
    debuglog += "$";

    if(ip == "0.0.0.0"){
      ip = WiFi.localIP().toString();
    }
    
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
    topica = "";
    dynamicTopic = "";
//    epochtimeTopic = getTime();
    dynamicTopic += prefixTopic;
    dynamicTopic += idTopic;
//    dynamicTopic += "/";
//    dynamicTopic += epochtimeTopic;
    fullTopic = dynamicTopic;
    fullTopic += ipTopic;
    topica = fullTopic.c_str();
    TempValue = "";
    TempValue += ip;
    TempValueChar = TempValue.c_str();
    client.publish(topica, TempValueChar);
//    delay(20);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    ipSentFlag = HIGH;
  }
}

void RespondPandC(){
  dynamicTopic = "";
//  epochtimeTopic = getTime();
  dynamicTopic += prefixTopic;
  dynamicTopic += idTopic;
//  dynamicTopic += "/";
//  dynamicTopic += epochtimeTopic;
  fullTopic = dynamicTopic;
  fullTopic += PandCTopic;
  topica = fullTopic.c_str();
  TempValue = "";
  TempValue += vmesna;
  TempValueChar = TempValue.c_str();
  client.publish(topica, TempValueChar);
}

void SENDCurrents(){
  if (client.connected()){    
    float tempaverage = average1Old - average1;
    tempaverage = abs(tempaverage);
    if(tempaverage >= 0.15){
      debuglog += "$";
      debuglog += "Sending Current 1";
      debuglog += "$";
      topica = "";
      dynamicTopic = "";
    //  epochtimeTopic = getTime();
      dynamicTopic += prefixTopic;
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
      vTaskDelay(20);
    }
   

    tempaverage = average2Old - average2;
    tempaverage = abs(tempaverage);
    if(tempaverage >= 0.15){
      debuglog += "$";
      debuglog += "Sending Current 2";
      debuglog += "$";
    //  dynamicTopic = prefixTopic;
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
      vTaskDelay(20);
    }

    tempaverage = average3Old - average3;
    tempaverage = abs(tempaverage);
    if(tempaverage >= 0.15){
      debuglog += "$";
      debuglog += "Sending Current 3";
      debuglog += "$";
    //  dynamicTopic = prefixTopic;
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
      vTaskDelay(20);
    }

    int tempmax_current = max_currentOld - max_current;
    tempmax_current = abs(tempmax_current);
    if(tempmax_current >= 0.15){
      debuglog += "$";
      debuglog += "Sending Max current";
      debuglog += "$";
    //  dynamicTopic = prefixTopic;
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
      vTaskDelay(20);
    }


    float tempcharge_current = charge_currentOld - charge_current;
    tempcharge_current = abs(tempcharge_current);
    if(tempcharge_current >= 0.15){
      debuglog += "$";
      debuglog += "Sending Charge Current";
      debuglog += "$";
    //  dynamicTopic = prefixTopic;
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
      vTaskDelay(20);
    }

    float temppower = powerOld - power;
    temppower = abs(temppower);
    if(temppower >= 100){
      debuglog += "$";
      debuglog += "Sending Power";
      debuglog += "$";
    //  dynamicTopic = prefixTopic;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += PowerTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += power;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      powerOld = power;
  //    delay(20);
      vTaskDelay(20);
    }

    int32_t tempenergy = energyOld - energy;
    tempenergy = abs(tempenergy);
    if(tempenergy >= 100){
      debuglog += "$";
      debuglog += "Sending Energy";
      debuglog += "$";
    //  dynamicTopic = prefixTopic;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += EnergyTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += energy;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      energyOld = energy;
  //    delay(20);
      vTaskDelay(20);
    }


    if(active_phasesOld != active_phases){
      debuglog += "$";
      debuglog += "Sending Active Phases";
      debuglog += "$";
    //  dynamicTopic = prefixTopic;
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
//      delay(10);
      fullTopic = dynamicTopic;
      fullTopic += NoOfPhasesTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += noofphases;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
  //    delay(20);
      active_phasesOld = active_phases;
    }
  }
}


void SENDSyncClock(){
  if (client.connected()){
    debuglog += "$";
    debuglog += "Sending Clock Sync Message";
    debuglog += "$";

    topica = "";
    dynamicTopic = "";
    epochtimeTopic = getTime();
    dynamicTopic += prefixTopic;
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
    delay(10);

    int FreeRAM = ESP.getFreeHeap();
    topica = "";
    dynamicTopic = "";
//    epochtimeTopic = getTime();
    dynamicTopic += prefixTopic;
    dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
    fullTopic = dynamicTopic;
    fullTopic += FreeRAMTopic;
    topica = fullTopic.c_str();
    TempValue = "";
    TempValue += FreeRAM;
    TempValueChar = TempValue.c_str();
    client.publish(topica, TempValueChar);
  }
}

void SENDCurrentsAlt(){
  if (client.connected()){

      debuglog += "$";
      debuglog += "Sending Currents Alternate";
      debuglog += "$";
      

      topica = "";
      dynamicTopic = "";
    //  epochtimeTopic = getTime();
      dynamicTopic += prefixTopic;
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

      vTaskDelay(20);



    //  dynamicTopic = prefixTopic;
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

      vTaskDelay(20);



    //  dynamicTopic = prefixTopic;
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

      vTaskDelay(20);



    //  dynamicTopic = prefixTopic;
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

      vTaskDelay(20);


    //  dynamicTopic = prefixTopic;
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

      vTaskDelay(20);

      float temppower = powerOld - power;
    temppower = abs(temppower);
    if(temppower >= 100){
      debuglog += "$";
      debuglog += "Sending Power";
      debuglog += "$";
    //  dynamicTopic = prefixTopic;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += PowerTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += power;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      powerOld = power;
  //    delay(20);

      vTaskDelay(20);
    }

    int32_t tempenergy = energyOld - energy;
    tempenergy = abs(tempenergy);
    if(tempenergy >= 100){
      debuglog += "$";
      debuglog += "Sending Energy";
      debuglog += "$";
    //  dynamicTopic = prefixTopic;
    //  dynamicTopic += idTopic;
    //  dynamicTopic += "/";
    //  dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += EnergyTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += energy;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      energyOld = energy;
  //    delay(20);

      vTaskDelay(20);
    }


    if(active_phasesOld != active_phases){
      debuglog += "$";
      debuglog += "Sending Active Phases";
      debuglog += "$";
    //  dynamicTopic = prefixTopic;
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
//      delay(10);
      fullTopic = dynamicTopic;
      fullTopic += NoOfPhasesTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += noofphases;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
  //    delay(20);
      active_phasesOld = active_phases;
    }
  }
}


void ConnectionAlert(){
  if((ConnectionTimeoutFlag == HIGH) && (ActiveTimeoutFlag == LOW)){
    ActiveTimeoutFlag = HIGH;
    debuglog += "$";
    debuglog += "Connection Alert No Connection";
    debuglog += "$";
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //   epochtimeTopic = getTime();
      dynamicTopic += prefixTopic;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += ConnectionTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue = "CON_ERROR_FAILED_COM :";
      TempValue += LastCom;
      TempValueChar = TempValue.c_str();    
      client.publish(topica, TempValueChar, true);
//      delay(20);
    }
  }
  if((ConnectionTimeoutFlag == LOW) && (ActiveTimeoutFlag == HIGH)){
    ActiveTimeoutFlag = LOW;
    debuglog += "$";
    debuglog += "Connection Alert Connection OK";
    debuglog += "$";
    if (client.connected()){
      topica = "";
      dynamicTopic = "";
  //   epochtimeTopic = getTime();
      dynamicTopic += prefixTopic;
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
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    COMused = HIGH;
    ResponseStatusCheckState = LOW;
    debuglog += "$";
    debuglog += "Checking State RAPI = G0";
    debuglog += "$";
    if(Serial2.available() > 0){
      CatchStateChange();
    }
    ResponseMessage = "";
    Serial2.println(G0);
    LastCom = "CheckState($G0)";

    Requestmillis = millis();   
  }
}

void ResponseHandleCheckState(){
  if(ResponseStatusCheckState == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusCheckState = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                        if (client.connected()){
                          topica = "";
                          dynamicTopic = "";
                      //    epochtimeTopic = getTime();
                          dynamicTopic += prefixTopic;
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
                      }
                  
                      int index = ResponseMessage.indexOf(" ");
                      ResponseMessage.remove(0, index+1);
                      ResponseMessage.remove(1);
                      State = ResponseMessage.toInt();
              
                      if (client.connected()){
                        if(OldState != State){
                          OldState = State;
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
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                        if (client.connected()){
                          topica = "";
                          dynamicTopic = "";
                      //    epochtimeTopic = getTime();
                          dynamicTopic += prefixTopic;
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
                      }
                  
                      int index = ResponseMessage.indexOf(" ");
                      ResponseMessage.remove(0, index+1);
                      ResponseMessage.remove(1);
                      State = ResponseMessage.toInt();
              
                      if (client.connected()){
                        if(OldState != State){
                          OldState = State;
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
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusCheckState == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
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
            ResponseStatusCheckState == HIGH;
            COMused = LOW;
        }
}

//void CheckState(){
//  if(ConnectionTimeoutFlag == LOW){
//    ResponseStatus = LOW;
//    debuglog += "$";
//    debuglog += "Checking State RAPI = G0";
//    debuglog += "$";
//    if(Serial2.available() > 0){
//      CatchStateChange();
//    }
//    ResponseMessage = "";
//    Serial2.println(G0);
//    LastCom = "CheckState($G0)";
//    t1 = 0;
//    while(ResponseStatus == LOW && t1 < timer13){
//      if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//      t1 = t1 + 1;
//      delayMicroseconds(50);
//    }
//    if(t1 > timer13/2){
//      if(debug1_MQTT == 1){
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//  //        epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//  //        dynamicTopic += "/";
//  //        dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += TimeoutTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue += t1;
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar);
//  //        delay(20);
//        }
//      }
//    }
//    if(t1 > timer13 - 1){
//      if(LoRaCERetryCount == LoRaCERetries){
//        if(debug1 == 1){
//          Serial.print("timeout flag ON, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = HIGH;
//        if(debug1_MQTT == 1){
//          if (client.connected()){
//            fullTopic = dynamicTopic;
//            fullTopic += StateTopic;
//            topica = fullTopic.c_str();
//            TempValue = "";
//            TempValue += "2";
//            TempValueChar = TempValue.c_str();
//            client.publish(topica, TempValueChar);
//  //          delay(20);
//          }
//        }
//      }else{
//        LoRaCERetries = LoRaCERetries + 1;
//      }
//     }else{
//        LoRaCERetries = 0;
//        if(debug1 == 1){
//          Serial.print("timeout flag OFF, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = LOW;
//        if(debug2_MQTT == 1){
//          if (client.connected()){
//            topica = "";
//            dynamicTopic = "";
//        //    epochtimeTopic = getTime();
//            dynamicTopic += prefixTopic;
//            dynamicTopic += idTopic;
//        //    dynamicTopic += "/";
//        //    dynamicTopic += epochtimeTopic;
//            fullTopic = dynamicTopic;
//            fullTopic += ResponseG0Topic;
//            topica = fullTopic.c_str();
//            TempValue = "";
//            TempValue += ResponseMessage;
//            TempValueChar = TempValue.c_str();
//            client.publish(topica, TempValueChar);
//  //          delay(20);
//          }
//        }
//    
//        int index = ResponseMessage.indexOf(" ");
//        ResponseMessage.remove(0, index+1);
//        ResponseMessage.remove(1);
//        State = ResponseMessage.toInt();
//
//        if (client.connected()){
//          if(OldState != State){
//            OldState = State;
//            fullTopic = dynamicTopic;
//            fullTopic += StateTopic;
//            topica = fullTopic.c_str();
//            TempValue = "";
//            TempValue += State;
//            TempValueChar = TempValue.c_str();
//            client.publish(topica, TempValueChar, true);
//  //          delay(20);
//          }
//        }
//      }
//      t1 = 0;
//  }
//}

void CheckStatus(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    COMused = HIGH;
    ResponseStatusCheckStatus = LOW;
    debuglog += "$";
    debuglog += "Checking Status RAPI = GS";
    debuglog += "$";
    if(Serial2.available() > 0){
      CatchStateChange();
    }
    ResponseMessage = "";
    Serial2.println(GS);
    LastCom = "CheckStatus($GS)";

    Requestmillis = millis();   
  }
}

void ResponseHandleCheckStatus(){
  if(ResponseStatusCheckStatus == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusCheckStatus = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                        if (client.connected()){
                          topica = "";
                          dynamicTopic = "";
                      //    epochtimeTopic = getTime();
                          dynamicTopic += prefixTopic;
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
                      }
                  
                      int index = ResponseMessage.indexOf(" ");
                      ResponseMessage.remove(0, index+1);
                      ResponseMessage.remove(1);
                      State = ResponseMessage.toInt();
              
                      if (client.connected()){
                        if(OldState != State){
                          OldState = State;
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
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                          topica = "";
                          dynamicTopic = "";
                      //    epochtimeTopic = getTime();
                          dynamicTopic += prefixTopic;
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
                      int index = ResponseMessage.indexOf(" ");
                      ResponseMessage.remove(0, index+1);
                      index = ResponseMessage.indexOf(" ");
                      ResponseMessage.remove(index);
                      uint8_t ChState = ResponseMessage.toInt();
                      if(ChState == 3 && PAndC == LOW && PowerOn == LOW && ChargeSetState == LOW){
                        if(debug6 == 1){
                          Serial.println("Izklop, stanje ne ustreza 1");
                        }
                        debuglog += "$";
                        debuglog += "Izklop, stanje ne ustreza";
                        debuglog += "$";
                        PowerOn = LOW;
                        SetRuntimeSettings();
                        TurnSleep();
                      }
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusCheckStatus == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
            ResponseStatusCheckStatus == HIGH;
            COMused = LOW;
        }
}


//void CheckStatus(){
//  if(ConnectionTimeoutFlag == LOW){
//    debuglog += "$";
//    debuglog += "Checking Status RAPI = GS";
//    debuglog += "$";
//    ResponseStatus = LOW;
//    if(Serial2.available() > 0){
//      CatchStateChange();
//    }
//    ResponseMessage = "";
//    Serial2.println(GS);
//    LastCom = "CheckStatus($GS)";
//    t1 = 0;
//    while(ResponseStatus == LOW && t1 < timer13){
//      if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//      t1 = t1 + 1;
//      delayMicroseconds(50);
//    }
//    if(t1 > timer13/2){
//      if(debug1_MQTT == 1){
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//  //        epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//  //        dynamicTopic += "/";
//  //        dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += TimeoutTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue += t1;
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar);
//  //        delay(20);
//        }
//      }
//    }
//    if(t1 > timer13 - 1){
//      if(LoRaCERetryCount == LoRaCERetries){
//        if(debug1 == 1){
//          Serial.print("timeout flag ON, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = HIGH;
//      }else{
//        LoRaCERetries = LoRaCERetries + 1;
//      }
//    }else{
//        LoRaCERetries = 0;
//        if(debug1 == 1){
//          Serial.print("timeout flag OFF, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = LOW;
//        t1 = 0;
//        if(debug2_MQTT == 1){
//          if (client.connected()){
//            topica = "";
//            dynamicTopic = "";
//        //    epochtimeTopic = getTime();
//            dynamicTopic += prefixTopic;
//            dynamicTopic += idTopic;
//        //    dynamicTopic += "/";
//        //    dynamicTopic += epochtimeTopic;
//            fullTopic = dynamicTopic;
//            fullTopic += ResponseGSTopic;
//            topica = fullTopic.c_str();
//            TempValue = "";
//            TempValue += ResponseMessage;
//            TempValueChar = TempValue.c_str();
//            client.publish(topica, TempValueChar);
//      //      delay(20);
//          }
//        }
//        int index = ResponseMessage.indexOf(" ");
//        ResponseMessage.remove(0, index+1);
//        index = ResponseMessage.indexOf(" ");
//        ResponseMessage.remove(index);
//        uint8_t ChState = ResponseMessage.toInt();
//        if(ChState == 3 && PAndC == LOW && PowerOn == LOW && ChargeSetState == LOW){
//          if(debug6 == 1){
//            Serial.println("Izklop, stanje ne ustreza 1");
//          }
//          debuglog += "$";
//          debuglog += "Izklop, stanje ne ustreza";
//          debuglog += "$";
//          PowerOn = LOW;
//          SetRuntimeSettings();
//          TurnSleep();
//        }
//      }
//  }
//}

void CheckSetAmps(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    COMused = HIGH;
    ResponseStatusCheckSetAmps = LOW;
    debuglog += "$";
    debuglog += "Checking Set AMPS RAPI = GC";
    debuglog += "$";
    if(Serial2.available() > 0){
      CatchStateChange();
    }
    ResponseMessage = "";
    Serial2.println(GC);
    LastCom = "CheckSetAmps($GC)";

    Requestmillis = millis();   
  }
}

void ResponseHandleCheckSetAmps(){
  if(ResponseStatusCheckSetAmps == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusCheckSetAmps = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                        if (client.connected()){
                          topica = "";
                          dynamicTopic = "";
                      //    epochtimeTopic = getTime();
                          dynamicTopic += prefixTopic;
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
                  
                        int index = ResponseMessage.indexOf("$OK");
                        ResponseMessage.remove(index, index+9);
                        index = ResponseMessage.indexOf(" ");
                        ResponseMessage.remove(index);
                        set_current = ResponseMessage.toInt();
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    int index = ResponseMessage.indexOf("$OK");
                    ResponseMessage.remove(index, index+9);
                    index = ResponseMessage.indexOf(" ");
                    ResponseMessage.remove(index);
                    set_current = ResponseMessage.toInt();
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusCheckSetAmps == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
            ResponseStatusCheckSetAmps == HIGH;
            COMused = LOW;
        }
}


//void CheckSetAmps(){
//  if(ConnectionTimeoutFlag == LOW){
//    debuglog += "$";
//    debuglog += "Checking Set AMPS RAPI = GC";
//    debuglog += "$";
//    ResponseStatus = LOW;
//    if(Serial2.available() > 0){
//      CatchStateChange();
//    }
//    ResponseMessage = "";
//    Serial2.println(GC);
//    LastCom = "CheckSetAmps($GC)";
//    t1 = 0;
//    while(ResponseStatus == LOW && t1 < timer13){
//      if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//      t1 = t1 + 1;
//      delayMicroseconds(50);
//    }
//    if(t1 > timer13/2){
//      if(debug1_MQTT == 1){
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//  //        epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//  //        dynamicTopic += "/";
//  //        dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += TimeoutTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue += t1;
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar);
//  //        delay(20);
//        }
//      }
//    }
//    if(t1 > timer13 - 1){
//      if(LoRaCERetryCount == LoRaCERetries){
//        if(debug1 == 1){
//          Serial.print("timeout flag ON, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = HIGH;
//      }else{
//        LoRaCERetries = LoRaCERetries + 1;
//      }
//    }else{
//        LoRaCERetries = 0;
//        if(debug1 == 1){
//          Serial.print("timeout flag OFF, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = LOW;
//    t1 = 0;
//    if(debug2_MQTT == 1){
//      if (client.connected()){
//        topica = "";
//        dynamicTopic = "";
//    //    epochtimeTopic = getTime();
//        dynamicTopic += prefixTopic;
//        dynamicTopic += idTopic;
//    //    dynamicTopic += "/";
//    //    dynamicTopic += epochtimeTopic;
//        fullTopic = dynamicTopic;
//        fullTopic += ResponseGCTopic;
//        topica = fullTopic.c_str();
//        TempValue = "";
//        TempValue += ResponseMessage;
//        TempValueChar = TempValue.c_str();
//        client.publish(topica, TempValueChar);
//  //      delay(20);
//        }
//      }
//
//      int index = ResponseMessage.indexOf("$OK");
//      ResponseMessage.remove(index, index+9);
//      index = ResponseMessage.indexOf(" ");
//      ResponseMessage.remove(index);
//      set_current = ResponseMessage.toInt();
//     }
//  }
//}

void CheckCharge(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    COMused = HIGH;
    ResponseStatusCheckCharge = LOW;
    debuglog += "$";
    debuglog += "Checking Charge RAPI = GG";
    debuglog += "$";
    if(Serial2.available() > 0){
      CatchStateChange();
    }
    ResponseMessage = "";
    Serial2.println(GG);
    LastCom = "CheckCharge($GG)";

    Requestmillis = millis();   
  }
}

void ResponseHandleCheckCharge(){
  if(ResponseStatusCheckCharge == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusCheckCharge = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                    if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
                
                
                    int index = ResponseMessage.indexOf(" ");
                    ResponseMessage.remove(0, index+1);
                    index = ResponseMessage.indexOf(" ");
                    ResponseMessage.remove(index);
                    uint32_t charge_current_mA = ResponseMessage.toInt();
                    charge_current = charge_current_mA / 1000.0;
                    if(charge_current > 0.05){
                      lastInfo6 = millis();
                      lastInfo8 = millis();
                    }
                
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
                    CalcPower();
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
                
                
                    int index = ResponseMessage.indexOf(" ");
                    ResponseMessage.remove(0, index+1);
                    index = ResponseMessage.indexOf(" ");
                    ResponseMessage.remove(index);
                    uint32_t charge_current_mA = ResponseMessage.toInt();
                    charge_current = charge_current_mA / 1000.0;
                    if(charge_current > 0.05){
                      lastInfo6 = millis();
                      lastInfo8 = millis();
                    }
                
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
                    CalcPower();
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusCheckCharge == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
            ResponseStatusCheckCharge == HIGH;
            COMused = LOW;
        }
}

//void CheckCharge(){
//  if(ConnectionTimeoutFlag == LOW){
//    ResponseStatus = LOW;
//    debuglog += "$";
//    debuglog += "Checking Charge RAPI = GG";
//    debuglog += "$";
//    if(Serial2.available() > 0){
//      CatchStateChange();
//    }
//    ResponseMessage = "";
//    Serial2.println(GG);
//    LastCom = "CheckCharge($GG)";
//    t1 = 0;
//    while(ResponseStatus == LOW && t1 < timer13){
//      if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//      t1 = t1 + 1;
//      delayMicroseconds(50);
//    }
//    if(t1 > timer13/2){
//      if(debug1_MQTT == 1){
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//  //        epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//  //        dynamicTopic += "/";
//  //        dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += TimeoutTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue += t1;
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar);
//  //        delay(20);
//        }
//      }
//    }
//    if(t1 > timer13 - 1){
//      if(LoRaCERetryCount == LoRaCERetries){
//        if(debug1 == 1){
//          Serial.print("timeout flag ON, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = HIGH;
//      }else{
//        LoRaCERetries = LoRaCERetries + 1;
//      }
//    }else{
//        LoRaCERetries = 0;
//        if(debug1 == 1){
//          Serial.print("timeout flag OFF, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = LOW;
//  
//          t1 = 0;
//          if(debug2_MQTT == 1){
//            if (client.connected()){
//              topica = "";
//              dynamicTopic = "";
//          //    epochtimeTopic = getTime();
//              dynamicTopic += prefixTopic;
//              dynamicTopic += idTopic;
//          //    dynamicTopic += "/";
//          //    dynamicTopic += epochtimeTopic;
//              fullTopic = dynamicTopic;
//              fullTopic += ResponseGGTopic;
//              topica = fullTopic.c_str();
//              TempValue = "";
//              TempValue += ResponseMessage;
//              TempValueChar = TempValue.c_str();
//              client.publish(topica, TempValueChar);
//        //      delay(20);
//            }
//          }
//      
//      
//          int index = ResponseMessage.indexOf(" ");
//          ResponseMessage.remove(0, index+1);
//          index = ResponseMessage.indexOf(" ");
//          ResponseMessage.remove(index);
//          uint32_t charge_current_mA = ResponseMessage.toInt();
//          charge_current = charge_current_mA / 1000.0;
//          if(charge_current > 0.05){
//            lastInfo6 = millis();
//            lastInfo8 = millis();
//          }
//      
//      //    Serial.print("C1 je :");
//      //    Serial.println(c1);
//          if((PhaseInfo == LOW) && (c1 > 2)){
//      //      Serial.println("Phase = LOW in c1 > 2");
//            if(charge_current > 5){
//              CheckPhaseChange();
//            }
//            if(c1 == 3){
//              c1 = 0;
//            }
//          }
//          c1 = c1+1;
//          if(c1 == 4){
//              c1 = 0;
//          }
//          CalcPower();
//        }
//  }
//}

void CheckEnergy(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    COMused = HIGH;
    ResponseStatusCheckEnergy = LOW;
    debuglog += "$";
    debuglog += "Checking Energy RAPI = GU";
    debuglog += "$";
    if(Serial2.available() > 0){
      CatchStateChange();
    }
    ResponseMessage = "";
    Serial2.println(GU);
    LastCom = "CheckEnergy($GU)";

    Requestmillis = millis();   
  }
}

void ResponseHandleCheckEnergy(){
  if(ResponseStatusCheckEnergy == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusCheckEnergy = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    int index = ResponseMessage.indexOf(" ");
                    ResponseMessage.remove(0, index+1);
                    index = ResponseMessage.indexOf(" ");
                    ResponseMessage.remove(index);
                    uint32_t energymid = ResponseMessage.toInt();
                    energy = energymid/3600;
                    CalcEnergy();
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    int index = ResponseMessage.indexOf(" ");
                    ResponseMessage.remove(0, index+1);
                    index = ResponseMessage.indexOf(" ");
                    ResponseMessage.remove(index);
                    uint32_t energymid = ResponseMessage.toInt();
                    energy = energymid/3600;
                    CalcEnergy();
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusCheckEnergy == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
            ResponseStatusCheckEnergy == HIGH;
            COMused = LOW;
        }
}


//void CheckEnergy(){
//  if(ConnectionTimeoutFlag == LOW){
//    ResponseStatus = LOW;
//    debuglog += "$";
//    debuglog += "Checking Energy RAPI = GU";
//    debuglog += "$";
//    if(Serial2.available() > 0){
//      CatchStateChange();
//    }
//    ResponseMessage = "";
//    Serial2.println(GU);
//    LastCom = "CheckEnergy($GU)";
//    t1 = 0;
//    while(ResponseStatus == LOW && t1 < timer13){
//      if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//      t1 = t1 + 1;
//      delayMicroseconds(50);
//    }
//    if(t1 > timer13/2){
//      if(debug1_MQTT == 1){
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//  //        epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//  //        dynamicTopic += "/";
//  //        dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += TimeoutTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue += t1;
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar);
//  //        delay(20);
//        }
//      }
//    }
//    if(t1 > timer13 - 1){
//      if(LoRaCERetryCount == LoRaCERetries){
//        if(debug1 == 1){
//          Serial.print("timeout flag ON, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = HIGH;
//      }else{
//        LoRaCERetries = LoRaCERetries + 1;
//      }
//    }else{
//        LoRaCERetries = 0;
//        if(debug1 == 1){
//          Serial.print("timeout flag OFF, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = LOW;
//        t1 = 0;
//        if(debug2_MQTT == 1){
//          if (client.connected()){
//            topica = "";
//            dynamicTopic = "";
//        //    epochtimeTopic = getTime();
//            dynamicTopic += prefixTopic;
//            dynamicTopic += idTopic;
//        //    dynamicTopic += "/";
//        //    dynamicTopic += epochtimeTopic;
//            fullTopic = dynamicTopic;
//            fullTopic += ResponseGUTopic;
//            topica = fullTopic.c_str();
//            TempValue = "";
//            TempValue += ResponseMessage;
//            TempValueChar = TempValue.c_str();
//            client.publish(topica, TempValueChar);
//      //      delay(20);
//          }
//        }
//        int index = ResponseMessage.indexOf(" ");
//        ResponseMessage.remove(0, index+1);
//        index = ResponseMessage.indexOf(" ");
//        ResponseMessage.remove(index);
//        uint32_t energymid = ResponseMessage.toInt();
//        energy = energymid/3600;
//        CalcEnergy();
//      }
//  }
//}

void TurnOn(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    COMused = HIGH;
    ResponseStatusTurnOn = LOW;
    debuglog += "$";
    debuglog += "Setting enable --> PowerOn ";
    debuglog += "$";
    ChargeSetState = HIGH;
    long now = millis();
    lastInfo5 = now;
    lastInfo7 = now;
    lastInfo8 = now;
    lastInfo9 = now - 80000;
    lastInfo10 = now;
    lastInfo12 = now;
    if(Serial2.available() > 0){
      CatchStateChange();
    }
    ResponseMessage = "";
    tmp = 2;
    Serial2.println(FE);
    LastCom = "Enable($FE)";
    SaveLastCurrents();

    Requestmillis = millis();   
  }
}


void ResponseHandleTurnOn(){
  if(ResponseStatusTurnOn == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusTurnOn = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if (client.connected()){
                      topica = "";
                      dynamicTopic = "";
                  //    epochtimeTopic = getTime();
                      dynamicTopic += prefixTopic;
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
            
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
                    lastInfo7 = lastInfo7 + 200;
                    lastInfo7 = lastInfo7 - timer7;
                    EnableState = 2;
                    c6 = 0;
                    PowerOn = HIGH;
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if (client.connected()){
                      topica = "";
                      dynamicTopic = "";
                  //    epochtimeTopic = getTime();
                      dynamicTopic += prefixTopic;
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
            
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
                    lastInfo7 = lastInfo7 + 200;
                    lastInfo7 = lastInfo7 - timer7;
                    EnableState = 2;
                    c6 = 0;
                    PowerOn = HIGH;
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusTurnOn == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
            ResponseStatusTurnOn == HIGH;
            COMused = LOW;
        }
}


//void TurnOn(){
//  if(ConnectionTimeoutFlag == LOW){
//    debuglog += "$";
//    debuglog += "Setting enable --> PowerOn ";
//    debuglog += "$";
//    ChargeSetState = HIGH;
//    ResponseStatus = LOW;
//    long now = millis();
//    lastInfo5 = now;
//    lastInfo7 = now;
//    lastInfo8 = now;
//    lastInfo9 = now - 80000;
//    lastInfo10 = now;
//    lastInfo12 = now;
//    if(Serial2.available() > 0){
//      CatchStateChange();
//    }
//    ResponseMessage = "";
//    tmp = 2;
//    Serial2.println(FE);
//    LastCom = "Enable($FE)";
//    t1 = 0;
//    SaveLastCurrents();
//    while(ResponseStatus == LOW && t1 < timer13){
//      if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//      t1 = t1 + 1;
//      delayMicroseconds(50);
//    }
//    if(t1 > timer13/2){
//      if(debug1_MQTT == 1){
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//  //        epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//  //        dynamicTopic += "/";
//  //        dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += TimeoutTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue += t1;
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar);
//  //        delay(20);
//        }
//      }
//    }
//    if(t1 > timer13 - 1){
//      if(LoRaCERetryCount == LoRaCERetries){
//        if(debug1 == 1){
//          Serial.print("timeout flag ON, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = HIGH;
//      }else{
//        LoRaCERetries = LoRaCERetries + 1;
//      }
//    }else{
//        LoRaCERetries = 0;
//        if(debug1 == 1){
//              Serial.print("timeout flag OFF, t1 = ");
//              Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = LOW;
//        if(NegativeAmperage == HIGH || NegAmpOnceFlag == HIGH){
//          NegAmpTime = millis();
//        }
//        t1 = 0;
////        tmp = 2;    PRESTAVIL GOR, RAZMISLI IN DAJ NAZAJ TU IN ZGORAJ ZBRIŠI
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//      //    epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//      //    dynamicTopic += "/";
//      //    dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += EnableTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue = "2";
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar, true);
////          delay(20);
//        }
//        PhaseInfo = LOW;
//        c1 = 0;
//
//        if(debug2_MQTT == 1){
//          if (client.connected()){
//            topica = "";
//            dynamicTopic = "";
//        //    epochtimeTopic = getTime();
//            dynamicTopic += prefixTopic;
//            dynamicTopic += idTopic;
//        //    dynamicTopic += "/";
//        //    dynamicTopic += epochtimeTopic;
//            fullTopic = dynamicTopic;
//            fullTopic += ResponseFETopic;
//            topica = fullTopic.c_str();
//            TempValue = "";
//            TempValue += ResponseMessage;
//            TempValueChar = TempValue.c_str();
//            client.publish(topica, TempValueChar);
//      //      delay(20);
//          }
//        }
//        lastInfo7 = lastInfo7 + 200;
//        lastInfo7 = lastInfo7 - timer7;
//        EnableState = 2;
//        c6 = 0;
//        PowerOn = HIGH;
//    }
//    t1 = 0;
//    
///*    int index = ATMessage.indexOf("$AT");
//    if(index > 1){
//      ATMessage.remove(0, index);
//      ResponseMessage.remove(index);
//    }*/
//
///*    if(index > 1){
//      if (client.connected()){
//        topica = "";
//        dynamicTopic = "";
//    //    epochtimeTopic = getTime();
//        dynamicTopic += prefixTopic;
//        dynamicTopic += idTopic;
//    //    dynamicTopic += "/";
//    //    dynamicTopic += epochtimeTopic;
//        fullTopic = dynamicTopic;
//        fullTopic += StateChangeTopic;
//        topica = fullTopic.c_str();
//        TempValue = "";
//        TempValue += ATMessage;
//        TempValueChar = TempValue.c_str();
//        client.publish(topica, TempValueChar);
//  //      delay(20);
//      }
//    }*/
//  }
//}

void TurnOff(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    COMused = HIGH;
    ResponseStatusTurnOff = LOW;
    debuglog += "$";
    debuglog += "Setting enable --> PowerOff ";
    debuglog += "$";
    ChargeSetState = LOW;
    if(Serial2.available() > 0){
      CatchStateChange();
    }
    tmp = 1;
    ResponseMessage = "";
    Serial2.println(FD);
    LastCom = "Disable($FD)";

    Requestmillis = millis();   
  }
}


void ResponseHandleTurnOff(){
  if(ResponseStatusTurnOff == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusTurnOn = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if (client.connected()){
                      topica = "";
                      dynamicTopic = "";
                  //    epochtimeTopic = getTime();
                      dynamicTopic += prefixTopic;
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
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
                    EnableState = 1;
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if (client.connected()){
                      topica = "";
                      dynamicTopic = "";
                  //    epochtimeTopic = getTime();
                      dynamicTopic += prefixTopic;
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
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
                    EnableState = 1;
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusTurnOff == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
            ResponseStatusTurnOff == HIGH;
            COMused = LOW;
        }
}


//void TurnOff(){
//  if(ConnectionTimeoutFlag == LOW){
//    debuglog += "$";
//    debuglog += "Setting enable --> PowerOff ";
//    debuglog += "$";
//    ChargeSetState = LOW;
//    ResponseStatus = LOW;
//    if(Serial2.available() > 0){
//      CatchStateChange();
//    }
//    tmp = 1;
//    ResponseMessage = "";
//    Serial2.println(FD);
//    LastCom = "Disable($FD)";
//    t1 = 0;
//    while(ResponseStatus == LOW && t1 < timer13){
//      if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//      t1 = t1 + 1;
//      delayMicroseconds(50);
//    }
//    if(t1 > timer13/2){
//      if(debug1_MQTT == 1){
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//  //        epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//  //        dynamicTopic += "/";
//  //        dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += TimeoutTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue += t1;
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar);
//  //        delay(20);
//        }
//      }
//    }
//    if(t1 > timer13 - 1){
//      if(LoRaCERetryCount == LoRaCERetries){
//        if(debug1 == 1){
//          Serial.print("timeout flag ON, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = HIGH;
//      }else{
//        LoRaCERetries = LoRaCERetries + 1;
//      }
//    }else{
//        LoRaCERetries = 0;
//        if(debug1 == 1){
//              Serial.print("timeout flag OFF, t1 = ");
//              Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = LOW;
//        t1 = 0;
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//      //    epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//      //    dynamicTopic += "/";
//      //    dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += EnableTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue = "1";
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar, true);
////          delay(20);
//        }
//        active_phases = 0;
//        if(debug2_MQTT == 1){
//          if (client.connected()){
//            topica = "";
//            dynamicTopic = "";
//        //    epochtimeTopic = getTime();
//            dynamicTopic += prefixTopic;
//            dynamicTopic += idTopic;
//        //    dynamicTopic += "/";
//        //    dynamicTopic += epochtimeTopic;
//            fullTopic = dynamicTopic;
//            fullTopic += ResponseFDTopic;
//            topica = fullTopic.c_str();
//            TempValue = "";
//            TempValue += ResponseMessage;
//            TempValueChar = TempValue.c_str();
//            client.publish(topica, TempValueChar);
//      //      delay(20);
//          }
//        }
//        EnableState = 1;
//    }
//    t1 = 0;
//
///*    int index = ATMessage.indexOf("$AT");
//    if(index > 1){
//      ATMessage.remove(0, index);
//      ResponseMessage.remove(index);
//    }*/
//    
// /*   if(index > 1){
//      if (client.connected()){
//        topica = "";
//        dynamicTopic = "";
//    //    epochtimeTopic = getTime();
//        dynamicTopic += prefixTopic;
//        dynamicTopic += idTopic;
//    //    dynamicTopic += "/";
//    //    dynamicTopic += epochtimeTopic;
//        fullTopic = dynamicTopic;
//        fullTopic += StateChangeTopic;
//        topica = fullTopic.c_str();
//        TempValue = "";
//        TempValue += ATMessage;
//        TempValueChar = TempValue.c_str();
//        client.publish(topica, TempValueChar);
//  //      delay(20);
//      }
//    }*/
//  }
//}

void TurnSleep(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    COMused = HIGH;
    ResponseStatusTurnSleep = LOW;
    debuglog += "$";
    debuglog += "Setting enable --> Sleep ";
    debuglog += "$";
    ChargeSetState = LOW;
    if(Serial2.available() > 0){
      CatchStateChange();
    }
    ResponseMessage = "";
    tmp = 3;
    Serial2.println(FS);
    LastCom = "Sleep($FS)";

    Requestmillis = millis();   
  }
}


void ResponseHandleTurnSleep(){
  if(ResponseStatusTurnSleep == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusTurnSleep = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    tmp = 3;
                    if (client.connected()){
                      topica = "";
                      dynamicTopic = "";
                  //    epochtimeTopic = getTime();
                      dynamicTopic += prefixTopic;
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
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
                    lastInfo7 = lastInfo7 + 200;
                    lastInfo7 = lastInfo7 - timer7;
                    EnableState = 3;
                    PowerOn = LOW;
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    tmp = 3;
                    if (client.connected()){
                      topica = "";
                      dynamicTopic = "";
                  //    epochtimeTopic = getTime();
                      dynamicTopic += prefixTopic;
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
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
                    lastInfo7 = lastInfo7 + 200;
                    lastInfo7 = lastInfo7 - timer7;
                    EnableState = 3;
                    PowerOn = LOW;
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusTurnSleep == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
            ResponseStatusTurnSleep == HIGH;
            COMused = LOW;
        }
}


//void TurnSleep(){
//  if(ConnectionTimeoutFlag == LOW){
//    debuglog += "$";
//    debuglog += "Setting enable --> Sleep ";
//    debuglog += "$";
//    ChargeSetState = LOW;
//    ResponseStatus = LOW;
//    if(Serial2.available() > 0){
//      CatchStateChange();
//    }
//    ResponseMessage = "";
//    tmp = 3;
//    Serial2.println(FS);
//    LastCom = "Sleep($FS)";
//    t1 = 0;
//    while(ResponseStatus == LOW && t1 < timer13){
//      if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//      t1 = t1 + 1;
//      delayMicroseconds(50);
//    }
//    if(t1 > timer13/2){
//      if(debug1_MQTT == 1){
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//  //        epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//  //        dynamicTopic += "/";
//  //        dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += TimeoutTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue += t1;
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar);
//  //        delay(20);
//        }
//      }
//    }
//    if(t1 > timer13 - 1){
//      if(LoRaCERetryCount == LoRaCERetries){
//        if(debug1 == 1){
//          Serial.print("timeout flag ON, t1 = ");
//          Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = HIGH;
//      }else{
//        LoRaCERetries = LoRaCERetries + 1;
//      }
//    }else{
//        LoRaCERetries = 0;
//        if(debug1 == 1){
//              Serial.print("timeout flag OFF, t1 = ");
//              Serial.println(t1);
//        }
//        ConnectionTimeoutFlag = LOW;
//        t1 = 0;
//        tmp = 3;
//        if (client.connected()){
//          topica = "";
//          dynamicTopic = "";
//      //    epochtimeTopic = getTime();
//          dynamicTopic += prefixTopic;
//          dynamicTopic += idTopic;
//      //    dynamicTopic += "/";
//      //    dynamicTopic += epochtimeTopic;
//          fullTopic = dynamicTopic;
//          fullTopic += EnableTopic;
//          topica = fullTopic.c_str();
//          TempValue = "";
//          TempValue = "3";
//          TempValueChar = TempValue.c_str();
//          client.publish(topica, TempValueChar, true);
////          delay(20);
//        }
//        active_phases = 0;
//        if(debug2_MQTT == 1){
//          if (client.connected()){
//            topica = "";
//            dynamicTopic = "";
//        //    epochtimeTopic = getTime();
//            dynamicTopic += prefixTopic;
//            dynamicTopic += idTopic;
//        //    dynamicTopic += "/";
//        //    dynamicTopic += epochtimeTopic;
//            fullTopic = dynamicTopic;
//            fullTopic += ResponseFSTopic;
//            topica = fullTopic.c_str();
//            TempValue = "";
//            TempValue += ResponseMessage;
//            TempValueChar = TempValue.c_str();
//            client.publish(topica, TempValueChar);
//      //      delay(20);
//          }
//        }
//        lastInfo7 = lastInfo7 + 200;
//        lastInfo7 = lastInfo7 - timer7;
//        EnableState = 3;
//        PowerOn = LOW;
//    }
//    t1 = 0;
//
///*    int index = ATMessage.indexOf("$AT");
//    if(index > 1){
//      ATMessage.remove(0, index);
//      ResponseMessage.remove(index);
//    }*/
//    
// /*   if(index > 1){
//      if (client.connected()){
//        topica = "";
//        dynamicTopic = "";
//    //    epochtimeTopic = getTime();
//        dynamicTopic += prefixTopic;
//        dynamicTopic += idTopic;
//    //    dynamicTopic += "/";
//    //    dynamicTopic += epochtimeTopic;
//        fullTopic = dynamicTopic;
//        fullTopic += StateChangeTopic;
//        topica = fullTopic.c_str();
//        TempValue = "";
//        TempValue += ATMessage;
//        TempValueChar = TempValue.c_str();
//        client.publish(topica, TempValueChar);
//  //      delay(20);
//      }
//    }*/
//  }
//}


void ChargeChanger(){
  if(ConnectionTimeoutFlag == LOW){
    if(SetChargeFlag == HIGH){
      debuglog += "$";
      debuglog += "Changing enable/charging state to : ";
      debuglog += tmp;
      debuglog += "$";
      switch(tmp){
        case 1:
          PowerOn = LOW;
          SetRuntimeSettings();
          TurnOff();
        break;
        case 2:
          c2 = 0;
          c3 = 0;
          c4 = 0;
          c5 = 0;
          PowerOn = HIGH;
          SetRuntimeSettings(); 
          TurnOn();
        break;
        case 3:
          PowerOn = LOW;
          SetRuntimeSettings();
          TurnSleep();
        break;
      }
      SetChargeFlag = LOW;
    }
  }
}


void SetMQTTCurrent(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    if((SetMQTTCurrentFlag == HIGH) && (MQTTmax_current < max_current)){
        debuglog += "$";
        debuglog += "Set new charging current received from MQTT : ";
        debuglog += MQTTmax_current;
        debuglog += "$";
        COMused = HIGH;
        ResponseStatusSetMQTTCurrent = LOW;
        if(Serial2.available() > 0){
          CatchStateChange();
        }
        if(MQTTmax_current >= min_current){
          ResponseMessage = "";
          TempValue = "";
          TempValue += SC;
          TempValue += MQTTmax_current;
          Serial2.println(TempValue);
          LastCom = "SetCurrentMQTT(";
          LastCom += TempValue;
          LastCom += ")";
          
          Requestmillis = millis();
        }
    }   
  }
}


void ResponseHandleSetMQTTCurrent(){
  if(ResponseStatusSetMQTTCurrent == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusSetMQTTCurrent = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
           /*         if(index > 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
           /*         if(index > 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusSetMQTTCurrent == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
            ResponseStatusSetMQTTCurrent == HIGH;
            COMused = LOW;
            if(SetMQTTCurrentFlag == HIGH){
              SetMQTTCurrentFlag = LOW;
            }
        }
}


//void SetMQTTCurrent(){
//  if(ConnectionTimeoutFlag == LOW){
//    if((SetMQTTCurrentFlag == HIGH) && (MQTTmax_current < max_current)){
//        debuglog += "$";
//        debuglog += "Set new charging current received from MQTT : ";
//        debuglog += MQTTmax_current;
//        debuglog += "$";
//        ResponseStatus = LOW;
//        if(Serial2.available() > 0){
//          CatchStateChange();
//        }
//        if(MQTTmax_current >= min_current){
//          ResponseMessage = "";
//          TempValue = "";
//          TempValue += SC;
//          TempValue += MQTTmax_current;
//          Serial2.println(TempValue);
//          LastCom = "SetCurrentMQTT(";
//          LastCom += TempValue;
//          LastCom += ")";
//          t1 = 0;
//          while(ResponseStatus == LOW && t1 < timer13){
//            if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//            t1 = t1 + 1;
//            delayMicroseconds(50);
//          }
//          if(t1 > timer13/2){
//            if(debug1_MQTT == 1){
//              if (client.connected()){
//                topica = "";
//                dynamicTopic = "";
//        //        epochtimeTopic = getTime();
//                dynamicTopic += prefixTopic;
//                dynamicTopic += idTopic;
//        //        dynamicTopic += "/";
//        //        dynamicTopic += epochtimeTopic;
//                fullTopic = dynamicTopic;
//                fullTopic += TimeoutTopic;
//                topica = fullTopic.c_str();
//                TempValue = "";
//                TempValue += t1;
//                TempValueChar = TempValue.c_str();
//                client.publish(topica, TempValueChar);
//      //          delay(20);
//              }
//            }
//          }
//        if(t1 > timer13 - 1){
//          if(LoRaCERetryCount == LoRaCERetries){
//            if(debug1 == 1){
//              Serial.print("timeout flag ON, t1 = ");
//              Serial.println(t1);
//            }
//            ConnectionTimeoutFlag = HIGH;
//          }else{
//            LoRaCERetries = LoRaCERetries + 1;
//          }
//        }else{
//            LoRaCERetries = 0;
//            if(debug1 == 1){
//              Serial.print("timeout flag OFF, t1 = ");
//              Serial.println(t1);
//            }
//            ConnectionTimeoutFlag = LOW;
//            t1 = 0;
//            
//  /*          int index = ATMessage.indexOf("$AT");
//            if(index > 1){
//              ATMessage.remove(0, index);
//              ResponseMessage.remove(index);
//            }*/
//            if(debug2_MQTT == 1){
//              if (client.connected()){
//                topica = "";
//                dynamicTopic = "";
//            //    epochtimeTopic = getTime();
//                dynamicTopic += prefixTopic;
//                dynamicTopic += idTopic;
//            //    dynamicTopic += "/";
//            //    dynamicTopic += epochtimeTopic;
//                fullTopic = dynamicTopic;
//                fullTopic += ResponseSCTopic;
//                topica = fullTopic.c_str();
//                TempValue = "";
//                TempValue += ResponseMessage;
//                TempValueChar = TempValue.c_str();
//                client.publish(topica, TempValueChar);
//      //          delay(20);
//              }
//            }
//   /*         if(index > 1){
//              if (client.connected()){
//                topica = "";
//                dynamicTopic = "";
//            //    epochtimeTopic = getTime();
//                dynamicTopic += prefixTopic;
//                dynamicTopic += idTopic;
//            //    dynamicTopic += "/";
//            //    dynamicTopic += epochtimeTopic;
//                fullTopic = dynamicTopic;
//                fullTopic += StateChangeTopic;
//                topica = fullTopic.c_str();
//                TempValue = "";
//                TempValue += ATMessage;
//                TempValueChar = TempValue.c_str();
//                client.publish(topica, TempValueChar);
//          //      delay(20);
//              }
//            }*/
//            SetMQTTCurrentFlag = LOW;
//            SENDBreakerAlt();
//          }
//      }
//    }
//    if(SetMQTTCurrentFlag == HIGH){
//      SetMQTTCurrentFlag = LOW;
//    }
//  }
//}


void SetCurrent(){
    if(SetCurrentFlag == HIGH && COMused == LOW){
      debuglog += "$";
      debuglog += "Set new charging current : ";
      debuglog += max_current;
      debuglog += "$";
      ResponseStatusSetCurrent = LOW;
      COMused = HIGH;
      c5 = 0;
      if(Serial2.available() > 0){
        CatchStateChange();
      }
      if(MQTTmax_current < max_current){
        max_current = MQTTmax_current;
      }
      if(max_current >= min_current){
          ResponseMessage = "";
          TempValue = "";
          TempValue += SC;
          TempValue += max_current;
          Serial2.println(TempValue);
          LastCom = "SetCurrent(";
          LastCom += TempValue;
          LastCom += ")";
          
          Requestmillis = millis();
       }
    }   
}


void ResponseHandleSetCurrent(){
  if(ResponseStatusSetCurrent == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusSetCurrent = HIGH;
            SetCurrentFlag = LOW;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    int index = ATMessage.indexOf("OK ");
                    if(index > 0.5){
                      ATMessage.remove(0, index+3);
                      index = ATMessage.indexOf("^");
                      ATMessage.remove(index);
                      int ATcurrent = ATMessage.toInt();
                      if(ATcurrent == max_current){
                        lastInfo5 = millis();
                      }
                    }
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
             /*       if(index > 1){
                      if (client.connected()){
                          topica = "";
                          dynamicTopic = "";
                      //    epochtimeTopic = getTime();
                          dynamicTopic += prefixTopic;
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
                    
                    set_current = max_current;
                    SetCurrentFlag = LOW;
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    int index = ATMessage.indexOf("OK ");
                    if(index > 0.5){
                      ATMessage.remove(0, index+3);
                      index = ATMessage.indexOf("^");
                      ATMessage.remove(index);
                      int ATcurrent = ATMessage.toInt();
                      if(ATcurrent == max_current){
                        lastInfo5 = millis();
                      }
                    }
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    }
             /*       if(index > 1){
                      if (client.connected()){
                          topica = "";
                          dynamicTopic = "";
                      //    epochtimeTopic = getTime();
                          dynamicTopic += prefixTopic;
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
                    
                    set_current = max_current;
                    SetCurrentFlag = LOW;
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusSetCurrent == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
            ResponseStatusSetCurrent == HIGH;
            COMused = LOW;
            SetCurrentFlag = LOW;
        }
}


//void SetCurrent(){
////  Serial.println("Set Current function");
//  if(SetCurrentFlag == HIGH){
////    Serial.println("Set Current flag is HIGH");
//      debuglog += "$";
//      debuglog += "Set new charging current : ";
//      debuglog += max_current;
//      debuglog += "$";
//      ResponseStatus = LOW;
//      c5 = 0;
//      if(Serial2.available() > 0){
//        CatchStateChange();
//      }
//      if(MQTTmax_current < max_current){
//        max_current = MQTTmax_current;
//      }
//      if(max_current >= min_current){
//          ResponseMessage = "";
//          TempValue = "";
//          TempValue += SC;
//          TempValue += max_current;
//          Serial2.println(TempValue);
//          LastCom = "SetCurrent(";
//          LastCom += TempValue;
//          LastCom += ")";
//          t1 = 0;
//          while(ResponseStatus == LOW && t1 < timer13){
//            if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//            t1 = t1 + 1;
//            delayMicroseconds(50);
//          }
//          if(t1 > timer13/2){
//            if(debug1_MQTT == 1){
//              if (client.connected()){
//                topica = "";
//                dynamicTopic = "";
//        //        epochtimeTopic = getTime();
//                dynamicTopic += prefixTopic;
//                dynamicTopic += idTopic;
//        //        dynamicTopic += "/";
//        //        dynamicTopic += epochtimeTopic;
//                fullTopic = dynamicTopic;
//                fullTopic += TimeoutTopic;
//                topica = fullTopic.c_str();
//                TempValue = "";
//                TempValue += t1;
//                TempValueChar = TempValue.c_str();
//                client.publish(topica, TempValueChar);
//      //          delay(20);
//              }
//            }
//          }
//          if(t1 > timer13 - 1){
//            if(LoRaCERetryCount == LoRaCERetries){
//              if(debug1 == 1){
//                Serial.print("timeout flag ON, t1 = ");
//                Serial.println(t1);
//              }
//              ConnectionTimeoutFlag = HIGH;
//            }else{
//              LoRaCERetries = LoRaCERetries + 1;
//            }
//          }else{
//            LoRaCERetries = 0;
//            if(debug1 == 1){
//              Serial.print("timeout flag OFF, t1 = ");
//              Serial.println(t1);
//            }
//            ConnectionTimeoutFlag = LOW;
//            t1 = 0;
//    
//            int index = ATMessage.indexOf("OK ");
//            if(index > 0.5){
//              ATMessage.remove(0, index+3);
//              index = ATMessage.indexOf("^");
//              ATMessage.remove(index);
//              int ATcurrent = ATMessage.toInt();
//              if(ATcurrent == max_current){
//                lastInfo5 = millis();
//              }
//            }
//            if(debug2_MQTT == 1){
//              if (client.connected()){
//                topica = "";
//                dynamicTopic = "";
//            //    epochtimeTopic = getTime();
//                dynamicTopic += prefixTopic;
//                dynamicTopic += idTopic;
//            //    dynamicTopic += "/";
//            //    dynamicTopic += epochtimeTopic;
//                fullTopic = dynamicTopic;
//                fullTopic += ResponseSCTopic;
//                topica = fullTopic.c_str();
//                TempValue = "";
//                TempValue += ResponseMessage;
//                TempValueChar = TempValue.c_str();
//                client.publish(topica, TempValueChar);
//      //          delay(20);
//              }
//            }
//     /*       if(index > 1){
//              if (client.connected()){
//                  topica = "";
//                  dynamicTopic = "";
//              //    epochtimeTopic = getTime();
//                  dynamicTopic += prefixTopic;
//                  dynamicTopic += idTopic;
//              //    dynamicTopic += "/";
//              //    dynamicTopic += epochtimeTopic;
//                  fullTopic = dynamicTopic;
//                  fullTopic += StateChangeTopic;
//                  topica = fullTopic.c_str();
//                  TempValue = "";
//                  TempValue += ATMessage;
//                  TempValueChar = TempValue.c_str();
//                  client.publish(topica, TempValueChar);
//            //      delay(20);
//                }
//            }*/
//            
//            set_current = max_current;
//          }
//      }
//  SetCurrentFlag = LOW;
//  }
//}

void CurrentFlagSet(){
  if(max_current < set_current){
    SetCurrentFlag = HIGH;
    debuglog += "$";
    debuglog += "Max current Calculated < Max current Set --> Lower charging power";
    debuglog += "$";
    c5 = 0;
  }
  if((max_current) > (set_current + 1)){
    c5 = c5 +1;
//    Serial.print("naracunan tok je vecji kot ga dovolimo : ");
//    Serial.println(c5);
    if(c5 == 50){
      if(debug6 == 1){
        Serial.println("c5 je 50, povecujem moc");
      }
      debuglog += "$";
      debuglog += "c5 je 50, povecujem moc";
      debuglog += "$";
      SetCurrentFlag = HIGH;
      c5 = 0;
    }
  }
}

void SetTimer(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    if(SetTimerFlag == HIGH){
        COMused = HIGH;
        debuglog += "$";
        debuglog += "Setting time Limit :";
        debuglog += TimeLimit;
        debuglog += "$";
        ResponseStatusSetTimer = LOW;
        if(Serial2.available() > 0){
          CatchStateChange();
        }
        ResponseMessage = "";
        TempValue = "";
        TempValue += ST;
        TempValue += TimeLimit;
        Serial2.println(TempValue);
        LastCom = "SetTimer(";
        LastCom += TempValue;
        LastCom += ")";
        
        Requestmillis = millis();
    }
  }
}

void ResponseHandleSetTimer(){
  if(ResponseStatusSetTimer == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusSetTimer = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    SetEnergyLimitFlag = LOW;
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    SetEnergyLimitFlag = LOW;
                    if(debug2_MQTT == 1){
                      if (client.connected()){
                        topica = "";
                        dynamicTopic = "";
                    //    epochtimeTopic = getTime();
                        dynamicTopic += prefixTopic;
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
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusSetTimer == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
           SetTimerFlag = LOW;
           if (client.connected()){
              topica = "";
              dynamicTopic = "";
          //    epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
          //    dynamicTopic += "/";
          //    dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += ResponseSHTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += "No response from EVSE";
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
      //        delay(20);
            }
            ResponseStatusSetTimer == HIGH;
            COMused = LOW;
        }
}

//void SetTimer(){
//  if(ConnectionTimeoutFlag == LOW){
//    if(SetTimerFlag == HIGH){
//        debuglog += "$";
//        debuglog += "Setting time Limit :";
//        debuglog += TimeLimit;
//        debuglog += "$";
//        ResponseStatus = LOW;
//        if(Serial2.available() > 0){
//          CatchStateChange();
//        }
//        ResponseMessage = "";
//        TempValue = "";
//        TempValue += ST;
//        TempValue += TimeLimit;
//        Serial2.println(TempValue);
//        LastCom = "SetTimer(";
//        LastCom += TempValue;
//        LastCom += ")";
//        t1 = 0;
//        while(ResponseStatus == LOW && t1 < timer13){
//          if(Serial2.available()){
//            ResponseStatus = HIGH;
//            ResponseMessageTemp = Serial2.readString();
//            int startIndex = -1;
//            String ResponseMid1 = "";
//            String ResponseMid2 = "";
//            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
//              if (ResponseMessageTemp.charAt(i) == '$') {
//                if (startIndex >= 0) {
//                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
//                  if (ResponseMid1.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid1;
//                    CatchStateChange();
//                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid1;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                  }
//                  // do something with the single message
//                }
//                startIndex = i;
//              }
//            }
//            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
//              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
//              if (ResponseMid2.startsWith("$AT")) {
//                    ResponseMessageAsync = ResponseMid2;
//                    CatchStateChange();
//                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
//                    ResponseMessage = ResponseMid2;
//                    ATMessage = ResponseMessage;
//                    // do something with the message
//                }
//                // do something with the last single message
//              }
//            }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
//        }
//        if(t1 > timer13/2){
//          if(debug1_MQTT == 1){
//            if (client.connected()){
//              topica = "";
//              dynamicTopic = "";
//      //        epochtimeTopic = getTime();
//              dynamicTopic += prefixTopic;
//              dynamicTopic += idTopic;
//      //        dynamicTopic += "/";
//      //        dynamicTopic += epochtimeTopic;
//              fullTopic = dynamicTopic;
//              fullTopic += TimeoutTopic;
//              topica = fullTopic.c_str();
//              TempValue = "";
//              TempValue += t1;
//              TempValueChar = TempValue.c_str();
//              client.publish(topica, TempValueChar);
//    //          delay(20);
//            }
//          }
//        }
//        if(t1 > timer13 - 1){
//          if(LoRaCERetryCount == LoRaCERetries){
//            if(debug1 == 1){
//              Serial.print("timeout flag ON, t1 = ");
//              Serial.println(t1);
//            }
//            ConnectionTimeoutFlag = HIGH;
//          }else{
//            LoRaCERetries = LoRaCERetries + 1;
//          }
//        }else{
//          LoRaCERetries = 0;
//          if(debug1 == 1){
//              Serial.print("timeout flag OFF, t1 = ");
//              Serial.println(t1);
//          }
//          ConnectionTimeoutFlag = LOW;
//          SetTimerFlag = LOW;
//        }
//        t1 = 0;
//        if(debug2_MQTT == 1){
//          if (client.connected()){
//            topica = "";
//            dynamicTopic = "";
//        //    epochtimeTopic = getTime();
//            dynamicTopic += prefixTopic;
//            dynamicTopic += idTopic;
//        //    dynamicTopic += "/";
//        //    dynamicTopic += epochtimeTopic;
//            fullTopic = dynamicTopic;
//            fullTopic += ResponseSTTopic;
//            topica = fullTopic.c_str();
//            TempValue = "";
//            TempValue += ResponseMessage;
//            TempValueChar = TempValue.c_str();
//            client.publish(topica, TempValueChar);
//    //        delay(20);
//          }
//        }
//    }  
//  }
//}

void SetLimit(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    if(SetEnergyLimitFlag == HIGH){
        COMused = HIGH;
        debuglog += "$";
        debuglog += "Setting energy Limit :";
        debuglog += EnergyLimit;
        debuglog += "$";
        ResponseStatusSetLimit = LOW;
        if(Serial2.available() > 0){
          CatchStateChange();
        }
        ResponseMessage = "";
        TempValue = "";
        TempValue += SH;
        TempValue += EnergyLimit;
        Serial2.println(TempValue);
        LastCom = "SetLimit(";
        LastCom += TempValue;
        LastCom += ")";
        
        Requestmillis = millis();
    }
  }
}

void ResponseHandleSetLimit(){
  if(ResponseStatusSetLimit == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusSetLimit = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    SetEnergyLimitFlag = LOW;
                    if (client.connected()){
                      topica = "";
                      dynamicTopic = "";
                  //    epochtimeTopic = getTime();
                      dynamicTopic += prefixTopic;
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
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    SetEnergyLimitFlag = LOW;
                    if (client.connected()){
                      topica = "";
                      dynamicTopic = "";
                  //    epochtimeTopic = getTime();
                      dynamicTopic += prefixTopic;
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
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusSetLimit == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
           if (client.connected()){
              topica = "";
              dynamicTopic = "";
          //    epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
          //    dynamicTopic += "/";
          //    dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += ResponseSHTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += "No response from EVSE";
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
      //        delay(20);
            }
            ResponseStatusSetLimit == HIGH;
            COMused = LOW;
        }
}
        

void AskRAPIF(){
  if(ConnectionTimeoutFlag == LOW && COMused == LOW){
    if(AskRAPI == HIGH){
        COMused = HIGH;
        if(debug6 == 1){
          Serial.println("asking RAPI");
        }
        debuglog += "$";
        debuglog += "Sending RAPI command :";
        debuglog += RAPI;
        debuglog += "$";
        ResponseStatusRAPIF = LOW;
        if(Serial2.available() > 0){
          CatchStateChange();
        }
        ResponseMessage = "";
        TempValue = "";
        TempValue += "$";
        TempValue += RAPI;
        Serial2.println(TempValue);
        Serial2.flush();
        LastCom = "AskRAPI(";
        LastCom += TempValue;
        LastCom += ")";

        Requestmillis = millis();
    }    
  }
}

void ResponseHandleRAPIF(){
  if(ResponseStatusRAPIF == LOW && millis()-Requestmillis < timer13){
          if(Serial2.available()){
            ResponseStatusRAPIF = HIGH;
            ResponseMessageTemp = Serial2.readString();
            int startIndex = -1;
            String ResponseMid1 = "";
            String ResponseMid2 = "";
            for (int i = 0; i < ResponseMessageTemp.length(); i++) {
              if (ResponseMessageTemp.charAt(i) == '$') {
                if (startIndex >= 0) {
                  ResponseMid1 = ResponseMessageTemp.substring(startIndex, i);
                  if (ResponseMid1.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid1;
                    CatchStateChange();
                  } else if (ResponseMid1.startsWith("$OK") || ResponseMid1.startsWith("$NK")) {
                    ResponseMessage = ResponseMid1;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    AskRAPI = LOW;
                    if (client.connected()){
                      topica = "";
                      dynamicTopic = "";
                  //    epochtimeTopic = getTime();
                      dynamicTopic += prefixTopic;
                      dynamicTopic += idTopic;
                  //    dynamicTopic += "/";
                  //    dynamicTopic += epochtimeTopic;
                      fullTopic = dynamicTopic;
                      fullTopic += RapiTopic;
                      fullTopic += RAPI;
                      topica = fullTopic.c_str();
                      TempValue = "";
                      TempValue += ResponseMessage;
                      TempValueChar = TempValue.c_str();
                      client.publish(topica, TempValueChar);
              //        delay(20);
                    }
                    COMused = LOW;
                    // do something with the message
                  }
                  // do something with the single message
                }
                startIndex = i;
              }
            }
            if (startIndex >= 0 && startIndex < ResponseMessageTemp.length() - 1) {
              ResponseMid2 = ResponseMessageTemp.substring(startIndex);
              if (ResponseMid2.startsWith("$AT")) {
                    ResponseMessageAsync = ResponseMid2;
                    CatchStateChange();
                  } else if (ResponseMid2.startsWith("$OK") || ResponseMid2.startsWith("$NK")) {
                    ResponseMessage = ResponseMid2;
                    ATMessage = ResponseMessage;
                    LoRaCERetries = 0;
                    if(debug1 == 1){
                        Serial.print("timeout flag OFF, t1 = ");
                        Serial.println(t1);
                    }
                    ConnectionTimeoutFlag = LOW;
                    AskRAPI = LOW;
                    if (client.connected()){
                      topica = "";
                      dynamicTopic = "";
                  //    epochtimeTopic = getTime();
                      dynamicTopic += prefixTopic;
                      dynamicTopic += idTopic;
                  //    dynamicTopic += "/";
                  //    dynamicTopic += epochtimeTopic;
                      fullTopic = dynamicTopic;
                      fullTopic += RapiTopic;
                      fullTopic += RAPI;
                      topica = fullTopic.c_str();
                      TempValue = "";
                      TempValue += ResponseMessage;
                      TempValueChar = TempValue.c_str();
                      client.publish(topica, TempValueChar);
              //        delay(20);
                    }
                    COMused = LOW;
                    // do something with the message
                }
                // do something with the last single message
              }
//          t1 = t1 + 1;
//          delayMicroseconds(50);
          }
        }else if(ResponseStatusRAPIF == LOW && millis()-Requestmillis > timer13){
          if(debug1_MQTT == 1){
            if (client.connected()){
              topica = "";
              dynamicTopic = "";
      //        epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
      //        dynamicTopic += "/";
      //        dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += TimeoutTopic;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += timer13;
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
    //          delay(20);
            }
          }
          if(LoRaCERetryCount == LoRaCERetries){
            if(debug1 == 1){
              Serial.print("timeout flag ON, t1 = ");
              Serial.println(t1);
            }
            ConnectionTimeoutFlag = HIGH;
           }else{
            LoRaCERetries = LoRaCERetries + 1;
           }
           if (client.connected()){
              topica = "";
              dynamicTopic = "";
          //    epochtimeTopic = getTime();
              dynamicTopic += prefixTopic;
              dynamicTopic += idTopic;
          //    dynamicTopic += "/";
          //    dynamicTopic += epochtimeTopic;
              fullTopic = dynamicTopic;
              fullTopic += RapiTopic;
              fullTopic += RAPI;
              topica = fullTopic.c_str();
              TempValue = "";
              TempValue += "No response from EVSE";
              TempValueChar = TempValue.c_str();
              client.publish(topica, TempValueChar);
      //        delay(20);
            }
            ResponseStatusRAPIF == HIGH;
            COMused = LOW;
        }
}

void SaveLastCurrents(){
  LastCurrent1 = average1;
  debuglog += "$";
  debuglog += "Last saved current 1 :";
  debuglog += LastCurrent1;
  debuglog += "$";
  LastCurrent2 = average2;
  debuglog += "$";
  debuglog += "Last saved current 2 :";
  debuglog += LastCurrent2;
  debuglog += "$";
  LastCurrent3 = average3;
  debuglog += "$";
  debuglog += "Last saved current 3 :";
  debuglog += LastCurrent3;
  debuglog += "$";
}

void CheckPhaseChange(){
  if(PhaseInfo == LOW){
//    Serial.println("PhaseCheck");
    debuglog += "$";
    debuglog += "PhaseCheck";
    debuglog += "$";
    Current1Change = average1 - LastCurrent1;
    Current2Change = average2 - LastCurrent2;
    Current3Change = average3 - LastCurrent3;
//    Serial.print("changes :  ");
//    Serial.print(Current1Change);
//    Serial.print("  ;  ");
//    Serial.print(Current2Change);
//    Serial.print("  ;  ");
//    Serial.println(Current3Change);
//    Serial.print("charge current :  ");
//    Serial.println(charge_current);
    if(charge_current > 3){
      debuglog += "$";
      debuglog += "Charge current > 6A";
      debuglog += "$";
      if(Current1Change >= charge_current*0.4){
        active_phases = 1;
        debuglog += "$";
        debuglog += "Current change on phase1 detected";
        debuglog += "$";
        if(Current2Change >= charge_current*0.4){
          active_phases = 12;
          debuglog += "$";
          debuglog += "Current change on phase2 detected";
          debuglog += "$";
          if(Current3Change >= charge_current*0.4){
            active_phases = 123;
            debuglog += "$";
            debuglog += "Current change on phase3 detected";
            debuglog += "$";
          }
        }else if(Current3Change >= charge_current*0.4){
          active_phases = 13;
          debuglog += "$";
          debuglog += "Current change on phase3 detected";
          debuglog += "$";
        }
      }else if(Current2Change >= charge_current*0.4){
        active_phases = 2;
        debuglog += "$";
        debuglog += "Current change on phase2 detected";
        debuglog += "$";
        if(Current3Change >= charge_current*0.4){
          active_phases = 23;
          debuglog += "$";
          debuglog += "Current change on phase3 detected";
          debuglog += "$";
        }
      }else if(Current3Change >= charge_current*0.4){
        active_phases = 3;
        debuglog += "$";
        debuglog += "Current change on phase3 detected";
        debuglog += "$";
      }
    }else if(charge_current < 0.5){
      debuglog += "$";
      debuglog += "Charge current < 0.5A";
      debuglog += "$";
      if(Current2Change >= 4){
        active_phases = 20;
        debuglog += "$";
        debuglog += "Current change on phase2 >=5A detected";
        debuglog += "$";
        if(Current3Change >= 4){
          active_phases = 23;
          debuglog += "$";
          debuglog += "Current change on phase3 >=5A detected";
          debuglog += "$";
        }
      }else if(Current3Change >= 4){
        active_phases = 30;
        debuglog += "$";
        debuglog += "Current change on phase3 >=5A detected";
        debuglog += "$";
      }
    }else{
      active_phases = 99;
      debuglog += "$";
      debuglog += "Charging current is between 0.5A - 6A";
      debuglog += "$";
    }
    if(active_phases != 0){
      PhaseInfo = HIGH;
      debuglog += "$";
      debuglog += "Phases calculated";
      debuglog += "$";
    }
  }
}

void PACChargeCheck(){
  if(PAndC == HIGH){
    if(State == 1 && (charge_current > 3 || EnableState == 2)){
      PowerOn = HIGH;
    }else{
      PowerOn = LOW;
    }
  }
}


void StopCharge(){
  if(ConnectionTimeoutFlag == LOW){
    if((ChargeSetState == HIGH || PowerOn == HIGH) && PAndC == LOW){
      if((State != 0) && (State != 2)){
        c6 = 0;
      }else{
        c6 = c6 + 1;
        if(c6 > timer2){
          if(debug6 == 1){
            Serial.println("Izklop, state");
          }
          debuglog += "$";
          debuglog += "Izklop, state  ";
          debuglog += State;
          debuglog += "$";
          PowerOn = LOW;
          SetRuntimeSettings();
          TurnSleep();
        }
      }
      if(charge_current > 0.3){
        c2 = 0;
      }else{
        c2 = c2+1;
        if(c2 > timer1){
          if(debug6 == 1){
            Serial.println("Izklop, premali tok");
          }
          debuglog += "$";
          debuglog += "Izklop, premali tok";
          debuglog += "$";
          PowerOn = LOW;
          SetRuntimeSettings();
          TurnSleep();
        }
      }
    }
    if(ChargeSetState == HIGH && PowerOn == HIGH){
      if(charge_current <= max_current + 0.5){
        c3 = 0; 
      }else{
        c3 = c3+1;
        if(c3 > timer3){
          if(debug6 == 1){
            Serial.println("Izklop, prevelik tok");
          }
          debuglog += "$";
          debuglog += "Izklop, prevelik tok";
          debuglog += "$";
          PowerOn = LOW;
          SetRuntimeSettings();
          TurnSleep();
        }     
      }
      if(max_current >= min_current){
        c4 = 0;
      }else{
        c4 = c4+1;
        if(c4 > timer4){
          if(debug6 == 1){
            Serial.println("Izklop, ni na voljo dovolj toka");
          }
          debuglog += "$";
          debuglog += "Izklop, ni na voljo dovolj toka";
          debuglog += "$";
          TurnSleep();
        }
      }
    }
    if(PAndC == LOW && PowerOn == LOW && ChargeSetState == LOW && (charge_current > 0.3) && tmp != 3){
      if(debug6 == 1){
        Serial.println("Izklop, stanje ne ustreza 2");
      }
      debuglog += "$";
      debuglog += "Izklop, stanje ne ustreza 2  ";
      debuglog += PowerOn;
      debuglog += "$";
      PowerOn = LOW;
      SetRuntimeSettings();
      TurnSleep();
    }
    if((PAndC == HIGH || PowerOn == HIGH) && ChargeSetState == LOW && max_current >= min_current && charge_current == 0){
      if(debug6 == 1){
        Serial.println("vklapljam polnilnico, dovolj toka + P&C ali PowerOn");
      }
      debuglog += "$";
      debuglog += "Vklop, dovolj toka + P&C ali PowerOn";
      debuglog += "$";
      TurnOn();
    }
  }
}


void SendDebugF2(){
  if (client.connected()){
    if(debuglog.length() > 3){
      const char * DebugChar;
      topica = "";
      dynamicTopic = "";
  //    epochtimeTopic = getTime();
      dynamicTopic += prefixTopic;
      dynamicTopic += idTopic;
  //    dynamicTopic += "/";
  //    dynamicTopic += epochtimeTopic;
      fullTopic = dynamicTopic;
      fullTopic += DebugTopic;
      topica = fullTopic.c_str();
      DebugChar = debuglog.c_str();
      client.publish(topica, DebugChar);
  //    delay(20);
      debuglog = "";
    }
  }
}

void CatchStateChange(){
  if (ResponseMessageAsync == "" && Serial2.available() && COMused == LOW) {
    ResponseMessageAsync = Serial2.readString();
  }
  if(ResponseMessageAsync != ""){
    ATMessage = ResponseMessageAsync;
    int index = ATMessage.indexOf("$AT ");
    if(index >= 0){
      ATMessage.remove(0, index+4);
      ATMessage.remove(5);
      if(debug6 == 1){
        Serial.print("Catch state izrez je : ");
        Serial.println(ATMessage);
      }
      if(ATMessage == "01 01"){
        State = 0;
        if (client.connected()){
          if(OldState != State){
            OldState = State;
            fullTopic = dynamicTopic;
            fullTopic += StateTopic;
            topica = fullTopic.c_str();
            TempValue = "";
            TempValue += State;
            TempValueChar = TempValue.c_str();
            client.publish(topica, TempValueChar, true);
            delay(20);
          }
        }
      }else if(ATMessage == "02 02"){
        State = 1;
        if (client.connected()){
          if(OldState != State){
            OldState = State;
            fullTopic = dynamicTopic;
            fullTopic += StateTopic;
            topica = fullTopic.c_str();
            TempValue = "";
            TempValue += State;
            TempValueChar = TempValue.c_str();
            client.publish(topica, TempValueChar, true);
            delay(20);
          }
        }
      }else if(ATMessage == "03 03"){
        State = 1;
        if (client.connected()){
          if(OldState != State){
            OldState = State;
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
      }else if(ATMessage == "02 03"){
        State = 1;
        if (client.connected()){
          if(OldState != State){
            OldState = State;
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
      }
      String ChargeStatevmesna;
      uint8_t ChargeState;
      ChargeStatevmesna = ATMessage;
      ChargeStatevmesna.remove(2);
      ChargeState = ChargeStatevmesna.toInt();
      Serial.println(ChargeState);
      if(PAndC == LOW){
        if(ChargeState != 2 && ChargeState != 3 && ChargeStatevmesna != "fe"){
          debuglog += "$";
          debuglog += "ChargeState:  ";
          debuglog += ChargeState;
          debuglog += "$";
          State = 0;
          PowerOn = LOW;
          Serial.println("Charging state is not 2 or 3");
          SetRuntimeSettings();
          TurnSleep();
        }
      }
      if(ChargeState >= 5 && ChargeState <= 15){
        Serial.print("Fault detected: ");
        Serial.println(ChargeState);
        debuglog += "$";
        debuglog += "Fault detected: ";
        debuglog += ChargeState;
        debuglog += "$";
        PowerOn = LOW;
        Serial.println("Charging state is not 2 or 3");
        SetRuntimeSettings();
        TurnSleep();
      }
      ATMessage.remove(0, 3);
      State = ATMessage.toInt();
      debuglog += "$";
      debuglog += "AT State: ";
      debuglog += State;
      debuglog += "$";
      if(State == 2 || State == 3){
        State = 1;
      }else if(State == 1){
        State = 0;
      }
      if (client.connected()){
          if(OldState != State){
            OldState = State;
            fullTopic = dynamicTopic;
            fullTopic += StateTopic;
            topica = fullTopic.c_str();
            TempValue = "";
            TempValue += State;
            TempValueChar = TempValue.c_str();
            client.publish(topica, TempValueChar, true);
            delay(20);
          }
       }
    }
    if (client.connected()){
            topica = "";
            dynamicTopic = "";
        //    epochtimeTopic = getTime();
            dynamicTopic += prefixTopic;
            dynamicTopic += idTopic;
        //    dynamicTopic += "/";
        //    dynamicTopic += epochtimeTopic;
            fullTopic = dynamicTopic;
            fullTopic += StateChangeTopic;
            topica = fullTopic.c_str();
            TempValue = "";
            TempValue += ResponseMessageAsync;
            TempValueChar = TempValue.c_str();
            client.publish(topica, TempValueChar);
     }
     ResponseMessageAsync = "";
     lastInfo6 = 0;
  }
}

void WiFiReconnect(){
  long now = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
//  Serial.println("WiFi reconnect was called");
  int ssidlength = ssid.length();
  if ((WiFi.status() != WL_CONNECTED) && (now - lastInfo11 >= timer11) && (networkFound == true || networkFound1 == true)) {
    int ssidlength = ssid.length();
    if(debug6 == 1){
      Serial.println("reconnect timer");
      Serial.println(wifi_reconnects);
    }
    if((wifi_reconnects > 25 && wifi_reconnects < 10000) && (networkFound == true || networkFound1 == true)){
//      initWiFi();
      Serial.println("First reconnect function");
      WiFiConnect();
      wifi_reconnects = wifi_reconnects*1.2;
    }else if(ssidlength > 2 && wifi_reconnects < 25){
      if(debug6 == 1){
        Serial.println("Reconnecting to user WiFi...");
      }
      debuglog += "$";
      debuglog += "Reconnecting to user WiFi...";
      debuglog += "$";
      WiFiConnect();
    }else if((networkFound == true || networkFound1 == true) && wifi_reconnects > 10000){
      ESP.restart();
    }
    lastInfo11 = now;
  }else if(WiFi.status() == WL_CONNECTED){
    int ssidlength = ssid.length();
    if((networkFound == true || networkFound1 == true) && r >= 100 && !client.connected()){
      ESP.restart(); 
      r = 0;
    }
  }
}

void WiFiConnect(){
  Serial.println("Try reconnect");
  if(networkFound == true || networkFound1 == true){
    WiFi.disconnect();
    delay(500);
    WiFi.mode(WIFI_STA);
    if(networkFound == true){
      WiFi.begin(ssid.c_str(), pass.c_str());
    }else if(networkFound1 == true){
      WiFi.begin(ssid1, pass1);
    }
    delay(500);
    ip = WiFi.localIP().toString();
    gateway = WiFi.gatewayIP().toString();
    if(ip.length()>1 && WiFi.status() == WL_CONNECTED){
      wifi_reconnects = 0;
      ipSentFlag = LOW;
    }
  }
}


void Task1code( void * pvParameters ){
  while(SetupComplete == LOW){
    vTaskDelay(1000);
  }
  pinMode(ct_sensor_1, INPUT_PULLUP);
  pinMode(ct_sensor_2, INPUT_PULLUP);
  pinMode(ct_sensor_3, INPUT_PULLUP);
  if(calibration > 5){
    switch (CTEnable){
      case 1:
        emon1.current(ct_sensor_1, calibration); // Current: input pin, calibration 102.1 = Arduino pro mini
        break;
      case 2:
        emon2.current(ct_sensor_2, calibration);
        break;
      case 3:
        emon3.current(ct_sensor_3, calibration);
        break;
      case 12:
        emon1.current(ct_sensor_1, calibration);
        vTaskDelay(50);
        emon2.current(ct_sensor_2, calibration);
        break;
      case 23:
        emon2.current(ct_sensor_2, calibration);
        vTaskDelay(50);
        emon3.current(ct_sensor_3, calibration);
        break;
      case 13:
        emon1.current(ct_sensor_1, calibration);
        vTaskDelay(50);
        emon3.current(ct_sensor_3, calibration);
        break;
      case 123:
        emon1.current(ct_sensor_1, calibration);
        vTaskDelay(50);
        emon2.current(ct_sensor_2, calibration);
        vTaskDelay(50);
        emon3.current(ct_sensor_3, calibration);
        break;
    }
  }
  OldState = State;

  for(;;){
    
    if(calibration > 5){
      switch (CTEnable){
        case 1:
          Irms_1 = emon1.calcIrms(1480);  // Calculate Irms only  1480
          break;
        case 2:
          Irms_2 = emon2.calcIrms(1480);
          break;
        case 3:
          Irms_3 = emon3.calcIrms(1480);
          break;
        case 12:
          Irms_1 = emon1.calcIrms(1480);
          Irms_2 = emon2.calcIrms(1480);
          break;
        case 23:
          Irms_2 = emon2.calcIrms(1480);
          Irms_3 = emon3.calcIrms(1480);
          break;
        case 13:
          Irms_1 = emon1.calcIrms(1480);
          Irms_3 = emon3.calcIrms(1480);
          break;
        case 123:
          Irms_1 = emon1.calcIrms(1480);
          Irms_2 = emon2.calcIrms(1480);
          Irms_3 = emon3.calcIrms(1480);
          break;
      }
    }
    vTaskDelay(500);
  } 
}
