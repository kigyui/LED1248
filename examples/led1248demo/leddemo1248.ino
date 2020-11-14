// Interface with the flexible panel LED displays
// the ones that are controlled by LED1248 app
//
// https://github.com/kigyui/LED1248/

// Copyright 2020 Yui Izmori
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <Adafruit_GFX.h>
#include "ledfont.h"
#include "LED1248.h"

// we do have to put most of the commands into a buffer
// to send them as we need to know the total size before
// we send the first segment (etc). This should be enough
// for a 5 frame animation.

// width 32, height 16, 3 colour (rgb), 2048 bytes for a buffer is default
LED1248 myled(&Serial1, 32,16,3,2048);

void setup() {
  Serial.begin(115200);
  myled.setdebug(&Serial);
  myled.startup(64); // battery level to display 0-255
}

byte barbuf[8];
byte barcolbuf[8];
GFXcanvas8 canvas(96, 16);
GFXcanvas8 canvas2(32, 16);
GFXcanvas8 canvas3(32, 16);
GFXcanvas8 canvas4(32, 16);

void loop() {
  // 1. do an animation with 4 canvas
  canvas.fillScreen(myled.COLBLACK);
  canvas.setFont(&Nimbus_Sans_L_Regular_Condensed_12);
  canvas.setCursor(1,12);
  canvas.setTextColor(myled.COLGREEN);
  canvas.print("Hello");

  canvas2.fillScreen(myled.COLRED);
  canvas2.setFont(&Nimbus_Sans_L_Regular_Condensed_12);
  canvas2.setCursor(1,12);
  canvas2.print("World");

  canvas3.fillScreen(myled.COLGREEN);
  canvas3.setFont(&Nimbus_Sans_L_Regular_Condensed_12);
  canvas3.setCursor(1,12);
  canvas3.setTextColor(myled.COLBLACK);
  canvas3.print("Hi!");

  canvas4.fillScreen(myled.COLYELLOW);

  GFXcanvas8 canvases[] = {canvas,canvas2,canvas3,canvas4 };
  myled.animatefromcanvas(canvases,4, 500);
  myled.scrollspeed(250);  
  delay(8000);

  // 2. scroll some text
  canvas.setFont(&DejaVu_Sans_Mono_12);
  canvas.fillScreen(myled.COLBLACK);  
  canvas.setCursor(0,12); 
  canvas.setTextColor(myled.COLCYAN); canvas.print("M");
  canvas.setTextColor(myled.COLBLUE); canvas.print("e");  
  canvas.setTextColor(myled.COLRED); canvas.print("o");
  canvas.setTextColor(myled.COLYELLOW); canvas.print("w!..");
  
  myled.textfromcanvas(canvas,60); 
  myled.scrollmode(myled.SCROLLLEFT);
  
  delay(8000);  

  // 3. do a bargraph display
  for (int i=0; i<100; i++) {
    for (byte j=0; j<8; j++) {  
      barbuf[j] = random(0,17);
      barcolbuf[j] = (j+1+millis()/500)%7+1;  // rotate the colours
    }
    myled.bargraph(barbuf,barcolbuf);
  }
}
