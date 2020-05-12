#include <PersWiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <EasySSDP.h>
#include <ESP8266mDNS.h>
#include <SPIFFSReadServer.h>
// upload data folder to chip with Arduino ESP8266 filesystem uploader
// https://github.com/esp8266/arduino-esp8266fs-plugin
#include <DNSServer.h>
#include <FS.h>  
#include <DoubleResetDetect.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "gcm.h"
#include "mbusparser.h"

#define DEBUG_BEGIN Serial1.begin(115200);
#define DEBUG_PRINT(x) Serial1.print(x);Log.push_back(x);sendmsg(String(mqtt_topic)+"/status",x);
#define DEBUG_PRINTLN(x) Serial1.println(x);Log.push_back(x);sendmsg(String(mqtt_topic)+"/status",x);

//#define DEBUG_BEGIN Serial.begin(115200);
//#define DEBUG_PRINT(x) Serial.print(x);Log.push_back(x);sendmsg(String(mqtt_topic)+"/status",x);
//#define DEBUG_PRINTLN(x) Serial.println(x);Log.push_back(x);sendmsg(String(mqtt_topic)+"/status",x);


#define DRD_TIMEOUT 1.0
#define DRD_ADDRESS 0x00

const size_t headersize = 11;
const size_t footersize = 3;

char mqtt_server[40];
char mqtt_port[6] = "1883";
char mqtt_uid[40];
char mqtt_pwd[40];
char mqtt_topic[40]="kamstrup";
char hostname[40] ="KamstrupMQTT";
char conf_key[32];// =     "5AD84121D9D20B364B7A11F3C1B5827F";
char conf_authkey[32];// = "AFB3F93E3E7204EDB3C27F96DBD51AE0";

uint8_t encryption_key[16];
uint8_t authentication_key[16];

/*uint8_t input[] = {0x7E,0xA1,0xE9,0x41,0x03,0x13,0xC6,0x37,0xE6,0xE7,0x00,
0xDB,0x08,0x4B,0x41,0x4D,0x45,0x01,0xA4,0xDC,0x52,0x82,0x01,0xD0,0x30,0x00,0x07,0x88,0xE1,0xA0,0x39,0xB2,0xD1,0x4C,0x71,
0x2D,0xD4,0xD8,0xC8,0x44,0x0D,0x53,0x68,0xE4,0x33,0xBD,0x70,0xB7,0x36,0x81,0xE9,0xA9,0xEF,0xFE,0x38,0xF1,0x75,0xA3,0x7D,0xE9,0xCD,0xE6,0x4E,0x8F,
0x78,0x0D,0x8F,0x18,0xB4,0x3F,0xC0,0x59,0xD8,0x79,0x02,0xF3,0xD7,0x47,0xB8,0x14,0xBC,0xD0,0x6A,0x47,0x00,0x68,0x78,0x01,0xBD,0x5D,0x06,0x61,0x20,
0x54,0x50,0x7D,0x44,0xE7,0x66,0x98,0xCC,0x3E,0x35,0xCC,0x9D,0xE6,0x2C,0x28,0x4C,0x0D,0xEE,0xA6,0x35,0xB9,0xBF,0xC5,0x6C,0xE1,0xFE,0x5A,0x3A,0x1E,
0x5E,0x27,0x0B,0x0C,0x18,0x1A,0xCF,0x02,0x15,0x1F,0xCC,0x59,0x21,0x34,0xD0,0x4F,0x02,0x92,0xB5,0xA3,0x53,0x38,0xD7,0xB7,0x81,0xBB,0x1F,0x2A,0x7E,
0x40,0x71,0x81,0x5C,0xEE,0xD5,0xD4,0xBA,0xC6,0xEF,0xAA,0xAF,0xF9,0x79,0xEF,0x96,0x9D,0x0D,0xB4,0x6F,0x51,0xE5,0xE0,0xFC,0x00,0xF5,0xAD,0x10,0x5F,
0xBE,0xF9,0x5F,0xC5,0xF8,0x85,0x46,0x0B,0x56,0x32,0x55,0x4A,0xC0,0x5D,0x9E,0xB4,0xF6,0x5F,0xF2,0x23,0x97,0x2A,0x47,0xCF,0xD4,0x34,0xB5,0xF5,0xE2,
0xD8,0x53,0xEA,0x4C,0x14,0x72,0x75,0x86,0xF0,0xE2,0x1C,0x6E,0xE1,0x25,0x26,0x8C,0xB4,0xDC,0x7E,0xC5,0xB1,0x0F,0x84,0x83,0xC0,0x10,0xC8,0xE6,0x88,
0xDF,0x86,0x58,0x4D,0x7C,0x29,0xD8,0x17,0x31,0xA4,0xE0,0x96,0x91,0x41,0xB6,0xAD,0xD9,0x42,0xE4,0x0A,0x96,0xE3,0xE2,0xDC,0x2F,0x90,0x20,0xBF,0x9D,
0x58,0x02,0xA1,0x8D,0xC9,0x85,0xBB,0x54,0x22,0xF4,0x70,0xC0,0x62,0x9D,0x22,0xDB,0x6F,0x16,0xB6,0x64,0x7D,0xB3,0xC9,0xF7,0x27,0xC1,0x70,0xC2,0xDB,
0x4C,0x9A,0x23,0x0F,0x82,0x83,0x46,0xE8,0x6F,0x56,0xD3,0x47,0xB6,0x2B,0xFE,0x28,0xA5,0x07,0xAE,0x2A,0x21,0x9A,0xCC,0x63,0xAD,0x5E,0xE0,0x6E,0xCB,
0x94,0x9D,0xC4,0xFD,0xC2,0xD2,0xF8,0x08,0x02,0x7A,0x4A,0x4C,0x67,0x7C,0x93,0xC4,0xC8,0x90,0x9D,0x73,0x36,0xD8,0xB8,0xB3,0x79,0x18,0x36,0xC5,0x55,
0x5E,0x74,0xE2,0x54,0x82,0xA4,0x4A,0x5F,0x6D,0x35,0x8D,0xCE,0x84,0x50,0x47,0x8F,0x8B,0x2C,0x5D,0x56,0x66,0xC2,0xCF,0x8B,0xD1,0x90,0xD4,0x87,0x4E,
0x70,0x7F,0x5D,0x4B,0xCD,0xE9,0x4E,0x91,0x35,0x5A,0x81,0x39,0xC3,0xE4,0x1C,0xD5,0xF0,0x88,0x99,0xB6,0x5E,0xE9,0xB5,0x9F,0xC6,0x03,0x72,0xE4,0xF9,
0x2B,0xC9,0x98,0x26,0xB8,0xC1,0x47,0xF2,0x09,0x5F,0xB3,0x8A,0x89,0x14,0x09,0xAA,0x81,0xE2,0x27,0x07,0x6C,0x21,0xCB,0x7C,0xAF,0x73,0xB3,0xE8,0xA4,
0xE2,0x56,0xC7,0x0D,0x95,0x47,0xA9,0x14,0xF3,0x9C,0x16,0x93,0x76,0xBF,0x92,0x2B,0x08,0x06,0x4E,0xC4,0xFB,0x31,0xD4,0x4E,0xCD,0x72,0x1D,0x1A,0x15,
0x1E,0x4E,0x68,0x4F,0x0B,0x26,0x85,0xC4,0xB6,0x9D,0x96,0xF5,0xFB,0x52,0xD0,0xB8,0x12,0x79,
0x36,0x24,0x7E};*/

uint8_t receiveBuffer[500];
uint8_t decryptedFrameBuffer[500];
VectorView decryptedFrame(decryptedFrameBuffer,0);

DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);
mbedtls_gcm_context m_ctx;
#define LOGSIZE 50
std::vector<String> Log;

SPIFFSReadServer server(80);
DNSServer dnsServer;
PersWiFiManager persWM(server, dnsServer);

WiFiClient espClient;
PubSubClient psclient(espClient);

void setup() {
  Serial.begin(2400, SERIAL_8N1);
  Serial.swap();
  DEBUG_BEGIN
  DEBUG_PRINTLN("")
  if (SPIFFS.begin()) 
    DEBUG_PRINTLN("Mounted file system");
  loadConfig();

  /*if (drd.detect())
  {
    DEBUG_PRINTLN("** Double reset boot. Ressetting WiFi settings **");
    persWM.resetSettings();
  }*/
  
  WiFi.hostname(hostname);
  persWM.onConnect(&on_connect);
  persWM.onAp([](){
    DEBUG_PRINTLN("AP MODE: " + persWM.getApSsid());
  });
  persWM.setApCredentials(hostname);
  persWM.setConnectNonBlock(false);
  persWM.begin();
  
  server.on("/log", []() {
    String logs = "Log contains:\n";
    for (int i = 0; i<Log.size();i++)
      logs += Log.at(i) + '\n';
    server.send(200, "text/plain", logs.c_str());
   });

  server.on("/config", []() {
    DEBUG_PRINTLN("server.on /config");
    if (server.hasArg("mqtt_server")) {
      strlcpy(mqtt_server,server.arg("mqtt_server").c_str(),sizeof(mqtt_server)-1);
      DEBUG_PRINTLN("mqtt_server: " + server.arg("mqtt_server"));
    } 
    if (server.hasArg("mqtt_port")) {
      strlcpy(mqtt_port,server.arg("mqtt_port").c_str(),sizeof(mqtt_port)-1);
      DEBUG_PRINTLN("mqtt_port: " + server.arg("mqtt_port"));
    }
    if (server.hasArg("mqtt_uid")) {
      strlcpy(mqtt_uid,server.arg("mqtt_uid").c_str(),sizeof(mqtt_uid)-1);
      DEBUG_PRINTLN("mqtt_uid: " + server.arg("mqtt_uid"));
    } 
    if (server.hasArg("mqtt_pwd")) {
      strlcpy(mqtt_pwd,server.arg("mqtt_pwd").c_str(),sizeof(mqtt_pwd)-1);
      DEBUG_PRINTLN("mqtt_pwd: " + server.arg("mqtt_pwd"));
    } 
    if (server.hasArg("mqtt_topic")) {
      strlcpy(mqtt_topic,server.arg("mqtt_topic").c_str(),sizeof(mqtt_topic)-1);
      DEBUG_PRINTLN("mqtt_topic: " + server.arg("mqtt_topic"));
    } 
    if (server.hasArg("hostname")) {
      strlcpy(hostname,server.arg("hostname").c_str(),sizeof(hostname)-1);
      DEBUG_PRINTLN("hostname: " + server.arg("hostname"));
    }
    if (server.hasArg("conf_key")) {
      strlcpy(conf_key,server.arg("conf_key").c_str(),sizeof(conf_key));
      DEBUG_PRINTLN("conf_key: " + server.arg("conf_key"));
    }
    if (server.hasArg("conf_authkey")) {
      strlcpy(conf_authkey,server.arg("conf_authkey").c_str(),sizeof(conf_authkey));
      DEBUG_PRINTLN("conf_authkey: " + server.arg("conf_authkey"));
    }

    if(server.hasArg("mqtt_server") && 
      server.hasArg("mqtt_port") && 
      server.hasArg("mqtt_uid") && 
      server.hasArg("mqtt_pwd") && 
      server.hasArg("mqtt_topic") && 
      server.hasArg("hostname") && 
      server.hasArg("conf_key") && 
      server.hasArg("conf_authkey")){
       saveConfig();
    }

  StaticJsonDocument<512> json;
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_uid"] = mqtt_uid;
  json["mqtt_pwd"] = mqtt_pwd;
  json["mqtt_topic"] = mqtt_topic;
  json["hostname"] = hostname;
  json["conf_key"] = conf_key;
  json["conf_authkey"] = conf_authkey;
  String tmp;
  serializeJson(json, tmp);
    server.send(200, "application/json", tmp);
  });
  server.begin();
  DEBUG_PRINTLN("Setup complete.");
}

MbusStreamParser streamParser(receiveBuffer, sizeof(receiveBuffer));

void loop() {
  if(Log.size()>LOGSIZE){
    std::rotate(Log.begin(),Log.end()-LOGSIZE,Log.end());
    Log.resize(LOGSIZE);
  }
  persWM.handleWiFi();
  dnsServer.processNextRequest();
  server.handleClient();
  
  if (!psclient.connected()) {
    mqttReconnect();
    for(auto msg : Log)
      sendmsg(String(mqtt_topic)+"/status",msg);
  }
  psclient.loop();

  /*WiFi.disconnect();
  WiFi.forceSleepBegin();
  delay(1); //For some reason the modem won't go to sleep unless you do a delay

  WiFi.forceSleepWake();
  delay(1);*/
  
  while (Serial.available() > 0) {
  //for(int i=0;i<sizeof(input);i++){
    if (streamParser.pushData(Serial.read())) {
  //  if (streamParser.pushData(input[i])) {
      VectorView frame = streamParser.getFrame();
      if (streamParser.getContentType() == MbusStreamParser::COMPLETE_FRAME) {
        DEBUG_PRINTLN("Frame complete");
        if(!decrypt(frame))
        {
          DEBUG_PRINTLN("Decryption failed");
          return;
        }
        MeterData md = parseMbusFrame(decryptedFrame);
        sendData(md);
      }
    }
  }
//  delay(1000);
}

void sendData(MeterData md){
   if (md.activePowerPlusValid) 
    sendmsg(String(mqtt_topic)+"/power/activePowerPlus",String(md.activePowerPlus));
   if (md.activePowerMinusValid) 
    sendmsg(String(mqtt_topic)+"/power/activePowerMinus",String(md.activePowerMinus));
   if (md.activePowerPlusValidL1) 
    sendmsg(String(mqtt_topic)+"/power/activePowerPlusL1",String(md.activePowerPlusL1));
   if (md.activePowerMinusValidL1) 
    sendmsg(String(mqtt_topic)+"/power/activePowerMinusL1",String(md.activePowerMinusL1));
   if (md.activePowerPlusValidL2) 
    sendmsg(String(mqtt_topic)+"/power/activePowerPlusL2",String(md.activePowerPlusL2));
   if (md.activePowerMinusValidL2) 
    sendmsg(String(mqtt_topic)+"/power/activePowerMinusL2",String(md.activePowerMinusL2));
   if (md.activePowerPlusValidL3) 
    sendmsg(String(mqtt_topic)+"/power/activePowerPlusL3",String(md.activePowerPlusL3));
   if (md.activePowerMinusValidL3) 
    sendmsg(String(mqtt_topic)+"/power/activePowerMinusL3",String(md.activePowerMinusL3));
   if (md.reactivePowerPlusValid) 
    sendmsg(String(mqtt_topic)+"/power/reactivePowerPlus",String(md.reactivePowerPlus));
   if (md.reactivePowerMinusValid) 
    sendmsg(String(mqtt_topic)+"/power/reactivePowerMinus",String(md.reactivePowerMinus));

   if (md.powerFactorValidL1) 
    sendmsg(String(mqtt_topic)+"/power/powerFactorL1",String(md.powerFactorL1));
   if (md.powerFactorValidL2) 
    sendmsg(String(mqtt_topic)+"/power/powerFactorL2",String(md.powerFactorL2));
   if (md.powerFactorValidL3) 
    sendmsg(String(mqtt_topic)+"/power/powerFactorL3",String(md.powerFactorL3));
   if (md.powerFactorTotalValid) 
    sendmsg(String(mqtt_topic)+"/power/powerFactorTotal",String(md.powerFactorTotal));

   if (md.voltageL1Valid) 
    sendmsg(String(mqtt_topic)+"/voltage/L1",String(md.voltageL1));
   if (md.voltageL2Valid) 
    sendmsg(String(mqtt_topic)+"/voltage/L2",String(md.voltageL2));
   if (md.voltageL3Valid) 
    sendmsg(String(mqtt_topic)+"/voltage/L3",String(md.voltageL3));

   if (md.centiAmpereL1Valid) 
    sendmsg(String(mqtt_topic)+"/current/L1",String(md.centiAmpereL1/100.));
   if (md.centiAmpereL2Valid) 
    sendmsg(String(mqtt_topic)+"/current/L2",String(md.centiAmpereL2/100.));
   if (md.centiAmpereL3Valid) 
    sendmsg(String(mqtt_topic)+"/current/L3",String(md.centiAmpereL3/100.));

   if (md.activeImportWhValid) 
    sendmsg(String(mqtt_topic)+"/energy/activeImportKWh",String(md.activeImportWh/1000.));
   if (md.activeExportWhValid) 
    sendmsg(String(mqtt_topic)+"/energy/activeExportKWh",String(md.activeExportWh/1000.));
   if (md.activeImportWhValidL1) 
    sendmsg(String(mqtt_topic)+"/energy/activeImportKWhL1",String(md.activeImportWhL1/1000.));
   if (md.activeExportWhValidL1) 
    sendmsg(String(mqtt_topic)+"/energy/activeExportKWhL1",String(md.activeExportWhL1/1000.));
   if (md.activeImportWhValidL2) 
    sendmsg(String(mqtt_topic)+"/energy/activeImportKWhL2",String(md.activeImportWhL2/1000.));
   if (md.activeExportWhValidL2) 
    sendmsg(String(mqtt_topic)+"/energy/activeExportKWhL2",String(md.activeExportWhL2/1000.));
   if (md.activeImportWhValidL3) 
    sendmsg(String(mqtt_topic)+"/energy/activeImportKWhL3",String(md.activeImportWhL3/1000.));
   if (md.activeExportWhValidL3) 
    sendmsg(String(mqtt_topic)+"/energy/activeExportKWhL3",String(md.activeExportWhL3/1000.));

   if (md.reactiveImportWhValid) 
    sendmsg(String(mqtt_topic)+"/energy/reactiveImportKWh",String(md.reactiveImportWh/1000.));
   if (md.reactiveExportWhValid) 
    sendmsg(String(mqtt_topic)+"/energy/reactiveExportKWh",String(md.reactiveExportWh/1000.));
}

void on_connect() {
  DEBUG_PRINTLN("Wifi connected");
  DEBUG_PRINTLN("Connect to http://"+String(hostname)+".local or http://" + WiFi.localIP().toString());
  EasySSDP::begin(server);
  if (MDNS.begin(hostname)) {
    DEBUG_PRINTLN("MDNS responder started");
  } else {
    DEBUG_PRINTLN("Error setting up MDNS responder!");
  }
  DEBUG_PRINTLN("Connecting to MQTT");
   psclient.setServer(mqtt_server, atoi(mqtt_port));
}

void loadConfig(){
  DEBUG_PRINTLN("Load config");
  if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      DEBUG_PRINTLN("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        DEBUG_PRINTLN("opened config file");
        size_t size = configFile.size();
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc,configFile);
        if (error){
          DEBUG_PRINTLN("Failed to read file. Using default configuration. - "+  String(error.c_str()));
        }
        else
        {
          strcpy(mqtt_server, doc["mqtt_server"]);
          strcpy(mqtt_port, doc["mqtt_port"]);
          strcpy(mqtt_uid, doc["mqtt_uid"]);
          strcpy(mqtt_pwd, doc["mqtt_pwd"]);
          strcpy(mqtt_topic, doc["mqtt_topic"]);
          strcpy(hostname, doc["hostname"]);
          strcpy(conf_key, doc["conf_key"]);
          strcpy(conf_authkey, doc["conf_authkey"]);
        }
        configFile.close();
        serializeJson(doc, Serial);
      }
  }
  else{
    DEBUG_PRINTLN("Config file not found. Using default configuration");
  }
  hexStr2bArr(encryption_key, conf_key, sizeof(encryption_key));
  hexStr2bArr(authentication_key, conf_authkey, sizeof(authentication_key));
  DEBUG_PRINTLN("MQTT server: " + String(mqtt_server));
  DEBUG_PRINTLN("MQTT port: " + String(mqtt_port));
  DEBUG_PRINTLN("MQTT user: " + String(mqtt_uid));
  DEBUG_PRINTLN("MQTT password: " + String(mqtt_pwd));
  DEBUG_PRINTLN("MQTT topic: " + String(mqtt_topic));
  DEBUG_PRINTLN("Hostname: " + String(hostname));
  DEBUG_PRINTLN("Encryption key: " + String(conf_key));
  DEBUG_PRINTLN("Authentication key: " + String(conf_authkey));
}

void saveConfig(){
  DEBUG_PRINTLN("saving config");
  StaticJsonDocument<512> json;
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_uid"] = mqtt_uid;
  json["mqtt_pwd"] = mqtt_pwd;
  json["mqtt_topic"] = mqtt_topic;
  json["hostname"] = hostname;
  json["conf_key"] = conf_key;
  json["conf_authkey"] = conf_authkey;
      
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    DEBUG_PRINTLN("failed to open config file for writing");
  }
  serializeJson(json, configFile);
  configFile.close();
  hexStr2bArr(encryption_key, conf_key, sizeof(encryption_key));
  hexStr2bArr(authentication_key, conf_authkey, sizeof(authentication_key));
  //todo: reload mqtt, mdns and other
}

void mqttReconnect() {
  // Loop until we're reconnected
  if (!psclient.connected()) {
    DEBUG_PRINTLN("Attempting MQTT connection...");
    // Attempt to connect
    if (psclient.connect("arduinoClient")) {
      DEBUG_PRINTLN("connected");
    } else {
      DEBUG_PRINTLN("failed, rc=" + String(psclient.state()));
    }
  }
}

void printHex(const unsigned char* data,const size_t length) {
  for (int i = 0; i < length; i++) {
    Serial.printf("%02X", data[i]);
  }
}

void printHex(const VectorView& frame) {
  for (int i = 0; i < frame.size(); i++) {
    Serial.printf("%02X", frame[i]);
  }
}

bool decrypt(const VectorView& frame){

  if(frame.size() < headersize+footersize+12+18){
    DEBUG_PRINTLN("Invalid frame size.");
  }
  
  memcpy(decryptedFrameBuffer, &frame.front(), frame.size());
    
  uint8_t system_title[8];
  memcpy(system_title, decryptedFrameBuffer+headersize+2, 8);
  
  uint8_t initialization_vector[12];
  memcpy(initialization_vector,system_title,8);
  memcpy(initialization_vector+8,decryptedFrameBuffer+headersize+14,4);
  
  uint8_t additional_authenticated_data[17];
  memcpy(additional_authenticated_data,decryptedFrameBuffer+headersize+13,1);
  memcpy(additional_authenticated_data+1,authentication_key,16);
  
  uint8_t authentication_tag[12];
  memcpy(authentication_tag,decryptedFrameBuffer+headersize+frame.size()-headersize-footersize-12,12);
 
  uint8_t cipher_text[frame.size()-headersize-footersize-18-12];
  memcpy(cipher_text,decryptedFrameBuffer+headersize+18,frame.size()-headersize-footersize-12-18);
 
  uint8_t plaintext[sizeof(cipher_text)];

  mbedtls_gcm_init(&m_ctx);
  int success = mbedtls_gcm_setkey(&m_ctx, MBEDTLS_CIPHER_ID_AES, encryption_key, sizeof(encryption_key)*8);
  if (0 != success ) {
    DEBUG_PRINTLN("Setkey failed: " + String(success));
    return false;
  }
  success = mbedtls_gcm_auth_decrypt(&m_ctx, sizeof(cipher_text), initialization_vector, sizeof(initialization_vector), 
    additional_authenticated_data, sizeof(additional_authenticated_data), authentication_tag, sizeof(authentication_tag), 
    cipher_text, plaintext);
  if (0 != success) {
    DEBUG_PRINTLN("authdecrypt failed: " + String(success));
    return false;
  }
  mbedtls_gcm_free(&m_ctx);
    
  //copy replace encrypted data with decrypted for mbusparser library. Checksum not updated. Hopefully not needed
  memcpy(decryptedFrameBuffer+headersize+18,plaintext,sizeof(plaintext));
  decryptedFrame = VectorView(decryptedFrameBuffer,frame.size());
  
  return true;
}

void hexStr2bArr(uint8_t *dest, const char *source, int bytes_n)
{
    uint8_t *dst = dest;
    uint8_t *end = dest + sizeof(bytes_n);
    unsigned int u;

    while (dest < end && sscanf(source, "%2x", &u) == 1)
    {
        *dst++ = u;
        source += 2;
    }
}

void sendmsg(String topic, String payload) {
  if(psclient.connected()){
    psclient.publish(topic.c_str(),payload.c_str());
  }
}
