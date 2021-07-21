/* WEMOS ESP I-GAUGE
   BOARD : WEMOS D1 R2 MINI

   HISTORY:
   4-2-2019
   FIRST BUILD BY HOLLANDA
   CONFIGURE RTC DS3231  - LIBRARY RTCLIB
   MICRO SD CARD - SDFAT
   ADS1115 - ADS1015
   DISPLAY USING OLED SSD1306 128X64 - ADAFRUIT SSD1306

  18/7/2021
  Pemindahan ke server mantis baru
  22/7/2021
  Fiksasi
*/

//LIBRARY
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_ADS1015.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <math.h>
#include "config.h"

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 7 * 3600;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds);

IPAddress    apIP(10, 10, 10, 1);


//OLED
#define OLED_RESET -1
Adafruit_SSD1306 display(panjang, lebar,&Wire,OLED_RESET);
int16_t posx;
int16_t posy;
uint16_t w, h;

//DHT
DHT dht(DHTPIN, DHTTYPE);

//RTC DS3231
RTC_DS3231 rtc;
DateTime ESPtime;
DateTime sekarang;

//SD CARD
//SdFat SD;
File file;
String filename;

//ADS1115
Adafruit_ADS1115 ads(ads_addr);

void setup() {
  //initialisation
  Serial.begin(9600);

  //WELCOME SCREEN
  Serial.println(F("\r\nWIFI IGAUGE V 2019"));
  Serial.flush();
  
  display.begin(SSD1306_SWITCHCAPVCC, oled);
  display.clearDisplay();
  // display.display();
  // delay(100);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.getTextBounds(F("USAID - PDAM"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((panjang - w) / 2, 0);
  display.println(F("USAID - PDAM"));
  display.getTextBounds(F("WIFI OSH"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((panjang - w) / 2, 20);
  display.println(F("WIFI OSH"));
  display.getTextBounds(F("WATER PRESSURE"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((panjang - w) / 2, 30);
  display.println(F("WATER PRESSURE"));
  display.getTextBounds(F("SENSOR DEVICE"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((panjang - w) / 2, 40);
  display.println(F("SENSOR DEVICE"));
  display.getTextBounds(F("2019"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((panjang - w) / 2, 50);
  display.println(F("2019"));
  display.display();
  delay(4000);

  //RTC INIT
  hapusmenu(17, 64);
  display.setTextColor(WHITE, BLACK);
  display.getTextBounds(F("INIT CLOCK"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 20);
  display.println(F("INIT CLOCK"));
  display.display();
  delay(2000);

  initRTC();

  //SD INIT
  hapusmenu(17, 64);
  display.getTextBounds(F("init SD Card..."), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 20);
  display.println(F("INIT SD Card..."));
  display.display();
  delay(1000);

  initSD();

  Serial.println(F("SD CARD INIT OK!"));
  Serial.flush();
  display.getTextBounds(F("SD Card OK!"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 40);
  display.println(F("SD Card OK!"));
  display.display();
  delay(3000);

  //GET CONFIG TXT
  hapusmenu(17, 64);
  display.getTextBounds(F("CONFIGURATION FILE"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 20);
  display.println(F("CONFIGURATION FILE"));
  display.display();
  delay(1000);
  configs();
  delay(2000);
  //Configuration file extraction procedur
  //---------------------------------------------------------------

  //DISPLAY BURST INTERVAL
  hapusmenu(40, 64);
  display.getTextBounds(F("BURST SAMPLING"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 40);
  display.print(F("BURST SAMPLING"));
  display.getTextBounds(F("10 SECONDS"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 52);
  display.print(burst);
  if (burst < 2) {
    display.print(F(" SECOND"));
  }
  else display.print(F(" SECONDS"));
  display.display();
  delay(2000);
  //---------------------------------------------------------------
  //DISPLAY DATA INTERVAL
  hapusmenu(40, 64);
  display.getTextBounds(F("INTERVAL DATA"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 40);
  display.print(F("INTERVAL DATA"));
  display.getTextBounds(F("10 MINUTES"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 52);
  display.print(interval);
  if (interval == 1) {
    display.print(F(" MINUTE"));
  }
  if (interval > 1) {
    display.print(F(" MINUTES"));
  }
  display.display();
  delay(2000);
  //---------------------------------------------------------------

  //ADS initialization
  Serial.println(F("INIT ADS1115"));
  Serial.flush();
  ads.begin();
  hapusmenu(17, 64);
  display.getTextBounds(F("INIT PRESSURE SENSOR"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 20);
  display.println(F("INIT PRESSURE SENSOR"));
  display.display();
  delay(1000);
  display.getTextBounds(F("PRESSURE SENSOR READY"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 40);
  display.println(F("PRESSURE SENSOR READY"));
  display.display();
  delay(2000);
  /*
    //check water pressure sensor connected or not
    Serial.println(F("CEK PRESSURE SENSOR"));
    Serial.flush();
    hapusmenu(17, 64);
    display.getTextBounds(F("CEK PRESSURE SENSOR"), 0, 0, &posx, &posy, &w, &h);
    display.setCursor((128 - w) / 2, 20);
    display.println(F("CEK PRESSURE SENSOR"));
    display.display();
    delay(1000);
    //cek via ads1115
    reads = digitalRead(connectPres);

    Serial.println(reads);
    if (reads == 1) {
      Serial.println("pressure sensor 1 not connected");

      while (1) {
        display.getTextBounds(F("PRESSURE SENSOR"), 0, 0, &posx, &posy, &w, &h);
        display.setCursor((128 - w) / 2, 30);
        display.println(F("PRESSURE SENSOR"));
        display.getTextBounds(F("NOT CONNECTED"), 0, 0, &posx, &posy, &w, &h);
        display.setCursor((128 - w) / 2, 40);
        display.println(F("NOT CONNECTED"));
        display.display();
        delay(1000);
      }
    }

    Serial.println("pressure sensor 1 connected");
    display.getTextBounds(F("PRESSURE SENSOR"), 0, 0, &posx, &posy, &w, &h);
    display.setCursor((128 - w) / 2, 30);
    display.println(F("PRESSURE SENSOR"));
    display.getTextBounds(F("CONNECTED"), 0, 0, &posx, &posy, &w, &h);
    display.setCursor((128 - w) / 2, 40);
    display.println(F("CONNECTED"));
    display.display();
    delay(2000);
  */
  //DHT INIT
  initDHT();

  Serial.print(F("OPEN WIFI "));
  Serial.println(hotspot);
  Serial.print(F("IP="));
  Serial.println(apIP);

  hapusmenu(17, 64);
  display.getTextBounds(F("OPEN WIFI"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 20);
  display.print(F("OPEN WIFI"));
  display.getTextBounds((char*)hotspot.c_str(), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 35);
  display.print(hotspot);
  display.display();
  delay(500);
  display.getTextBounds(F("ip=10.10.10.1"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 50);
  display.print(F("IP="));
  display.print(apIP);
  display.display();
  delay(1000);

  waktu = millis();
  while (millis() - waktu < 3000) {
    reads = ads.readADC_SingleEnded(button);
    delay(1000);
  }
  Serial.print("reset button ");
  Serial.println(reads);

  //WIFI MANAGER
  //connect to network
  WiFiManager wifiManager;
  wifiManager.setAPStaticIPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); //IP, Gateway, DNS
  IPAddress myIP = WiFi.softAPIP();
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setBreakAfterConfig(true);

  if ( reads < 10 ) {
    wifiManager.resetSettings();
    wifiManager.setAPStaticIPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); //IP, Gateway, DNS
    IPAddress myIP = WiFi.softAPIP();
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setBreakAfterConfig(true);
    wifiManager.autoConnect((char*)hotspot.c_str());
    wifiManager.setSaveConfigCallback(saveConfigCallback);
  }

  if (!wifiManager.autoConnect((char*)hotspot.c_str())) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //system_deep_sleep(3e6);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
    //system_deep_sleep(5e6);
  }

  ////AFTER FINISH SETTING GOES TO HERE
  hapusmenu(17, 64);
  Serial.print(F("AP Local IP: "));
  Serial.println(WiFi.localIP());
  display.getTextBounds(F("CONNECTED TO AP"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 17);
  display.print(F("CONNECTED TO AP"));
  display.getTextBounds((char*)WiFi.SSID().c_str(), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 35);
  display.print(WiFi.SSID().c_str());
  display.display();
  delay(1000);
  filename = WiFi.localIP().toString();
  filename.toCharArray(sdcard, filename.length() + 1);
  display.getTextBounds(sdcard, 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 50);
  display.print(filename);
  display.display();  
//  WiFi.mode(WIFI_STA);
  

  //Waktu NTP
  timeClient.begin();
  ambilWaktu();
  delay(5000);

  display.clearDisplay();
  displaydate();

  //display menu parameter
  hapusmenu(17, 64);
  display.setCursor(0, 17);  display.print(F("Pres"));
  display.setCursor(0, 30);  display.print(F("Temp"));
  display.setCursor(0, 40);  display.print(F("Humid"));
  display.setCursor(0, 50);  display.print(F("Volt"));

  display.setCursor(32, 17);  display.print(F("="));
  display.setCursor(32, 30);  display.print(F("="));
  display.setCursor(32, 40);  display.print(F("="));
  display.setCursor(32, 50);  display.print(F("="));

  display.setCursor(80, 17);  display.print(F("bar"));
  display.setCursor(80, 25);  display.print("o");
  display.setCursor(85, 30);  display.print("C");
  display.setCursor(80, 40);  display.print(F("RH"));
  display.setCursor(80, 50);  display.print(F("V"));

  display.display();

//waktu tunggu
detik=30;
while(detik!=0){
  displaydate();
}
  //display.setCursor(100,52);  display.print(F("ERR"));
}

void loop() {
  waktu = millis();
  bersihdata();
  Serial.println(sTime);
//  Serial.println("ready 3to get data"); 

  // burst sampling
  reads1 = 0; reads = 0;
  for (i = 0; i < burst; i++) {
    reads1 = ads.readADC_SingleEnded(1); //pressure
    //nilai ads sensor #debug
//    Serial.print(i);    
//    Serial.print("=");
//    Serial.println(reads1);
    if (reads1 < 0) {
      reads = reads + 0;
    }
    else {
      reads = reads + reads1;
    }
    delay(1000);
  }
  
//  reads=random(0,30000);
//  Serial.println(reads);
  //KONVERSI
  volt = ((float)reads / (float)burst) * 0.1875 / 1000.0000; // nilai voltase dari nilai DN
  volt = volt * 147.00 / 100.00;
//  Serial.println(volt);
  tekanan = (300.00 * volt - 150.00) * 0.01 + float(offset);
  if (tekanan < 0) tekanan = 0;
  reads = ads.readADC_SingleEnded(tegangan);
  volt = (float)reads * 0.1875 / 1000.0000 * 147.00 / 100.00; // nilai voltase dari nilai DN
  humid = dht.readHumidity();      //humid
  temp = dht.readTemperature();    //temperature
  if (isnan(humid)) {
    humid = -99.9;
  }
  if (isnan(temp)) {
    temp = -99.9;
  }

  Serial.print(F("PRESSURE = "));
  Serial.println(tekanan, 1);
  Serial.print(F("TEMPERATURE = "));
  Serial.println(temp, 1);
  Serial.print(F("HUMIDITY = "));
  Serial.println(humid, 1);
  Serial.print(F("VOLTAGE = "));
  Serial.println(volt, 1);
  

  //display data
  display.fillRect(40, 17, 32, 64, BLACK); //clear display
  display.fillRect(100, 52, 32, 64, BLACK); //clear display
  display.display();
  display.setCursor(40, 17);  display.print(tekanan, 1);
  display.setCursor(40, 30);  display.print(temp, 1);
  display.setCursor(40, 40);  display.print(humid, 1);
  display.setCursor(40, 50);  display.print(volt, 1);
  display.display();
  

  //SIMPAN DATA
  Serial.println("Simpan data");
  simpandata();

  //KIRIM data
  Serial.println("Kirim data");
  httppost();
  delay(2000);
  while (millis() - waktu < interval * 60 * 1000) {
    displaydate();
  }
  displaydate();
}

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

String strTwoDigit(float nilai) {
  String result = String(nilai, 2);
  String angka, digit;
  b = 0;
  a = result.indexOf(".", b + 1);
  digit = result.substring(a + 1, result.length());
  angka = result.substring(0, a);
  if (result.length() == 4) { //4.12
    angka = "00" + angka + digit;
  }
  if (result.length() == 5) { //51.23
    angka = "0" + angka + digit;
  }
  if (result.length() > 5) { //123.45
    angka = angka + digit;
  }
  return angka;
}
