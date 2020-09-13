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

#define DRD_TIMEOUT 1.0
#define DRD_ADDRESS 0x00

#define HEARTBEATLED 4
#define FAULTLED 5
#define ON 0
#define OFF 1

const size_t headersize = 11;
const size_t footersize = 3;

char mqtt_server[40];
char mqtt_port[6] = "1883";
char mqtt_uid[40] = "";
char mqtt_pwd[40] = "";
char mqtt_topic[40]="kamstrup";
char hostname[40] ="KamstrupMQTT";
char conf_key[33];
char conf_authkey[33];
bool isAP=true;

uint8_t encryption_key[16];
uint8_t authentication_key[16];

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
  //PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0RXD_U);
  PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTCK_U);
  pinMode(HEARTBEATLED,OUTPUT);
  pinMode(FAULTLED,OUTPUT);
  digitalWrite(HEARTBEATLED,OFF);
  digitalWrite(FAULTLED,ON);
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
    digitalWrite(FAULTLED,ON);
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
  MDNS.update();
  
  if (!psclient.connected() && !isAP && mqtt_server[0]!='\0') {
    mqttReconnect();
    for(auto msg : Log)
      sendmsg(String(mqtt_topic)+"/status",msg);
  }
  psclient.loop();
  
  while (Serial.available() > 0) {
  digitalWrite(HEARTBEATLED,ON);
    if (streamParser.pushData(Serial.read())) {
      VectorView frame = streamParser.getFrame();
      if (streamParser.getContentType() == MbusStreamParser::COMPLETE_FRAME) {
        DEBUG_PRINTLN("Frame complete");
        if(!decrypt(frame))
        {
          DEBUG_PRINTLN("Decryption failed");
          digitalWrite(FAULTLED,ON);
          return;
        }
        MeterData md = parseMbusFrame(decryptedFrame);
        digitalWrite(FAULTLED,OFF);
        sendData(md);
      }
    }
  }
  digitalWrite(HEARTBEATLED,OFF);
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
  digitalWrite(FAULTLED,OFF);
  isAP = false;
  EasySSDP::begin(server);
  if (MDNS.begin(hostname)) {
    DEBUG_PRINTLN("MDNS responder started");
  } else {
    DEBUG_PRINTLN("Error setting up MDNS responder!");
  }
  MDNS.addService("http", "tcp", 80);
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
  if (!psclient.connected()) {
    DEBUG_PRINTLN("Attempting MQTT connection...");
    // Attempt to connect
    if (psclient.connect(hostname,
     (mqtt_uid[0]=='\0')?NULL:mqtt_uid,
     (mqtt_pwd[0]=='\0')?NULL:mqtt_pwd)) {
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
