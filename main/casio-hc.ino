#include <U8g2lib.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <OneButton.h>

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
OneButton nut1(12, true);
OneButton nut2(13, true);

String fileContent, DATABASE_PATH, nut;
int page_here, page, tt;
int lineHeight, startY, scrollSpeed, totalTextHeight;
bool huong, kt, landau;

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ -1);

void setup() {
  // set font
  display.begin();
  display.setFont(u8g2_font_6x10_tr);
  // connect wifi and ko phát
  WiFi.mode(WIFI_STA);
  WiFi.begin("tro 1", "01010101");
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
  config.database_url = "casio-5e6f3-default-rtdb.asia-southeast1.firebasedatabase.app";
  config.api_key = "AIzaSyCnbruR9nVKIbkSZTpYtp4RWNRiHQ9R0KM";
  Firebase.signUp(&config, &auth, "", "");
  Firebase.begin(&config, &auth);
  // buttons
  nut="";
  nut1.attachClick(singleTapUp);
  nut2.attachClick(singleTapDown);
  nut1.attachDoubleClick(doubleTapUp);
  nut2.attachDoubleClick(doubleTapDown);
  nut1.attachDuringLongPress(longPressUp);
  nut2.attachDuringLongPress(longPressDown);
  nut1.setDebounceTicks(50);
  nut2.setDebounceTicks(50);
  // Lấy số trang
  DATABASE_PATH = "tong";
  while(!ktfb()) delay(100);
  page = firebaseData.stringData().toInt();
  // get data page
  huong=true; DATABASE_PATH = "luu_trang";
  while(!ktfb()) delay(100);
  page_here = firebaseData.stringData().toInt();
  kt=true;
}

void loop() {
  nut1.tick(); nut2.tick();
  if(nut == "MENU") doubleTapUp();
  if(nut == ""){
    if(kt){
      // get data
      DATABASE_PATH = "trang"+String(page_here);
      while(!ktfb()) delay(100);
      fileContent = firebaseData.stringData(); fileContent.replace("\t", " "); fileContent.replace("  ", " ");
      // Save page here
      Firebase.set(firebaseData, "luu_trang", page_here);
      // print and scroll
      tinhtoan(); kt=false; landau=true;
    }
    tt=print();
    if(tt == 1){
      if(page_here < page) page_here++;
      else page_here=1;
      huong=true; kt=true;
    }
    if(tt == 2){
      if(page_here > 1) page_here--;
      else page_here=page;
      huong=false; kt=true;
    }
  }
  delay(10);
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
void tinhtoan(){
  lineHeight = display.getFontAscent() - display.getFontDescent() + 2;
  startY = 10;
  scrollSpeed = 8;

  // Tính toán chiều cao của toàn bộ văn bản
  totalTextHeight = 0;
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
}
int print(){
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);

  // Chia nội dung thành nhiều dòng và hiển thị
  int currentX = 0;
  int currentY = startY;
  String currentWord = "";
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
    startY = -totalTextHeight+50;
    huong=true;
    return 0;
  }

  // Kiểm tra xem đã cuộn hết văn bản chưa
  if (startY < -totalTextHeight+50 && !landau) return 1;
  else if (startY > 10 && !landau) return 2;
  else return 0;
}