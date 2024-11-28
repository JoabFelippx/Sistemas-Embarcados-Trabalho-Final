#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID
#define WIFI_SSID "ssid"
// WiFi password
#define WIFI_PASSWORD "password"
// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "http://<ip>:<port>"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> <select token>)
#define INFLUXDB_TOKEN "<token>"
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "<org>"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "<bucket>"

#define timeSeconds 10
#define LDR_PIN 34  // ESP32's pin GPIO34 connected to DO pin of the ldr module


const int led = 25;
const int motionSensor = 27;

// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
boolean motion = false;
boolean light = false;


// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// DHT sensor
#include "DHTesp.h"

const int DHT_PIN = 4;

DHTesp dhtSensor;

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
    Point sensor("wifi_status");

void IRAM_ATTR detectsMovement() {
  digitalWrite(led, HIGH);
  startTimer = true;
  lastTrigger = millis();
}


void setup() {
    Serial.begin(115200);

    // initialize the ESP32's pin as an input
    pinMode(LDR_PIN, INPUT);

      
    // PIR Motion Sensor mode INPUT_PULLUP
    pinMode(motionSensor, INPUT_PULLUP);
    // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
    attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
    
    // Set LED to LOW
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);

    // Initialize DHT sensor    
    dhtSensor.setup(DHT_PIN, DHTesp::DHT11); // In Wokwi DHT22
    // Setup wifi
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to wifi");
    while (wifiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    pinMode(motionSensor, INPUT_PULLUP);
//    attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);  
  
  // Add tags

    sensor.addTag("Temp", "DHT11");
    sensor.addTag("device", DEVICE);
    sensor.addTag("SSID", WiFi.SSID());
    sensor.addTag("Light", "LDR");

  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
    if (client.validateConnection()) {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());
    } else {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
  }
}


void loop() {
    // Clear fields for reusing the point. Tags will remain untouched
    sensor.clearFields();
     
    // Store measured value into point
    // DHTesp library returns humidity and temperature in a single call
    TempAndHumidity  data = dhtSensor.getTempAndHumidity();

    // Add fields to point
    sensor.addField("temperature", data.temperature);
    sensor.addField("humidity", data.humidity);

    sensor.addField("rssi", WiFi.RSSI());

    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());

    Serial.println(String(data.temperature, 2));
    Serial.println(String(data.humidity, 1));

    now = millis();

    if(motion == false){
      
      sensor.addField("Motion", 0);      
      }
    if(motion == true){

      sensor.addField("Motion", 1);      

      }

    if(light == false){
      
      sensor.addField("LDR", 0);      
      }
    if(light == true){

      sensor.addField("LDR", 1);      

      }

    
    if((digitalRead(led) == HIGH) && (motion == false)) {
      Serial.println("MOTION DETECTED!!!");
      sensor.addField("Motion", 1);
      
      motion = true;
    }
    // Turn off the LED after the number of seconds defined in the timeSeconds variable
    if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
      Serial.println("Motion stopped...");
      digitalWrite(led, LOW);
      sensor.addField("Motion", 0);
      startTimer = false;
      motion = false;
    }
    // Check WiFi connection and reconnect if needed
    if (wifiMulti.run() != WL_CONNECTED) {
        Serial.println("Wifi connection lost");
    }

    // Write point
    if (!client.writePoint(sensor)) {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
    }

    int lightState = digitalRead(LDR_PIN);

    if (lightState == HIGH){
        Serial.println("It is dark");
        sensor.addField("LDR", 0);
        light = false;}
    else {
        Serial.println("It is light");
        sensor.addField("LDR", 1);
        light = true;}

   
    
    Serial.println("Wait 10s");
        
    delay(10000);
}
