#include <U8g2lib.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>

#define WIFI_SSID "tro 1"
#define WIFI_PASSWORD "01010101"
#define FIREBASE_HOST "casio-5e6f3-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyCnbruR9nVKIbkSZTpYtp4RWNRiHQ9R0KM"
#define OLED_RESET -1
#define HALL_SENSOR_LEFT_PIN 12
#define HALL_SENSOR_RIGHT_PIN 13

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

String fileContent, DATABASE_PATH;
int nut1,nut2;
bool huong;

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ OLED_RESET);

void setup() {
  // set font
  display.begin();
  display.setFont(u8g2_font_6x10_tr);
  // connect wifi and ko phát
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.clearBuffer();
    display.drawStr(10, 10, "WiFi connecting...");
    display.sendBuffer();
  }
  display.clearBuffer();
  display.drawStr(10, 10, "WiFi connected");
  display.sendBuffer();
  // connect firebase
  config.database_url = FIREBASE_HOST;
  config.api_key = FIREBASE_AUTH;
  Firebase.signUp(&config, &auth, "", "");
  Firebase.begin(&config, &auth);
  // khởi tạo kiểu chân cảm biến
  pinMode(HALL_SENSOR_LEFT_PIN, OUTPUT);
  pinMode(HALL_SENSOR_RIGHT_PIN, OUTPUT);
}

void loop() {
  // Lấy số trang
  DATABASE_PATH = "tong";
  while(!ktfb()) delay(100);
  int page = firebaseData.stringData().toInt();

  // get data page
  int page_here=1; huong=true;
  while(true){
    DATABASE_PATH = "trang"+String(page_here);
    while(!ktfb()) delay(100);
    fileContent = firebaseData.stringData();
    if(print()){
      if(page_here < page) page_here++;
      else page_here=1;
      huong=true;
    } else {
      if(page_here > 1) page_here--;
      else page_here=page;
      huong=false;
    }
  }
}

bool ktfb(){
  if (Firebase.ready()){
    if (Firebase.getString(firebaseData, DATABASE_PATH)) return true;
    else{
      display.clearBuffer();
      display.drawStr(10, 10, "Failed to get data");
      display.sendBuffer();
      return false;
    }
  }else{
    display.clearBuffer();
    display.drawStr(10, 10, "Firebase is not ready");
    display.sendBuffer();
    return false;
  }
}
bool print(){
  fileContent.replace("\t", " ");
  fileContent.replace("  ", " ");

  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);

  int lineHeight = display.getFontAscent() - display.getFontDescent() + 2;
  int startY = 10;
  int scrollSpeed = 3;

  // Tính toán chiều cao của toàn bộ văn bản
  int totalTextHeight = 0;
  int currentX = 0;
  String currentWord = "";
  for (int i = 0; i < fileContent.length(); i++) {
    char currentChar = fileContent.charAt(i);
    if (currentChar == ' ' || currentChar == '\n') {
      if (currentX + display.getStrWidth((currentWord + " ").c_str()) > 128) {
        currentX = 0;
        totalTextHeight += lineHeight;
      }
      currentX += display.getStrWidth((currentWord + " ").c_str());
      currentWord = "";
      if (currentChar == '\n') {
        currentX = 0;
        totalTextHeight += lineHeight;
      }
    } else {
      currentWord += currentChar;
    }
  }
  if (currentWord.length() > 0) {
    totalTextHeight += lineHeight;
  }

  // if(huong) int startY = 10;
  // else int startY = -totalTextHeight+30;

  while (true) {
    display.clearBuffer();

    // Chia nội dung thành nhiều dòng và hiển thị
    currentX = 0;
    int currentY = startY;
    currentWord = "";
    for (int i = 0; i < fileContent.length(); i++) {
      char currentChar = fileContent.charAt(i);
      if (currentChar == ' ' || currentChar == '\n') {
        if (currentX + display.getStrWidth((currentWord + " ").c_str()) > 128) {
          currentX = 0;
          currentY += lineHeight;
        }
        display.drawStr(currentX, currentY, currentWord.c_str());
        currentX += display.getStrWidth((currentWord + " ").c_str());
        currentWord = "";
        if (currentChar == '\n') {
          currentX = 0;
          currentY += lineHeight;
        }
      } else {
        currentWord += currentChar;
      }
    }
    if (currentWord.length() > 0) {
      if (currentX + display.getStrWidth(currentWord.c_str()) > 128) {
        currentX = 0;
        currentY += lineHeight;
      }
      display.drawStr(currentX, currentY, currentWord.c_str());
    }

    display.sendBuffer();
    
    if(huong==false){
      startY = -totalTextHeight+30;
      huong=true;
      continue;
    }

    while(true){
      digitalWrite(12, LOW); digitalWrite(12, HIGH);
      digitalWrite(13, LOW); digitalWrite(13, HIGH);
      nut1 = digitalRead(HALL_SENSOR_LEFT_PIN);
      nut2 = digitalRead(HALL_SENSOR_RIGHT_PIN);
      if(nut1 == HIGH && nut2 == LOW){
        startY -= scrollSpeed; break;
      }
      if(nut1 == LOW && nut2 == HIGH){
        startY += scrollSpeed; break;
      }
      delay(100);
    }
    // Kiểm tra xem đã cuộn hết văn bản chưa
    if (startY < -totalTextHeight+30) return true;
    if (startY > 10) return false;
  }
}