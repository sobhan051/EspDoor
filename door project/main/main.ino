#include <WiFi.h>
#include "time.h"
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <HTTPClient.h> 

#define RST_PIN  4
#define SS_PIN 5

MFRC522 mfrc522(SS_PIN, RST_PIN);   
LiquidCrystal_I2C lcd(0x27 , 16 ,2);

const int relayPin = 27; 
const int denyPin = 26;
const int buzPin = 25; 

const char* ssid     = "WLAN_2818";
const char* password = "erfan.askari021";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 12600;
const int   daylightOffset_sec = 3600;
const String room = "251";   // class number
String URL = "http://192.168.1.2/test/apply_data.php"; // change the IP address

char yearNtp[5];
char monthNtp[3];
char dayNtp[3];
char hourNtp[3];
char testNtp[3];
int year, month, day, hour, test, dateOut[3];

void setup() {
  // Serial.begin(9600);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);  // Wait until connected
  }
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  lcd.init();
  lcd.clear();
  lcd.backlight();
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(relayPin, OUTPUT);
  pinMode(denyPin, OUTPUT);
  pinMode(buzPin, OUTPUT);
  digitalWrite(buzPin, 0);
}

void loop() {
  if (WiFi.status() == WL_CONNECTION_LOST || WiFi.status() == WL_DISCONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("      Wifi      ");
    lcd.setCursor(0, 1);
    lcd.print("  Disconnected  ");
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);  // Wait until reconnected
    }
  }

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) { 
    return;
  }

  strftime(yearNtp, 5, "%Y", &timeinfo);
  strftime(monthNtp, 3, "%m", &timeinfo);
  strftime(dayNtp, 3, "%d", &timeinfo);
  strftime(hourNtp, 3, "%H", &timeinfo);
  strftime(testNtp, 3, "%M", &timeinfo);

  // Making variables to int
  year = atoi(yearNtp);
  month = atoi(monthNtp);
  day = atoi(dayNtp);
  hour = atoi(hourNtp);
  test = atoi(testNtp);

  // Check for the condition to go to sleep
  if (test == 35) {
    lcd.clear();
    lcd.noBacklight();
    lcd.noBlink();
    lcd.noCursor();
    lcd.noDisplay();
    
    // Sleep for 60 seconds (60000000 microseconds)
    esp_sleep_enable_timer_wakeup(60000000);  // Sleep for 60 seconds
    esp_deep_sleep_start();  // Enter deep sleep mode
  }

  hour--;
  String h = String(hour);
  gregorian_to_jalali(year, month, day, dateOut);
  lcd.setCursor(0, 0);
  lcd.print(String(dateOut[0]) + "/" + String(dateOut[1]) + "/" + String(dateOut[2]) + "  ");
  lcd.setCursor(11, 0);
  lcd.print(h);
  lcd.print(&timeinfo, ":%M");
  lcd.setCursor(0, 1);
  lcd.print(" Scan Your Card ");

  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  String UidCard = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {  
    UidCard.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    UidCard.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  UidCard.toUpperCase();
  String postData = "UIDCard=" + String(UidCard) + "&Room=" + String(room);
  
  HTTPClient http; 
  http.begin(URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(postData); 
  String result = http.getString();
  
  if (result == "1") {
    lcd.setCursor(0, 1);
    lcd.print("   Authorized   ");
    digitalWrite(relayPin, 1);
    delay(5000);
    digitalWrite(relayPin, 0);
  } else {
    lcd.setCursor(0, 1);
    lcd.print(" Access Denied ");
    digitalWrite(denyPin, 1);
    digitalWrite(buzPin, 1);
    delay(50);
    digitalWrite(buzPin, 0);
    delay(100);
    digitalWrite(buzPin, 1);
    delay(50);
    digitalWrite(denyPin, 0);
    digitalWrite(buzPin, 0);
  }

  http.end();
}

int *gregorian_to_jalali(int gy, int gm, int gd, int out[]) {
  out[0] = (gm > 2) ? (gy + 1) : gy;
  {
    int g_d_m[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    out[3] = 355666 + (365 * gy) + ((int)((out[0] + 3) / 4)) - ((int)((out[0] + 99) / 100)) + ((int)((out[0] + 399) / 400)) + gd + g_d_m[gm - 1];
  }
  out[0] = -1595 + (33 * ((int)(out[3] / 12053)));
  out[3] %= 12053;
  out[0] += 4 * ((int)(out[3] / 1461));
  out[3] %= 1461;
  if (out[3] > 365) {
    out[0] += (int)((out[3] - 1) / 365);
    out[3] = (out[3] - 1) % 365;
  }
  if (out[3] < 186) {
    out[1]/*jm*/ = 1 + (int)(out[3] / 31);
    out[2]/*jd*/ = 1 + (out[3] % 31);
  } else {
    out[1]/*jm*/ = 7 + (int)((out[3] - 186) / 30);
    out[2]/*jd*/ = 1 + ((out[3] - 186) % 30);
  }
  return out;
}
