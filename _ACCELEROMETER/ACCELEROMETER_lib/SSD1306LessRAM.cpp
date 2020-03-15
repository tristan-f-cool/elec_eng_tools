// This Software is provided under the MIT License


// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.


// Notice from original file SSD1306.cpp:
// some of this code was written by <cstone@pobox.com> originally; 
// it is in the public domain.


// SSD1306 display driver with less RAM usage.
// Made from SSD1306.cpp

// Added support for hardware SPI using SPI library with SCLK on D13 and MOSI on D11
// Note that D12 becomes an input and can only be used as MISO.  
// Also, D10 is SS when an input it will force the SPI interface to slave mode when low.

#define __PROG_TYPES_COMPAT__
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

//#if SID == 11 && SCLK == 13
#include <SPI.h>
//#endif

#include "SSD1306LessRAM.h"
#include "glcdfont.h"

// a 5x7 font table
extern const uint8_t PROGMEM font[];

// the memory buffer for the LCD 256 bytes

static uint8_t buffer[SSD1306_LCDWIDTH] = { 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

inline char getByte( const uint8_t* arr, int index, boolean progMem ) {
  return progMem ? pgm_read_byte( arr + index ) : arr[ index ];
}

void SSD1306::drawbitmap(uint8_t x, uint8_t y, 
                        const uint8_t *bitmap, uint8_t w, uint8_t h,
                        uint8_t color, boolean progMem) {
  uint8_t line, bitLine, j;
  if ( 0==h || 0==w || h > SSD1306_LCDHEIGHT || w>SSD1306_LCDWIDTH ) {
    return;  // Bail out if bitmap too big or too small to display.
  }
  for (j=0; j<h; j++) {
    bitLine = (y+j) % 8;
    for (uint8_t i=0; i<w; i++ ) {
      if (getByte(bitmap, i + (j/8)*w, progMem) & _BV(j%8) ) {//pgm_read_byte(bitmap + i + (j/8)*w) & _BV(j%8)) {
        setpixel(x+i, bitLine, color);
      }
      else // Make "off" pixels be in contrast with "on" pixels.
      {
        if ( WHITE == color ) {
          setpixel(x+i, bitLine, BLACK);
        }
        else {
          setpixel(x+i, bitLine, WHITE);
        }
      }
    }
    line = (y+j)/8;
    if ( 7 == bitLine ) {
      displayLine(line);
    }
  }
  if ( 0 == bitLine ) {
    displayLine(line);
  }
}

void SSD1306::drawstring(uint8_t x, uint8_t line, char *c, uint8_t color) {
  while (c[0] != 0) {
    drawchar(x, c[0], color);
    c++;
    x += 6; // 6 pixels wide
    if (x + 6 >= SSD1306_LCDWIDTH) {
      x = 0;    // ran out of this line
      displayLine(line);
      line++;
    }
    if (line >= (SSD1306_LCDHEIGHT/8))
      return;        // ran out of space :(
  }
  if ( x > 0 ) {
    displayLine(line);
  }
}

void SSD1306::drawstring_P(uint8_t x, uint8_t line, const char *c, uint8_t color) {
  char ch;
  ch = pgm_read_byte(c);
  while ( ch != 0) {
    drawchar(x, ch, color);
    c++;
    ch = pgm_read_byte(c);
    x += 6; // 6 pixels wide
    if (x + 6 >= SSD1306_LCDWIDTH) {
      x = 0;    // ran out of this line
      displayLine(line);
      line++;
    }
    if (line >= (SSD1306_LCDHEIGHT/8))
      return;        // ran out of space :(
  }
  if ( x > 0 ) {
    displayLine(line);
  }
}

void  SSD1306::drawchar(uint8_t x, uint8_t c, uint8_t color) {
  if (x >= (SSD1306_LCDWIDTH - 6)) {
      return;
  }
  if ( WHITE == color ) {
    for (uint8_t i =0; i<5; i++ ) {
      buffer[x] = pgm_read_byte(font+(c*5)+i);
      x++;
    }
    if (x < SSD1306_LCDWIDTH) {
      buffer[x] = 0x0;  // set the empty space past char to backround color.
    }
  } else {
    for (uint8_t i =0; i<5; i++ ) {
      buffer[x] = ~pgm_read_byte(font+(c*5)+i);
      x++;
    }
    if (x < SSD1306_LCDWIDTH) {
      buffer[x] = 0xFF;  // set the empty space past char to backround color.
    }
  }
}



// the most basic function, set a single pixel
void SSD1306::setpixel(uint8_t x, uint8_t y, uint8_t color) {
  if ((x >= SSD1306_LCDWIDTH) || (y >= 8))
    return;

  // x is which column
  if (color == WHITE) 
    buffer[x] |= _BV(y);  
  else
    buffer[x] &= ~_BV(y); 
}

void SSD1306::ssd1306_init(uint8_t vccstate) {
  // set pin directions
  pinMode(sid, OUTPUT);
  pinMode(sclk, OUTPUT);
  pinMode(dc, OUTPUT);
  pinMode(rst, OUTPUT);
  pinMode(cs, OUTPUT);

//#if sid == 11 && sclk == 13
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();
//#endif

  digitalWrite(rst, HIGH);
  // VDD (3.3V) goes high at start, lets just chill for a ms
  delay(1);
  // bring0xset low
  digitalWrite(rst, LOW);
  // wait 10ms
  delay(10);
  // bring out of reset
  digitalWrite(rst, HIGH);
  // turn on VCC (9V?)

   #if defined SSD1306_128_32
    // Init sequence for 128x32 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x1F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x10); }
    else 
      { ssd1306_command(0x14); }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
        ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x22); }
    else 
      { ssd1306_command(0xF1); }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  #endif

  #if defined SSD1306_128_64
    // Init sequence for 128x64 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x3F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x10); }
    else 
      { ssd1306_command(0x14); }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108 "Horizontal addressing" mode
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x9F); }
    else 
      { ssd1306_command(0xCF); }
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x22); }
    else 
      { ssd1306_command(0xF1); }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  #endif
  
  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}




inline void SSD1306::spiwrite(uint8_t c) {
//#if SID == 11 && SCLK == 13
  SPI.transfer(c);
//#else
//  shiftOut(sid, sclk, MSBFIRST, c);
//#endif
}
void SSD1306::ssd1306_command(uint8_t c) { 
  digitalWrite(cs, HIGH);
  digitalWrite(dc, LOW);
  digitalWrite(cs, LOW);
  spiwrite(c);
  digitalWrite(cs, HIGH);
}

void SSD1306::ssd1306_data(uint8_t c) {
  digitalWrite(cs, HIGH);
  digitalWrite(dc, HIGH);
  digitalWrite(cs, LOW);
  spiwrite(c);
  digitalWrite(cs, HIGH);
}

void SSD1306::ssd1306_set_brightness(uint8_t val) {
  
}


void SSD1306::displayLine(uint8_t line) {
  if (line >= (SSD1306_LCDHEIGHT/8))
    return;        // ran out of space :(
//  ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
//  ssd1306_command(SSD1306_SETHIGHCOLUMN | SSD1306_LCDWIDTH-1); // hi col = SSD1306_LCDWIDTH-1
//delay(1000);
  ssd1306_command(SSD1306_SETPAGEADDRESS); // setup current display line
  ssd1306_command(line); // start line
  ssd1306_command(line); // end line

  for (uint16_t i=0; i<(SSD1306_LCDWIDTH); i++) {
    ssd1306_data(buffer[i]);
  }
}

// clear everything in buffer
void SSD1306::clear_buf(void) {
  memset(buffer, 0, (SSD1306_LCDWIDTH));
}

void SSD1306::clear_display(void) {
  clear_buf();
  for (uint16_t line=0; line<(SSD1306_LCDHEIGHT/8); line++) {
    displayLine(line);
  }
}

