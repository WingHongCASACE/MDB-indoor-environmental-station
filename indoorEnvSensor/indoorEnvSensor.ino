/*Many website/ tutorials were consulted for this project, including electronic components and Arduino libraries

Pressure sensor:
https://lastminuteengineers.com/bmp180-arduino-tutorial/
https://create.arduino.cc/projecthub/electropeak/determining-the-pressure-and-altitude-using-gy-68-bmp180-and-93fd6f
Gas sensor:
https://www.programmingboss.com/2021/02/mq-135-gas-sensor-with-arduino-code.html
https://www.olimex.com/Products/Components/Sensors/Gas/SNS-MQ135/resources/SNS-MQ135.pdf
https://www.codrey.com/electronic-circuits/how-to-use-mq-135-gas-sensor/
LCD:
https://electronics-project-hub.com/how-to-connect-multiple-lcd-to-arduino/
https://www.arduino.cc/en/Tutorial/LibraryExamples/HelloWorld
LED ring:
https://create.arduino.cc/projecthub/electropeak/neopixel-how-to-control-ws2812-rgb-led-w-arduino-46c08f
Light sensor:
https://how2electronics.com/temt6000-ambient-light-sensor-arduino-measure-light-intensity/
WiFi:
https://www.arduino.cc/en/Reference/WiFiNINA
MQTT:
https://docs.arduino.cc/tutorials/uno-wifi-rev2/uno-wifi-r2-mqtt-device-to-device

In addition, I haved received many helps from Martin and my classmates, notably from MingH
*/


#include <SPI.h>
#include <ezTime.h>
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#define seaLevelPressure_hPa 1013.25
Adafruit_BMP085 bmp;
#include <LiquidCrystal.h>
float gasValue;
int buzzerPin = 6;
int gasPin = A0;
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 16
#define PIXELPIN 13
Adafruit_NeoPixel pixels(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int temt6000Pin = 2;

#include "arduino_secrets.h"
int status = WL_IDLE_STATUS;     // the WiFi radio's status
int rainvalue;
int Raindrop;
float temperature;

//sensitive data in arduino_secrets.h
const char ssid[]     = SECRET_SSID;
const char password[] = SECRET_PASS;
const char mqttuser[] = SECRET_MQTTUSER;
const char mqttpass[] = SECRET_MQTTPASS;

//mqtt broker and topic hiddened
const char broker[] = "mqtt...org";
int        port     = 1884;
const char topic[]  = ".../temperature_*C";
const char topic1[]  = ".../pressure";
//const char topic2[]  = ".../altitude";
const char topic3[]  = ".../light_lux";


WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

long lastMsg = 0;
char msg[50];
int value = 0;
Timezone GB;

void startWifi(){         //corresponse to 0014 workshop but adjusted for WiFiNINA
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // check to see if connected and wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void syncDate() {             //corresponse to 0014 workshop
  // get real date and time
  waitForSync();
  Serial.println("UTC: " + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time: " + GB.dateTime());
}


void setup() {
 Wire.begin();
 // set up the LCD's number of columns and rows:
 lcd.begin(16, 2);

 bmp.begin(); 
 pinMode(buzzerPin, OUTPUT);
 pinMode(gasPin, INPUT);
 
 pixels.begin();

 Serial.begin(9600);
 delay(100);
 
 startWifi();
 syncDate();


  mqttClient.setUsernamePassword(mqttuser, mqttpass);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("MQTT broker connected");
  Serial.println();
}

void readData(){
   = analogRead(gasPin);       // read analog input pin A5
  float volts = analogRead(A2) * 5.0 / 1024.0;
  float amps = volts / 10000.0; // across 10,000 Ohms
  float microamps = amps * 1000000;
  float lux = microamps * 2.0;
  Serial.print("Light: ");
  Serial.println(lux);
  Serial.print("Temperature = ");
  temperature = bmp.readTemperature();
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");    
  float temperature = bmp.readTemperature();
  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
  float pressure = bmp.readPressure();
  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");
  float altitude = bmp.readAltitude();
  Serial.print("Pressure at sealevel (calculated) = ");
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");
  Serial.print("Real altitude = ");
  Serial.print(bmp.readAltitude(seaLevelPressure_hPa * 100));
  Serial.println(" meters");
  Serial.print("AirQuality Value: ");
  Serial.println(, DEC);
  float airQuality = analogRead(gasPin);

  delay(500);
  // (column, row)=(0+n,0+m)
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Pressure:");
  lcd.print(bmp.readPressure()/100);
  lcd.print("hPa");
  lcd.setCursor(0, 1);
  lcd.print("Light:");
  lcd.print(lux);
  lcd.print(" lx");

  if (gasValue > 180) {                       //buzzer is used as an gas/smoke alarm: it will remain silent and unnoticed if gas level remains below threshold
    tone(buzzerPin,1000);
    delay(2000); // wait 2000ms
  }

  else {
    noTone(buzzerPin);
    delay(500); // wait 500ms
  }

  if (temperature <=10){                      //number of LED ring to be on depends on temperature; color of each LED was fixed by given RGB value
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
  else if (temperature <=12){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else if (temperature <=14){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else if (temperature <=16){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(11,pixels.Color(21,0,106));
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else if (temperature <=18){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(10,pixels.Color(28,0,100));
    pixels.setPixelColor(11,pixels.Color(21,0,106));
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else if (temperature <=20){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(9,pixels.Color(28,0,85));
    pixels.setPixelColor(10,pixels.Color(28,0,100));
    pixels.setPixelColor(11,pixels.Color(21,0,106));
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else if (temperature <=22){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(8, pixels.Color(35, 57, 15));
    pixels.setPixelColor(9,pixels.Color(28,0,85));
    pixels.setPixelColor(10,pixels.Color(28,0,100));
    pixels.setPixelColor(11,pixels.Color(21,0,106));
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else if (temperature <=24){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(7, pixels.Color(35, 57, 15));
    pixels.setPixelColor(8, pixels.Color(35, 57, 15));
    pixels.setPixelColor(9,pixels.Color(28,0,85));
    pixels.setPixelColor(10,pixels.Color(28,0,100));
    pixels.setPixelColor(11,pixels.Color(21,0,106));
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else if (temperature <=26){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(6, pixels.Color(72,0,55));
    pixels.setPixelColor(7, pixels.Color(35, 57, 15));
    pixels.setPixelColor(8, pixels.Color(35, 57, 15));
    pixels.setPixelColor(9,pixels.Color(28,0,85));
    pixels.setPixelColor(10,pixels.Color(28,0,100));
    pixels.setPixelColor(11,pixels.Color(21,0,106));
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else if (temperature <=28){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(5, pixels.Color(80, 1, 47));
    pixels.setPixelColor(6, pixels.Color(72,0,55));
    pixels.setPixelColor(7, pixels.Color(35, 57, 15));
    pixels.setPixelColor(8, pixels.Color(35, 57, 15));
    pixels.setPixelColor(9,pixels.Color(28,0,85));
    pixels.setPixelColor(10,pixels.Color(28,0,100));
    pixels.setPixelColor(11,pixels.Color(21,0,106));
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else if (temperature <=30){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(4, pixels.Color(108, 0, 20));
    pixels.setPixelColor(5, pixels.Color(80, 1, 47));
    pixels.setPixelColor(6, pixels.Color(72,0,55));
    pixels.setPixelColor(7, pixels.Color(35, 57, 15));
    pixels.setPixelColor(8, pixels.Color(35, 57, 15));
    pixels.setPixelColor(9,pixels.Color(28,0,85));
    pixels.setPixelColor(10,pixels.Color(28,0,100));
    pixels.setPixelColor(11,pixels.Color(21,0,106));
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else if (temperature <=32){
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(3, pixels.Color(117, 0, 10));
    pixels.setPixelColor(4, pixels.Color(108, 0, 20));
    pixels.setPixelColor(5, pixels.Color(80, 1, 47));
    pixels.setPixelColor(6, pixels.Color(72,0,55));
    pixels.setPixelColor(7, pixels.Color(35, 57, 15));
    pixels.setPixelColor(8, pixels.Color(35, 57, 15));
    pixels.setPixelColor(9,pixels.Color(28,0,85));
    pixels.setPixelColor(10,pixels.Color(28,0,100));
    pixels.setPixelColor(11,pixels.Color(21,0,106));
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
    else {
    pixels.clear();
    pixels.setBrightness(20);
    pixels.setPixelColor(2, pixels.Color(127, 0, 1));
    pixels.setPixelColor(3, pixels.Color(117, 0, 10));
    pixels.setPixelColor(4, pixels.Color(108, 0, 20));
    pixels.setPixelColor(5, pixels.Color(80, 1, 47));
    pixels.setPixelColor(6, pixels.Color(72,0,55));
    pixels.setPixelColor(7, pixels.Color(35, 57, 15));
    pixels.setPixelColor(8, pixels.Color(35, 57, 15));
    pixels.setPixelColor(9,pixels.Color(28,0,85));
    pixels.setPixelColor(10,pixels.Color(28,0,100));
    pixels.setPixelColor(11,pixels.Color(21,0,106));
    pixels.setPixelColor(12,pixels.Color(11,0,116));
    pixels.setPixelColor(13,pixels.Color(3,2,171));
    pixels.setPixelColor(14,pixels.Color(3,2,200));
    pixels.show();
  }
}

void sendMQTT() {                    //corresponse to 0014 workshop but adjusted for ArduinoMqttClient
    mqttClient.poll();
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    float temperature = bmp.readTemperature();
    mqttClient.print(temperature);
    mqttClient.endMessage();
    mqttClient.beginMessage(topic1);
    float pressure = bmp.readPressure()/100;
    mqttClient.print(pressure);
    mqttClient.endMessage();
    //mqttClient.beginMessage(topic2);
    //float altitude = bmp.readAltitude();
    //mqttClient.print(altitude);
    //mqttClient.endMessage();
    mqttClient.beginMessage(topic3);
    float volts = analogRead(A2) * 5.0 / 1024.0;
    float amps = volts / 10000.0; // across 10,000 Ohms
    float microamps = amps * 1000000;
    float lux = microamps * 2.0;
    mqttClient.print(lux);
    mqttClient.endMessage();
}
  
void loop() {
  // Set the frequency of reading data;
  // Call sensor functions;
  if (secondChanged()) {
    readData();
    sendMQTT();
    Serial.println(GB.dateTime("H:i:s")); // UTC.dateTime("l, d-M-y H:i:s.v T")
  }
  }
