#include <FS.h>  
#include <DoubleResetDetect.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <ArduinoJson.h>

#include "gcm.h"
#include "mbusparser.h"

#define DRD_TIMEOUT 5.0
#define DRD_ADDRESS 0x00

const size_t headersize = 11;
const size_t footersize = 3;

char mqtt_server[40];
char mqtt_port[6] = "1883";
char mqtt_uid[40];
char mqtt_pwd[40];
char hostname[40] ="KamstrupMQTT";
char conf_key[] =     "5AD84121D9D20B364B7A11F3C1B5827F";
char conf_authkey[] = "AFB3F93E3E7204EDB3C27F96DBD51AE0";

uint8_t encryption_key[16];
uint8_t authentication_key[16];

bool shouldSaveConfig = false;
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

uint8_t input[] = {0x7E,0xA1,0xE9,0x41,0x03,0x13,0xC6,0x37,0xE6,0xE7,0x00,
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
0x36,0x24,0x7E};

uint8_t receiveBuffer[500];
uint8_t decryptedFrameBuffer[500];
VectorView decryptedFrame(decryptedFrameBuffer,0);

DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);
mbedtls_gcm_context m_ctx;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc,configFile);
        if (error){
          Serial.println(F("Failed to read file, using default configuration"));
          Serial.println(error.c_str());
        }
        else
        {
          strcpy(mqtt_server, doc["mqtt_server"]);
          strcpy(mqtt_port, doc["mqtt_port"]);
          strcpy(mqtt_uid, doc["mqtt_uid"]);
          strcpy(mqtt_pwd, doc["mqtt_pwd"]);
          strcpy(hostname, doc["hostname"]);
          strcpy(conf_key, doc["conf_key"]);
          strcpy(conf_authkey, doc["conf_authkey"]);
        }
        configFile.close();
        serializeJson(doc, Serial);
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_userid("userid", "mqtt uuid", mqtt_uid, 40);
  WiFiManagerParameter custom_mqtt_pwd("password", "mqtt pwd", mqtt_pwd, 40);
  WiFiManagerParameter custom_hostname("hostname", "hostname", hostname, 40);
  WiFiManagerParameter custom_enckey("encryption key", "encryption key", conf_key, 16);
  WiFiManagerParameter custom_authkey("authentication key", "authentication key", conf_authkey, 16);

  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_userid);
  wifiManager.addParameter(&custom_mqtt_pwd);
  wifiManager.addParameter(&custom_hostname);
  wifiManager.addParameter(&custom_enckey);
  wifiManager.addParameter(&custom_authkey);
  
  if (drd.detect())
  {
    Serial.println("** Double reset boot **");
    if (!wifiManager.startConfigPortal(hostname)) {
      Serial.println("failed to connect and hit timeout");
    }
  }
  else
  {
    Serial.println("** Normal boot **");
    if (!wifiManager.autoConnect(hostname)) {
      Serial.println("failed to connect and hit timeout");
    }
  }

  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_uid, custom_mqtt_userid.getValue());
  strcpy(mqtt_pwd, custom_mqtt_pwd.getValue());
  strcpy(hostname, custom_hostname.getValue());
  strcpy(conf_key, custom_enckey.getValue());
  strcpy(conf_authkey, custom_authkey.getValue());

  if (shouldSaveConfig) {
    Serial.println("saving config");
    StaticJsonDocument<512> json;
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["mqtt_uid"] = mqtt_uid;
    json["mqtt_pwd"] = mqtt_pwd;
    json["hostname"] = hostname;
    json["conf_key"] = conf_key;
    json["conf_authkey"] = conf_authkey;
    serializeJson(json, Serial);

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
    serializeJson(json, configFile);
    configFile.close();
    ESP.reset();
  }
  

  f(encryption_key, conf_key, sizeof(encryption_key));
  f(authentication_key, conf_authkey, sizeof(authentication_key));
  Serial.print("Encryptionkey: ");
  printHex(encryption_key,sizeof(encryption_key));
  Serial.println();
  Serial.print("authentication_key: ");
  printHex(authentication_key,sizeof(authentication_key));
  Serial.println();

  
}

void loop() {
  return;
  MbusStreamParser streamParser(receiveBuffer, sizeof(receiveBuffer));
  for(int i=0;i<sizeof(input);i++){
    if (streamParser.pushData(input[i])) {
      VectorView frame = streamParser.getFrame();
      if (streamParser.getContentType() == MbusStreamParser::COMPLETE_FRAME) {
        Serial.println("Frame complete");
        if(!decrypt(frame))
          Serial.println("Decryption failed");
        MeterData md = parseMbusFrame(decryptedFrame);
        if (md.activePowerPlusValid) {
          Serial.print("ActivePower: ");
          Serial.println(md.activePowerPlus);
        }
        if (md.voltageL1Valid) {
          Serial.print("Voltage L1: ");
          Serial.println(md.voltageL1);
        }
        if (md.voltageL2Valid) {
          Serial.print("Voltage L2: ");
          Serial.println(md.voltageL2);
        }
        if (md.voltageL3Valid) {
          Serial.print("Voltage L3: ");
          Serial.println(md.voltageL3);
        }
      } else {
        Serial.print(".");
      }
    }
  }
  delay(10000);

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
    Serial.println("Invalid frame size.");
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
    Serial.print("Setkey failed: ");
    Serial.println(success);
    return false;
  }
  success = mbedtls_gcm_auth_decrypt(&m_ctx, sizeof(cipher_text), initialization_vector, sizeof(initialization_vector), 
    additional_authenticated_data, sizeof(additional_authenticated_data), authentication_tag, sizeof(authentication_tag), 
    cipher_text, plaintext);
  if (0 != success) {
    Serial.print("authdecrypt failed: ");
    Serial.println(success);
    return false;
  }
  mbedtls_gcm_free(&m_ctx);
    
  //copy replace encrypted data with decrypted for mbusparser library. Checksum not updated. Hopefully not needed
  memcpy(decryptedFrameBuffer+headersize+18,plaintext,sizeof(plaintext));
  decryptedFrame = VectorView(decryptedFrameBuffer,frame.size());
  
  return true;
}

void f(uint8_t *dest, const char *source, int bytes_n)
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
