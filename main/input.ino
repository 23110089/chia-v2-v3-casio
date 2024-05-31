//single tap for selection
void singleTapUp()
{
  if(nut == "MENU"){
    if(page_here < page) page_here++;
    else page_here=1;
  } else{
    startY-=scrollSpeed; landau=false;
  }
}

void singleTapDown()
{
  if(nut == "MENU"){
    if(page_here > 1) page_here--;
    else page_here=page;
  } else {
    startY+=scrollSpeed; landau=false;
  }
}

//multitap for going inside the menu and also to return back to menu
void doubleTapUp()
{
  nut = "MENU";
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(0, 10, "PAGE:");
  display.drawStr(32, 10, String(page_here).c_str());
  display.sendBuffer();
    
}

void doubleTapDown()
{
  if(nut != "MENU") return;
  nut = ""; kt = true;
}

//Long press for scrolling +y and -Y direction and also to cycle through text message
void longPressUp()
{
  if(nut == "MENU"){
    if(page_here < page) page_here++;
    else page_here=1;
    delay(200);
  } else{
    startY-=scrollSpeed; landau=false;
  }
}

void longPressDown()
{
  if(nut == "MENU"){
    if(page_here > 1) page_here--;
    else page_here=page;
    delay(200);
  } else{
    startY+=scrollSpeed; landau=false;
  }
}