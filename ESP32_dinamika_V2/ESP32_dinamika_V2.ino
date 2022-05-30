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
esp32FOTA esp32FOTA("Dinamics", 2);

int FW_version = 2;


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

const char * topica;
const char * charBreaker;
const char * charUpdate;
const char * charStatus;
String sub_BreakerTopic;
String sub_StatusTopic;
String sub_UpdateTopic;


boolean SS1;
boolean SS2;
boolean SS3;
boolean SS4;

boolean UpdateStart;

uint16_t breaker;
String temp_message;
String TempValue;
const char * TempValueChar;

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

  analogSetAttenuation(ADC_6db);
  pinMode(S1, INPUT_PULLDOWN);
  pinMode(S2, INPUT_PULLDOWN);
  pinMode(S3, INPUT_PULLDOWN);
  pinMode(S4, INPUT_PULLDOWN);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);


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


  
  Serial.begin(9600);
  setup_wifi();
  delay(2000);
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
  
  DIPswitchCalc();
  SENDBreaker();
  SENDFWversion();

  Serial.println(FW_version);
}

void loop() {
  // put your main code here, to run repeatedly:
  boolean updatedNeeded = esp32FOTA.execHTTPcheck();
  if(updatedNeeded == HIGH)
  {
    Serial.print("najden update");
    if(UpdateStart == HIGH){
      esp32FOTA.execOTA();
    }
  }
  
  if (!client.connected()) {
    if (!WiFi.status() == WL_CONNECTED){
      setup_wifi();
    }
    reconnect();
  }else{
    SENDCurrents();
  }
  
 client.loop();
 delay(3000);
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
  topica = "";
  dynamicTopic = "";
  epochtimeTopic = getTime();
  dynamicTopic += prefix;
  dynamicTopic += idTopic;
  dynamicTopic += "/";
  dynamicTopic += epochtimeTopic;
  fullTopic = dynamicTopic;
  fullTopic += BreakerTopic;
  topica = fullTopic.c_str();
  TempValue = "";
  TempValue += breaker;
  TempValueChar = TempValue.c_str();
  client.publish(topica, TempValueChar);
  delay(100);
}

void SENDFWversion(){
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
  delay(100);
}

void SENDCurrents(){
  topica = "";
  dynamicTopic = "";
  epochtimeTopic = getTime();
  dynamicTopic += prefix;
  dynamicTopic += idTopic;
  dynamicTopic += "/";
  dynamicTopic += epochtimeTopic;
  fullTopic = dynamicTopic;
  fullTopic += I1Topic;
  topica = fullTopic.c_str();
  TempValue = "";
  TempValue += average1;
  TempValueChar = TempValue.c_str();
  client.publish(topica, TempValueChar);
  delay(100);

  dynamicTopic = prefix;
  dynamicTopic += idTopic;
  dynamicTopic += "/";
  dynamicTopic += epochtimeTopic;
  fullTopic = dynamicTopic;
  fullTopic += I2Topic;
  topica = fullTopic.c_str();
  TempValue = "";
  TempValue += average2;
  TempValueChar = TempValue.c_str();
  client.publish(topica, TempValueChar);
  delay(100);

  dynamicTopic = prefix;
  dynamicTopic += idTopic;
  dynamicTopic += "/";
  dynamicTopic += epochtimeTopic;
  fullTopic = dynamicTopic;
  fullTopic += I3Topic;
  topica = fullTopic.c_str();
  TempValue = "";
  TempValue += average3;
  TempValueChar = TempValue.c_str();
  client.publish(topica, TempValueChar);
  delay(100);
}


void Task1code( void * pvParameters ){
  pinMode(ct_sensor_1, INPUT);
  pinMode(ct_sensor_2, INPUT);
  pinMode(ct_sensor_3, INPUT);
  emon1.current(ct_sensor_1, 6.75); // Current: input pin, calibration 102.1 = Arduino pro mini
  emon2.current(ct_sensor_2, 6.75);
  emon3.current(ct_sensor_3, 6.75);

  for(;;){
    Irms_1 = emon1.calcIrms(1480);  // Calculate Irms only  1480
    Irms_2 = emon2.calcIrms(1480);
    Irms_3 = emon3.calcIrms(1480);

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

//    long now = millis();
//    if (now - lastInfo > 500) {
//      lastInfo = now;
//      
//      Serial.println("AVERAGE:");
//      Serial.print(average1);
//      Serial.write("  ");
//      Serial.print(average2);
//      Serial.write("  ");
//      Serial.println(average3);
//  
////      Serial.println("IRMS:");
////      Serial.print(Irms_1);
////      Serial.write("  ");
////      Serial.print(Irms_2);
////      Serial.write("  ");
////      Serial.println(Irms_3);
//    }
    delay(100);
  } 
}
