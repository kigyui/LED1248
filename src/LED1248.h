#ifndef LED1248_h
#define LED1248_h

#include <Adafruit_GFX.h>

class LED1248
{
  public:
    LED1248(HardwareSerial *hs, byte col, byte row=16, byte planes=3, int buflen=2048);
    void setdebug(Stream *s);
    void scrollmode(byte mode);
    void scrollspeed(byte ss);
    void brightness(byte brightness);
    void displayswitch(boolean onoff);
    void charging();
    void powerdown();    
    void invertdisplay();
    void startup(byte batterylevel=255);
    void animatefromcanvas(GFXcanvas8 *canvas, byte frames, int frspeed, boolean blank=true);
    void textfromcanvas(GFXcanvas8 canvas, int cols, boolean blank=true);
    void bargraph(byte *bars, byte *cols);
    typedef enum {
        COLRED = 1,
        COLGREEN = 2,
        COLYELLOW = 3,
        COLBLUE = 4,
        COLMAGENTA = 5,
        COLCYAN = 6,
        COLWHITE = 7,
        COLBLACK = 0,
    };
    typedef enum {
        SCROLLSTATIC = 1,
        SCROLLLEFT = 2,
        SCROLLRIGHT = 3,
        SCROLLUP = 4,
        SCROLLDOWN = 5,
        SCROLLSNOWFLAKE = 6,
        SCROLLPICTURE = 7,
        SCROLLLASER = 8,
    };
      
  private:
    int _buflen;
    byte *cbuf;
    byte _row;
    byte _col;
    byte _planes;
    byte _pc=0;
    boolean _debug = false;
    HardwareSerial *_sled;
    Stream *_sdebug;
    
    void sendsingle(byte x);
    void sendbyte(byte x);
    void bsend(byte *buf, int blen, boolean sendchecksum);
    void sendsplitdata(byte c1, byte c2, byte *cbuf, int clen);
    void text(byte *text, int text_len, boolean blank); // old
};

#endif

