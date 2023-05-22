#include "EmonLib.h" // Include Emon Library
#include "math.h"
#include "driver/adc.h"
#include "esp32fota.h"
#include "WiFi.h"
#include "PubSubClient.h"


// Second Core Settings//
TaskHandle_t Task1;
long lastInfo = 0;

// Change to your WiFi credentials
const char *ssid = "Implera01";
const char *password = "iMplera!";

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


const char * topica;
const char * charBreaker;
const char * charUpdate;
const char * charStatus;
String sub_BreakerTopic;
String sub_StatusTopic;
String sub_UpdateTopic;
const char * charEnable;
String sub_EnableTopic;
const char * charTimer;
String sub_TimerTopic;
const char * charEnergyLimit;
String sub_EnergyLimitTopic;
const char * charCurrentLimit;
String sub_CurrentLimitTopic;



boolean SS1;
boolean SS2;
boolean SS3;
boolean SS4;

boolean UpdateStart;
boolean ResponseStatus;

float charge_current;
int16_t max_current;
int16_t MQTTmax_current;
int16_t set_current;
uint16_t breaker;
uint8_t active_phases;
String temp_message;
String TempValue;
const char * TempValueChar;


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


uint8_t tmp;
uint8_t MQTTBreaker;
uint8_t EnergyLimit;
uint8_t Timer;


uint16_t t1;
uint8_t c1 = 0;
uint8_t c2 = 0;
uint8_t c3 = 0;
uint8_t c4 = 0;


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


void setup_wifi()
{
  WiFi.mode(WIFI_OFF);
  delay(3000);
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_MODE_STA);
  delay(3000);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int i;
  while (WiFi.status() != WL_CONNECTED && i<25) {
    delay(500);
    i = i + 1;
    Serial.print(".");
  }
  if(i==25){
    ESP.restart();
  }
  delay(3000);
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
  }

  if (String(topic) == sub_UpdateTopic) {
    Serial.print("New update JSON link: ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    UpdateStart = HIGH;
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
    Timer = temp_message.toInt();
    SetTimerFlag = HIGH;
  }

  if (String(topic) == sub_EnergyLimitTopic) {
    Serial.print("EnergyLimit received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    EnergyLimit = temp_message.toInt();
    SetEnergyLimitFlag = HIGH;
  }

  if (String(topic) == sub_CurrentLimitTopic) {
    Serial.print("CurrentLimit received ");
    Serial.println(messageTemp);
    temp_message = messageTemp;
    MQTTmax_current = temp_message.toInt();
    SetMQTTCurrentFlag = HIGH;
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
      setup_wifi();
    }
    if (client.connect(chardeviceName, "sharelock" , "biciklekomplikovanije")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(charBreaker);
      client.subscribe(charUpdate);
      client.subscribe(charStatus);
      client.subscribe(charEnable);
      client.subscribe(charTimer);
      client.subscribe(charEnergyLimit);
      client.subscribe(charCurrentLimit);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
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
  sub_EnableTopic += prefix;
  sub_EnableTopic += idTopic;
  sub_EnableTopic += "/set_enable";
  charEnable = sub_EnableTopic.c_str();
  sub_TimerTopic += prefix;
  sub_TimerTopic += idTopic;
  sub_TimerTopic += "/set_t_limit";
  charTimer = sub_TimerTopic.c_str();
  sub_EnergyLimitTopic += prefix;
  sub_EnergyLimitTopic += idTopic;
  sub_EnergyLimitTopic += "/set_e_limit";
  charEnergyLimit = sub_EnergyLimitTopic.c_str();
  sub_CurrentLimitTopic += prefix;
  sub_CurrentLimitTopic += idTopic;
  sub_CurrentLimitTopic += "/set_c_limit";
  charCurrentLimit = sub_CurrentLimitTopic.c_str();


  
  
  setup_wifi();
  delay(200);
  client.setServer(mqtt_server, 31883);
  client.setCallback(callback);
  esp32FOTA.checkURL = "http://lockit.pro/ota/Dinamics/Dinamics.json";


  //init and get the time
  configTime(0, 0, ntpServer);


  if (!client.connected()) {
    if (!WiFi.status() == WL_CONNECTED){
      setup_wifi();
    }
    reconnect();
  }
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);

  delay(500);
  
//  DIPswitchCalc();
//  SENDBreaker();
  SENDFWversion();

  Serial.println(FW_version);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(UpdateStart == HIGH){
    boolean updatedNeeded = esp32FOTA.execHTTPcheck();
    if(updatedNeeded == HIGH){
      Serial.print("najden update");
      esp32FOTA.execOTA();
    }
  }


//  total1 = total1 - branja1[readindex1];
//  branja1[readindex1] = Irms_1;
//  total1 = total1 + branja1[readindex1];
//  readindex1 = readindex1 + 1;
//
//  if (readindex1 >= NoRead1){
//    readindex1 = 0;
//  }
//
//  average1 = total1 / NoRead1;
//
//  total2 = total2 - branja2[readindex2];
//  branja2[readindex2] = Irms_2;
//  total2 = total2 + branja2[readindex2];
//  readindex2 = readindex2 + 1;
//
//  if (readindex2 >= NoRead2){
//    readindex2 = 0;
//  }
//
//  average2 = total2 / NoRead2;
//
//  total3 = total3 - branja3[readindex3];
//  branja3[readindex3] = Irms_3;
//  total3 = total3 + branja3[readindex3];
//  readindex3 = readindex3 + 1;
//
//  if (readindex3 >= NoRead3){
//    readindex3 = 0;
//  }
//
//  average3 = total3 / NoRead3;


  Dovoljen_Tok();
  client.loop();
  ChargeChanger();
  client.loop();
  SetMQTTCurrent();
  client.loop();
  ConnectionAlert();
  client.loop();
  CurrentFlagSet();
  client.loop();
  SetCurrent();
  client.loop();
  StopCharge();

  

  
  
  if (!client.connected()) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    if (!WiFi.status() == WL_CONNECTED){
      
      setup_wifi();
    }
    reconnect();
  }else{
    digitalWrite(LED_RED, LOW);
    long now = millis();
    if (now - lastInfo > 5000) {
      digitalWrite(LED_GREEN, HIGH);
      lastInfo = now;
      SENDCurrents();
      client.loop();
      delayMicroseconds(50);
      digitalWrite(LED_GREEN, LOW);
      CheckSetAmps();
      client.loop();
      CheckState();
      client.loop();
      CheckCharge();
      client.loop();
      CheckStatus();
      client.loop();
      CheckEnergy();
      client.loop();
      SetTimer();
      client.loop();
      SetLimit();
      }
  }
  
 client.loop();
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
}

void SENDBreaker(){
  if (client.connected()){
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
    delay(20);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    delay(20);
  }
}

void SENDBreakerAlt(){
  if (client.connected()){
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
    delay(20);
  }
}

void SENDFWversion(){
  if (client.connected()){
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
    delay(20);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    delay(20);
  }
}

void SENDCurrents(){
  if (client.connected()){
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
    delay(20);
  
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
    delay(20);
  
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
    delay(20);
  
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
    delay(20);
  
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
    delay(20);
  
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
    delay(20);
  }
}


void ConnectionAlert(){
  if((ConnectionTimeoutFlag == HIGH) && (ActiveTimeoutFlag == LOW)){
    ActiveTimeoutFlag = HIGH;
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
      delay(20);
    }
  }
  if((ConnectionTimeoutFlag == LOW) && (ActiveTimeoutFlag == HIGH)){
    ActiveTimeoutFlag = LOW;
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
      delay(20);
    }
  }
}

void CheckState(){
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(G0);
    while(ResponseStatus == LOW && t1 < 800){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(500);
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
        delay(20);
      }
    }
    if(t1 > 799){
        ConnectionTimeoutFlag = HIGH;
        if (client.connected()){
          fullTopic = dynamicTopic;
          fullTopic += StateTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += "2";
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar);
          delay(20);
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
          delay(20);
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
          delay(20);
        }
      }
      t1 = 0;
}

void CheckStatus(){
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(GS);
    while(ResponseStatus == LOW && t1 < 800){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(500);
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
        delay(20);
      }
    }
    if(t1 > 799){
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
      delay(20);
    }
}


void CheckSetAmps(){
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(GC);
    while(ResponseStatus == LOW && t1 < 800){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(500);
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
        delay(20);
      }
    }
    if(t1 > 799){
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
      delay(20);
    }
}

void CheckCharge(){
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(GG);
    while(ResponseStatus == LOW && t1 < 800){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(500);
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
        delay(20);
      }
    }
    if(t1 > 799){
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
      delay(20);
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
}

void CheckEnergy(){
   ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(GU);
    while(ResponseStatus == LOW && t1 < 800){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(500);
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
        delay(20);
      }
    }
    if(t1 > 799){
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
      delay(20);
    }
}


void TurnOn(){
    ChargeSetState = HIGH;
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(FE);
    SaveLastCurrents();
    while(ResponseStatus == LOW && t1 < 800){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(500);
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
        delay(20);
      }
    }
    if(t1 > 799){
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
          TempValue = "2";
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar, true);
          delay(20);
        }
        PhaseInfo = LOW;
        c1 = 0;
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
      fullTopic += ResponseFETopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += ResponseMessage;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      delay(20);
    }
}
void TurnOff(){
    ChargeSetState = LOW;
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(FD);
    while(ResponseStatus == LOW && t1 < 800){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(500);
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
        delay(20);
      }
    }
    if(t1 > 799){
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
          delay(20);
        }
        active_phases = 0;
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
      fullTopic += ResponseFDTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += ResponseMessage;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      delay(20);
    }
}
void TurnSleep(){
    ChargeSetState = LOW;
    ResponseStatus = LOW;
    while(Serial.available() > 0) {
      char t = Serial.read();
    }
    ResponseMessage = "";
    Serial.println(FS);
    while(ResponseStatus == LOW && t1 < 800){
      if(Serial.available()){
        ResponseStatus = HIGH;
        ResponseMessage = Serial.readString();
      }
      t1 = t1 + 1;
      delayMicroseconds(500);
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
        delay(20);
      }
    }
    if(t1 > 799){
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
          TempValue = "3";
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar, true);
          delay(20);
        }
        active_phases = 0;
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
      fullTopic += ResponseFSTopic;
      topica = fullTopic.c_str();
      TempValue = "";
      TempValue += ResponseMessage;
      TempValueChar = TempValue.c_str();
      client.publish(topica, TempValueChar);
      delay(20);
    }
}


void ChargeChanger(){
  if(SetChargeFlag == HIGH){
    switch(tmp){
      case 1:
        TurnOff();
      break;
      case 2:
        TurnOn();
      break;
      case 3:
        TurnSleep();
      break;
    }
    SetChargeFlag = LOW;
  }
}


void SetMQTTCurrent(){
  if((SetMQTTCurrentFlag == HIGH) && (MQTTmax_current < max_current)){
      ResponseStatus = LOW;
      while(Serial.available() > 0) {
        char t = Serial.read();
      }
      ResponseMessage = "";
      TempValue = "";
      TempValue += SC;
      TempValue += MQTTmax_current;
      Serial.println(TempValue);
      while(ResponseStatus == LOW && t1 < 800){
        if(Serial.available()){
          ResponseStatus = HIGH;
          ResponseMessage = Serial.readString();
        }
        t1 = t1 + 1;
        delayMicroseconds(500);
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
          delay(20);
        }
      }
      if(t1 > 799){
        ConnectionTimeoutFlag = HIGH;
      }else{
        ConnectionTimeoutFlag = LOW;
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
          fullTopic += ResponseSCTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += ResponseMessage;
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar);
          delay(20);
        }
        SetMQTTCurrentFlag = LOW;
        SENDBreakerAlt();
      }
  }
  if(SetMQTTCurrentFlag == HIGH){
    SetMQTTCurrentFlag = LOW;
  }
}

void SetCurrent(){
  if(SetCurrentFlag == HIGH){
      ResponseStatus = LOW;
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
      while(ResponseStatus == LOW && t1 < 800){
        if(Serial.available()){
          ResponseStatus = HIGH;
          ResponseMessage = Serial.readString();
        }
        t1 = t1 + 1;
        delayMicroseconds(500);
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
          delay(20);
        }
      }
      if(t1 > 799){
        ConnectionTimeoutFlag = HIGH;
      }else{
        ConnectionTimeoutFlag = LOW;
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
          fullTopic += ResponseSCTopic;
          topica = fullTopic.c_str();
          TempValue = "";
          TempValue += ResponseMessage;
          TempValueChar = TempValue.c_str();
          client.publish(topica, TempValueChar);
          delay(20);
        }
        SetCurrentFlag = LOW;
        set_current = max_current;
      }
  }
}

void CurrentFlagSet(){
  if(max_current < set_current){
    SetCurrentFlag = HIGH;
  }
  if((max_current) > (set_current + 1)){
    SetCurrentFlag = HIGH;
  }
}

void SetTimer(){
  if(SetTimerFlag == HIGH){
      ResponseStatus = LOW;
      while(Serial.available() > 0) {
        char t = Serial.read();
      }
      ResponseMessage = "";
      TempValue = "";
      TempValue += ST;
      TempValue += Timer;
      Serial.println(TempValue);
      while(ResponseStatus == LOW && t1 < 800){
        if(Serial.available()){
          ResponseStatus = HIGH;
          ResponseMessage = Serial.readString();
        }
        t1 = t1 + 1;
        delayMicroseconds(500);
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
          delay(20);
        }
      }
      if(t1 > 799){
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
        delay(20);
      }
  }  
}

void SetLimit(){
  if(SetEnergyLimitFlag == HIGH){
      ResponseStatus = LOW;
      while(Serial.available() > 0) {
        char t = Serial.read();
      }
      ResponseMessage = "";
      TempValue = "";
      TempValue += SH;
      TempValue += EnergyLimit;
      Serial.println(TempValue);
      while(ResponseStatus == LOW && t1 < 800){
        if(Serial.available()){
          ResponseStatus = HIGH;
          ResponseMessage = Serial.readString();
        }
        t1 = t1 + 1;
        delayMicroseconds(500);
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
          delay(20);
        }
      }
      if(t1 > 799){
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
        delay(20);
      }
  }    
}

void SaveLastCurrents(){
  LastCurrent1 = average1;
  LastCurrent2 = average2;
  LastCurrent3 = average3;
}

void CheckPhaseChange(){
  if(PhaseInfo == LOW){
//    Serial.println("PhaseCheck");
    Current1Change = average1 - LastCurrent1 + 2;
    Current2Change = average2 - LastCurrent2 + 2;
    Current3Change = average3 - LastCurrent3 + 2;
//    Serial.print("changes :  ");
//    Serial.print(Current1Change);
//    Serial.print("  ;  ");
//    Serial.print(Current2Change);
//    Serial.print("  ;  ");
//    Serial.println(Current3Change);
//    Serial.print("charge current :  ");
//    Serial.println(charge_current);
    if(charge_current > 6){
      if(Current1Change >= charge_current){
        active_phases = 1;
        if(Current2Change >= charge_current){
          active_phases = 12;
          if(Current3Change >= charge_current){
            active_phases = 123;
          }
        }else if(Current3Change >= charge_current){
          active_phases = 13;
        }
      }else if(Current2Change >= charge_current){
        active_phases = 2;
        if(Current3Change >= charge_current){
          active_phases = 23;
        }
      }else if(Current3Change >= charge_current){
        active_phases = 3;
      }
    }else if(charge_current < 0.5){
      if(Current2Change >= 5){
        active_phases = 20;
        if(Current3Change >= 5){
          active_phases = 23;
        }
      }else if(Current3Change >= 5){
        active_phases = 30;
      }
    }else{
      active_phases = 99;
    }
    if(active_phases != 0){
      PhaseInfo = HIGH;
    }
  }
}


void StopCharge(){
  if(ChargeSetState == HIGH){
    if(charge_current < 1){
      c2 = c2+1;
      if(c2 > 200){
        TurnSleep();
      }
    }else{
      c2 = 0;
    }
    if(State == 0 || State == 2){
      TurnSleep();
    }
    if(charge_current > max_current){
      c3 = c3+1;
      if(c3 > 200){
        TurnSleep();
      }      
    }else{
      c3 = 0;
    }
    if(max_current < 6){
      c4 = c4+1;
      if(c4 > 20){
        TurnSleep();
      }
    }else{
      c4 = 0;
    }
  }
}
    


void Task1code( void * pvParameters ){
  pinMode(ct_sensor_1, INPUT_PULLUP);
  pinMode(ct_sensor_2, INPUT_PULLUP);
  pinMode(ct_sensor_3, INPUT_PULLUP);
  emon1.current(ct_sensor_1, 27.7); // Current: input pin, calibration 102.1 = Arduino pro mini
  delay(50);
  emon2.current(ct_sensor_2, 27.7);
  delay(50);
  emon3.current(ct_sensor_3, 27.7);

  for(;;){
    Irms_1 = emon1.calcIrms(1480);  // Calculate Irms only  1480
    delay(3);
    Irms_2 = emon2.calcIrms(1480);
    delay(3);
    Irms_3 = emon3.calcIrms(1480);
    delay(3);


    


//    long now = millis();
//    if (now - lastInfo > 50) {
//      lastInfo = now;

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
          
//      Serial.println("AVERAGE:");
//      Serial.print(average1);
//      Serial.write("  ");
//      Serial.print(average2);
//      Serial.write("  ");
//      Serial.println(average3);
  
//      Serial.println("IRMS:");
//      Serial.print(Irms_1);
//      Serial.write("  ");
//      Serial.print(Irms_2);
//      Serial.write("  ");
//      Serial.println(Irms_3);
//    }
  } 
}
