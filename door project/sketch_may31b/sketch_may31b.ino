#include <WiFi.h>
#include "time.h"
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h> 

#define SS_PIN 5 
#define RST_PIN  4
#define buzPin 25
MFRC522 mfrc522(SS_PIN, RST_PIN);   
LiquidCrystal_I2C lcd(0x27 , 16 ,2);
const int relayPin = 27 ; 
const char* host = "esp32";
const char* ssid     = "WLAN_2818";
const char* password = "erfan.askari021";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 12600;
const int   daylightOffset_sec = 3600;


WebServer server(80);

const String room = "251";   //class number
String URL = "http://192.168.1.5/test/apply_data.php"; // change the ip address

char yearNtp[5],dayNtp[3],monthNtp[3], hourChar[3];

int year, month, day, dateOut[3], hour;

const char* loginIndex = 
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<td>Username:</td>"
        "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";
 
/*
 * Server Index Page
 */
 
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')" 
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";




void setup() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  }

  digitalWrite(buzPin , 0);
  pinMode(buzPin , OUTPUT);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(relayPin , OUTPUT);

  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });

  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });

  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();
}

void loop() {
  server.handleClient();
  delay(1);
  if(WiFi.status() != WL_CONNECTED){
    WiFi.disconnect();
    WiFi.reconnect();
    lcd.setCursor(0, 0);
    lcd.print("      Wifi      ");
    lcd.setCursor(0, 1);
    lcd.print("  Disconnected  ");
    while (WiFi.status() != WL_CONNECTED);
  }
    

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){ 
    return;
  }
  strftime(yearNtp,5, "%Y", &timeinfo);
  strftime(monthNtp,3, "%m", &timeinfo);
  strftime(dayNtp,3, "%d", &timeinfo);
  strftime(hourChar,3, "%H", &timeinfo);
  //making var to int
  year = atoi(yearNtp);
  month = atoi(monthNtp);
  day = atoi(dayNtp);
  hour = atoi(hourChar);
  gregorian_to_jalali(year, month, day, dateOut);
  lcd.setCursor( 0 , 0);
  lcd.print(String(dateOut[0])+"/"+String(dateOut[1])+"/"+String(dateOut[2])+"  ");
  lcd.setCursor(11,0);
  hour--;
  lcd.print(hour);
  lcd.print(&timeinfo, ":%M");
  //lcd.print(&timeinfo, "%R");
  lcd.setCursor(0, 1);
  lcd.print(" Scan Your Card ");
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  String UidCard = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)  
  {
    UidCard.concat(String(mfrc522.uid.uidByte[i]  < 0x10 ? " 0" : " "));
    UidCard.concat(String(mfrc522.uid.uidByte[i],  HEX));
  }

  UidCard.toUpperCase();
  String postData = "UIDCard=" + String(UidCard) + "&Room=" + String(room);

  HTTPClient http; 
  http.begin(URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(postData); 

  String result = "1";
  result = http.getString();
  
  if (result == "1")
  {
    lcd.setCursor(0 , 1);
    lcd.print("   Authorized   ");
    digitalWrite(relayPin,1);
    delay(3000);
    digitalWrite(relayPin, 0);
  }
  else{
    lcd.setCursor(0 , 1);
    lcd.print(" Access  Denied ");
    digitalWrite(buzPin, 1);
    delay(200);
    digitalWrite(buzPin, 0);
    delay(100);
    digitalWrite(buzPin, 1);
    delay(200);
    digitalWrite(buzPin, 0);
  }  
  //http.end();
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