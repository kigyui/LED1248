// Talk to the bluetooth LED matrix
//
// Write data to the module using the single bluetooth characteristic
// or we just connect directly from an Ardunio to the board tx/rx
// as it runs at 38400
//
// Sending "01" is the start of a command and "03" is the end of a
// command. Those can't appear in the body, so "02" is escape:
// i.e. "0205" is a "01", "0206" is a "02" and "0207" is a "03".
//
// Split the commands to send a max of 20 bytes at once, and if the
// overall command length is more than 128 bytes we split that into
// multiple commands. Some commands take a simple checksum at the end.
//
// For 1 plane bitmaps: Display is 48x12 bits top 0-f middle 0-f bottom 0-f lsb
// is bottom, last byte ignored.

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

#include "Arduino.h" 
#include "LED1248.h"
#include <Adafruit_GFX.h>

// we do have to put most of the commands into a buffer
// to send them as we need to know the total size before
// we send the first segment (etc)

LED1248::LED1248(HardwareSerial *hs, byte col, byte row, byte planes, int buflen) {
    _buflen = buflen;
    _row = row;
    _col = col;
    _planes = planes;
    _sled = hs;
    cbuf=(byte *)malloc(buflen);
    return;
}

void LED1248::setdebug(Stream *s) {
    _debug = true;
    _sdebug = s;
}

// Public Methods

// 1 static 2 left 3 right 4 up 5 down 6 snowflake 7 picture 8 laser
void LED1248::scrollmode(byte mode) {
    cbuf[0] = 0x6; cbuf[1] = mode; bsend(cbuf,2,false);  
}
void LED1248::scrollspeed(byte ss) {
    cbuf[0] = 0x7; cbuf[1] = ss; bsend(cbuf,2,false);  
}

void LED1248::brightness(byte brightness) {
    cbuf[0] = 0x8; cbuf[1] = brightness; bsend(cbuf,2,false);   
}

void LED1248::displayswitch(boolean onoff) {
    cbuf[0] = 0x09;
    if (onoff) cbuf[1] = 1; else cbuf[1] = 0;
    bsend(cbuf,2,false);  
}

// Show a charging animation
void LED1248::charging() {
    cbuf[0] = 0x11; bsend(cbuf,1,false);  
}

void LED1248::invertdisplay() {
    cbuf[0] = 0x15; bsend(cbuf,1,false);  
}

void LED1248::powerdown() {
    cbuf[0] = 0x12; bsend(cbuf,1,false);  
}

// Do the startup display and show the battery level
void LED1248::startup(byte batterylevel) {
    _sled->begin(38400);
    cbuf[0] = 0x13; cbuf[1] = batterylevel; bsend(cbuf,2,false);
    delay(1400); // it does a little animation thing, 1400 by testing
}

// text from a bitmap buffer, not very useful
void LED1248::text(byte *text, int text_len, boolean blank) {
    int i = 0;
    for (byte j=0;j<25;j++) cbuf[i++] = 0;
    // this says we have 9 characters each given the pixel width.  this is used so we don't display
    // a character in static mode over the edge of the screen, but it can just be ignored.  Pad to 80
    // cbuf[i++]=0x09; cbuf[i++]=0x0c; cbuf[i++]=0x0c; cbuf[i++]=0x0c; cbuf[i++]=0x0c; cbuf[i++]=0x0c; cbuf[i++]=0x0c; cbuf[i++]=0x10; cbuf[i++]=0x10; cbuf[i++]=0x0a;
    
    for (byte j=0; j<80; j++) {
        cbuf[i++] = 0;
    }
    cbuf[i++] = (text_len*3)/256; cbuf[i++] = (text_len*3)%256;
    for (byte n=0; n<_planes; n++) {
        for (byte j=0; j<text_len; j++) {
            cbuf[i++] = text[j];  
        }
    }
    if (blank) displayswitch(false);
    sendsplitdata(0x02,0x00,cbuf,i);
    if (blank) displayswitch(true);
}

void LED1248::animatefromcanvas(GFXcanvas8 *canvas, byte frames, int frspeed, boolean blank) {
    int i = 0;
    for (byte j=0;j<24;j++) cbuf[i++] = 0x00;
    cbuf[i++] = frames; 
    cbuf[i++] = frspeed/256; cbuf[i++] = frspeed%256;
    
    byte cols=_col;
    byte colplane = 1;
    if (_planes == 1) colplane=255;
    for (byte n=0; n<_planes; n++) {
        for (byte z=0; z< frames; z++) {
            for (byte j=0; j<cols; j++) {
                for (byte x = 0; x<(_row+7)/8; x++) {
                    byte pixel = 0;
                    for (int16_t k=0; k<8; k++) {
                        if (((canvas[z].getPixel(j,k+x*8))&colplane)>0 ) pixel|=1;
                        if (k<7) pixel=pixel<<1;
                    }
                    cbuf[i++] = pixel;
                    if (i>_buflen-1) return;
                }
            }
        }
        colplane=colplane<<1;
    }
    if (blank) displayswitch(false);
    sendsplitdata(0x04,0x00,cbuf,i);
    if (blank) displayswitch(true);
}

void LED1248::textfromcanvas(GFXcanvas8 canvas, int cols, boolean blank) {
    int i = 0;
    for (byte j=0; j<25;j++) cbuf[i++] = 0;
    // this says we have 9 characters each given the pixel width.  this is used so we don't display
    // a character in static mode over the edge of the screen, but it can just be ignored.  Pad to 80
    // cbuf[i++]=0x09; cbuf[i++]=0x0c; cbuf[i++]=0x0c; cbuf[i++]=0x0c; cbuf[i++]=0x0c; cbuf[i++]=0x0c; cbuf[i++]=0x0c; cbuf[i++]=0x10; cbuf[i++]=0x10; cbuf[i++]=0x0a;  
    for (byte j=0; j<80; j++) cbuf[i++] = 0;
    cbuf[i++] = (_planes*cols*2)/256; cbuf[i++] = (_planes*cols*2)%256;
    byte colplane = 1;
    if (_planes ==1) colplane = 255;
    
    for (byte n=0; n<_planes; n++) {
        for (byte j=0; j<cols; j++) {
            for (byte x = 0; x<(_row+7)/8; x++) {
                byte pixel = 0;
                for (int16_t k=0; k<8; k++) {
                    if ((canvas.getPixel(j,k+x*8)&colplane)>0) pixel|=1;
                    if (k<7) pixel=pixel<<1;
                }
                cbuf[i++] = pixel;
                if (i>_buflen-1) return;        
            }
        }
        colplane=colplane<<1;
    }
    if (blank) displayswitch(false);
    sendsplitdata(0x02,0x00,cbuf,i);
    if (blank) displayswitch(true);
}

void LED1248::bargraph(byte *bars, byte *colours) {
    byte i = 0;
    cbuf[i++] = 0x01;
    for (byte j=0; j<8; j++) {
        cbuf[i++] = bars[j];
    }
    if (colours) {
        for (byte j=0; j<8; j++) {
            cbuf[i++] = colours[j];
        }    
    }
    bsend(cbuf,i,false);
}

// Private Methods

void LED1248::sendsingle(byte x) {
    _sled->write(x);
    if (_debug) _sdebug->printf("%02x",x);
    _pc++; if (_pc>20) { if (_debug) _sdebug->println(); _pc=0; };
}

// 01 is used for start of record, 03 for end of record so we have
// to encode those by using 02 as an escape character.

void LED1248::sendbyte(byte x) {
    if (x == 1 || x == 2 || x == 3) {
        sendsingle(0x02); 
        sendsingle(x+4);
    } else {
        sendsingle(x);
    }  
}

// send a entire command. some commands need the checksum, some don't

void LED1248::bsend(byte *buf, int blen, boolean sendchecksum) {
    _pc = 0;
    byte checksum = 0;
    int slen = blen;
    if (sendchecksum) slen=slen+1;
    sendsingle(0x01);  
    sendbyte(slen/256); sendbyte(slen%256);
    for (byte i=0; i<blen; i++) {
        if (i>0) checksum ^= buf[i];
        sendbyte(buf[i]);
    }
    if (sendchecksum) sendsingle(checksum);
    sendsingle(0x03);
    if (_debug) _sdebug->println();
    // the rx pin isn't uart serial, so we don't need to bother connecting
    // it and can just wait the max time it usually takes for the processing
    delay(100);
}

// text and animation require us to split the command up, this is true 
// even connected directly and not via bluetooth.  Must send the 
// checksum on these commands.

#define sendmax 128

void LED1248::sendsplitdata(byte c1, byte c2, byte *cbuf, int clen) {
    byte nbuf[sendmax+10];
    if (_debug) _sdebug->printf("sendsplit len=%d\n",clen);
    byte ncount = 0;
    for (int i=0; i<clen; i+=sendmax) {
        byte n = 0;
        nbuf[n++]=c1; nbuf[n++]=c2;
        nbuf[n++]=clen/256; nbuf[n++]=clen%256;
        nbuf[n++]=ncount/256; nbuf[n++]=ncount%256;
        ncount++;
        int thislen = sendmax;
        if (i+thislen > clen) thislen = clen-i;
        nbuf[n++]=(byte)thislen;
        for (byte j=0; j<thislen; j++) {
            nbuf[n++]=cbuf[j+i];
        }
        if (_debug) _sdebug->printf("sendsplit send=%d\n",thislen+7);    
        bsend(nbuf,n,true);
    }
}
