/*#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <mbusparser.h>

static const int LED_PIN = 2;
static const char* ssid = "YourWifi";
static const char* password = "YourPassword";
static const char* mqtt_server = "192.168.10.58";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[60];
char debugbuf[1000];

// Power meter serial com
uint8_t receiveBuffer[1000];

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial1.println();
  Serial1.print("Connecting to ");
  Serial1.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }

  randomSeed(micros());

  Serial1.println("");
  Serial1.println("WiFi connected");
  Serial1.println("IP address: ");
  Serial1.println(WiFi.localIP());
}

void blink(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(50);
    digitalWrite(LED_PIN, HIGH);
    delay(50);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial1.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Kamstrup-ESP8266-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), "mqttclient", "YourPassword")) {
      Serial1.println("connected");
      // Once connected, publish an announcement...
      client.publish("house/electricity/total/status", "Initializing");
    } else {
      Serial1.print("failed, rc=");
      Serial1.print(client.state());
      Serial1.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void writeFrameAsHex(char* buf, size_t bufsize, const VectorView& frame)
{
  size_t position = 0;
  for (size_t i = 0; i < frame.size(); ++i) {
    position += snprintf(buf+position, bufsize-position, "%02X", frame[i]);
  }
}

void writeDebugStringToBuf(char* buf, size_t bufsize, const MeterData& md, const VectorView& frame)
{
  size_t position = 0;
  position += snprintf(buf+position, bufsize-position, "SZ=%d ", frame.size());
  position += snprintf(buf+position, bufsize-position, "MS=%d ", md.parseResultMessageSize);
  position += snprintf(buf+position, bufsize-position, "P+=%d ", md.activePowerPlusValid);
  position += snprintf(buf+position, bufsize-position, "P-=%d ", md.activePowerMinusValid);
  position += snprintf(buf+position, bufsize-position, "R+=%d ", md.reactivePowerMinusValid);
  position += snprintf(buf+position, bufsize-position, "R-=%d ", md.reactivePowerPlusValid);
  position += snprintf(buf+position, bufsize-position, "V1=%d ", md.voltageL1Valid);
  position += snprintf(buf+position, bufsize-position, "V2=%d ", md.voltageL2Valid);
  position += snprintf(buf+position, bufsize-position, "V3=%d ", md.voltageL3Valid);
  position += snprintf(buf+position, bufsize-position, "A1=%d ", md.centiAmpereL1Valid);
  position += snprintf(buf+position, bufsize-position, "A2=%d ", md.centiAmpereL2Valid);
  position += snprintf(buf+position, bufsize-position, "A3=%d ", md.centiAmpereL3Valid);
  position += snprintf(buf+position, bufsize-position, "AI=%d ", md.activeImportWhValid);
  position += snprintf(buf+position, bufsize-position, "AO=%d ", md.activeExportWhValid);
  position += snprintf(buf+position, bufsize-position, "RI=%d ", md.reactiveImportWhValid);
  position += snprintf(buf+position, bufsize-position, "RO=%d", md.reactiveExportWhValid);
  position += snprintf(buf+position, bufsize-position, " ");
  writeFrameAsHex(buf+position, bufsize-position, frame);
}

void parseData(const VectorView& frame) {
  // Parse serial data here, and send to MQTT broker
  MeterData md = parseMbusFrame(frame);

  if (md.activePowerPlusValid) {
    snprintf(msg, sizeof(msg), "%ld", md.activePowerPlus);
    client.publish("house/electricity/power/activePowerPlus", msg);
  }
  if (md.activePowerMinusValid) {
    snprintf(msg, sizeof(msg), "%ld", md.activePowerMinus);
    client.publish("house/electricity/power/activePowerMinus", msg);
  }
  if (md.reactivePowerPlusValid) {
    snprintf(msg, sizeof(msg), "%ld", md.reactivePowerPlus);
    client.publish("house/electricity/power/reactivePowerPlus", msg);
  }
  if (md.reactivePowerMinusValid) {
    snprintf(msg, sizeof(msg), "%ld", md.reactivePowerMinus);
    client.publish("house/electricity/power/reactivePowerMinus", msg);
  }

  if (md.voltageL1Valid) {
    snprintf(msg, sizeof(msg), "%ld", md.voltageL1);
    client.publish("house/electricity/voltage/l1", msg);
  }
  if (md.voltageL2Valid) {
    snprintf(msg, sizeof(msg), "%ld", md.voltageL2);
    client.publish("house/electricity/voltage/l2", msg);
  }
  if (md.voltageL3Valid) {
    snprintf(msg, sizeof(msg), "%ld", md.voltageL3);
    client.publish("house/electricity/voltage/l3", msg);
  }

  if (md.centiAmpereL1Valid) {
    int position = 0;
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.centiAmpereL1 / 100);
    position += snprintf(msg+position, sizeof(msg)-position, ".");
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.centiAmpereL1 % 100);
    client.publish("house/electricity/current/l1", msg);
  }
  if (md.centiAmpereL2Valid) {
    int position = 0;
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.centiAmpereL2 / 100);
    position += snprintf(msg+position, sizeof(msg)-position, ".");
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.centiAmpereL2 % 100);
    client.publish("house/electricity/current/l2", msg);
  }
  if (md.centiAmpereL3Valid) {
    int position = 0;
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.centiAmpereL3 / 100);
    position += snprintf(msg+position, sizeof(msg)-position, ".");
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.centiAmpereL3 % 100);
    client.publish("house/electricity/current/l3", msg);
  }

  if (md.activeImportWhValid) {
    int position = 0;
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.activeImportWh / 1000);
    position += snprintf(msg+position, sizeof(msg)-position, ".");
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.activeImportWh % 1000);
    client.publish("house/electricity/energy/activeImport", msg);
  }
  if (md.activeExportWhValid) {
    int position = 0;
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.activeExportWh / 1000);
    position += snprintf(msg+position, sizeof(msg)-position, ".");
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.activeExportWh % 1000);
    client.publish("house/electricity/energy/activeExport", msg);
  }
  if (md.reactiveImportWhValid) {
    int position = 0;
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.reactiveImportWh / 1000);
    position += snprintf(msg+position, sizeof(msg)-position, ".");
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.reactiveImportWh % 1000);
    client.publish("house/electricity/energy/reactiveImport", msg);
  }
  if (md.reactiveExportWhValid) {
    int position = 0;
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.reactiveExportWh / 1000);
    position += snprintf(msg+position, sizeof(msg)-position, ".");
    position += snprintf(msg+position, sizeof(msg)-position, "%u", md.reactiveExportWh % 1000);
    client.publish("house/electricity/energy/reactiveExport", msg);
  }

  if ((md.listId == 1 && md.parseResultMessageSize != 226) ||
      (md.listId == 2 && md.parseResultMessageSize != 300) ||
      !md.activePowerPlusValid ||
      !md.activePowerMinusValid ||
      !md.reactivePowerPlusValid ||
      !md.reactivePowerMinusValid ||
      !md.voltageL1Valid ||
      !md.voltageL2Valid ||
      !md.voltageL3Valid ||
      !md.centiAmpereL1Valid ||
      !md.centiAmpereL2Valid ||
      !md.centiAmpereL3Valid ||
      (md.listId == 2 &&
       (!md.activeImportWhValid ||
        !md.activeExportWhValid ||
        !md.reactiveImportWhValid ||
        !md.reactiveExportWhValid)))
  {
    writeDebugStringToBuf(debugbuf, sizeof(debugbuf), md, frame);
    client.publish("house/electricity/status", debugbuf);
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);     // Initialize the LED_PIN pin as an output
  Serial.begin(2400, SERIAL_8N1);
  Serial.swap();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  client.publish("house/electricity/status", "Initializing");  
}

int bytesReceived = 0;
int loopCounter = 0;
MbusStreamParser streamParser(receiveBuffer, sizeof(receiveBuffer));

// the loop function runs over and over again forever
void loop() {
  blink(10);
  for (int i = 0; i < 10; ++i) {
    delay(1000);
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    while (Serial.available() > 0) {
      bytesReceived++;
      if (streamParser.pushData(Serial.read())) {
        VectorView frame = streamParser.getFrame();
        if (streamParser.getContentType() == MbusStreamParser::COMPLETE_FRAME) {
          parseData(frame);
        } else {
          writeDebugStringToBuf(debugbuf, sizeof(debugbuf), MeterData(), frame);
          client.publish("house/electricity/status", debugbuf);
        }
      }
    }
  }
  ++loopCounter;

  // Debug:
  snprintf(msg, sizeof(msg), "Looping. Loops=%ld", loopCounter);
  client.publish("house/electricity/status", msg);
  snprintf(msg, sizeof(msg), "Received bytes: %ld", bytesReceived);
  client.publish("house/electricity/status", msg);
}*/
