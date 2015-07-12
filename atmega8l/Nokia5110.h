/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {

  /*
   * Class to control the Nokia 5110 display
   */

  class Nokia5110 {

    protected:
      
      enum {
        LCD_EXTENDED_MODE = 0x21,
        LCD_NORMAL_MODE = 0x20,
        LCD_TEMPERATURE_COEFFICIENT = 0x4,
        LCD_BIAS = 0x14,
        LCD_NOT_INVERTED = 0xC,

        DEFAULT_CONTRAST = 80
      };

      static const PROGMEM uint8_t CharacterData[][5];
      static const PROGMEM uint8_t LargeNumberData[][22];

    protected:
      static void writeCommand(uint8_t command);
      static void writeData(uint8_t data);
      static void write(uint8_t value);
      static void writeData(uint8_t *data,uint16_t count);

    public:
      static void setup();

      static void clear();
      static void moveTo(uint8_t x,uint8_t y);
      static void writeString_P(uint8_t x,uint8_t y,const char *str_p,bool inverted);
      static void writeString(uint8_t x,uint8_t y,const char *str,bool inverted);
      static void writeCharacter(uint8_t c,bool inverted);
      static void writeLargeTemperature(uint8_t x,uint8_t y,uint16_t temperature);
      static void writeLargeNumber(uint8_t x,uint8_t y,uint16_t number);
      static void writeLargeDigit(uint8_t x,uint8_t y,uint8_t number);

      static void setBacklight(uint8_t percent);
      static void setContrast(uint8_t contrast);
  };



  /*
   * Setup. Do the initialisation.
   */

  inline void Nokia5110::setup() {

    uint8_t contrast;

    // reset the device

    GpioScreenReset::set();
    MillisecondTimer::delay(35);    // VDD to RES LOW min 30ms
    GpioScreenReset::reset();
    MillisecondTimer::delay(2);     // min RES pulse = 100ns
    GpioScreenReset::set();

    // initialise it

    writeCommand(LCD_EXTENDED_MODE);
    writeCommand(LCD_BIAS);
    writeCommand(LCD_TEMPERATURE_COEFFICIENT);

    // we must send 0x20 before modifying the display control mode
    
    writeCommand(LCD_NORMAL_MODE); 
    writeCommand(LCD_NOT_INVERTED);

    contrast=Eeprom::Reader::lcdContrast();
    
    if(contrast & 0x80) {
      contrast=DEFAULT_CONTRAST;
      Eeprom::Writer::lcdContrast(contrast);
    }

    setContrast(contrast);
    clear();
  }


  /*
   * clear the screen
   */

  inline void Nokia5110::clear() {

    uint8_t i,j;

    for(i=2;i;i--)        // 504 bytes to clear
      for(j=252;j;j--)
        writeData(0);
  }


  /*
   * move the screen cursor
   */

  inline void Nokia5110::moveTo(uint8_t x,uint8_t y) {
    writeCommand(0x80 | x);
    writeCommand(0x40 | y);
  }


  /*
   * write a string of characters to a single line
   */

  inline void Nokia5110::writeString_P(uint8_t x,uint8_t y,const char *str_p,bool inverted) {

    uint8_t b;

    writeCommand(0x40 | y);

    for(;;) {
      
      writeCommand(0x80 | x);
      x+=6;

      if((b=pgm_read_byte_near(str_p))==0)
        break;

      str_p++;
      writeCharacter(b,inverted);
    }
  }


  /*
   * write a string of characters to a single line
   */

  inline void Nokia5110::writeString(uint8_t x,uint8_t y,const char *str,bool inverted) {

    uint8_t b;

    writeCommand(0x40 | y);

    for(;;) {
      
      writeCommand(0x80 | x);
      x+=6;

      if((b=*str++)==0)
        break;

      writeCharacter(b,inverted);
    }
  }


  /*
   * Write a temperature in the large font. Temperatures are at most 3 digits
   * and are followed by the degrees-C suffix.
   */

  inline void Nokia5110::writeLargeTemperature(uint8_t x,uint8_t y,uint16_t number) {

    writeLargeNumber(x,y,number);
    writeLargeDigit(x+36,y,11);      // degree symbol
    writeLargeDigit(x+48,y,12);      // upper case C
  }


  /*
   * Write a number in the large font. Numbers are at most 3 digits
   */

  inline void Nokia5110::writeLargeNumber(uint8_t x,uint8_t y,uint16_t number) {

    uint8_t c,buffer[3];

    c=number/100;
    buffer[2]=c ? c : 10;
    number%=100;

    c=number/10; 
    buffer[1]=c || buffer[2]!=10 ? c : 10;
    number%=10;

    buffer[0]=number;

    writeLargeDigit(x,y,buffer[2]);
    writeLargeDigit(x+12,y,buffer[1]);
    writeLargeDigit(x+24,y,buffer[0]);
  }


  /*
   * Write out a single large digit
   */

  inline void Nokia5110::writeLargeDigit(uint8_t x,uint8_t y,uint8_t number) {

    uint8_t i,buf[22];

    memcpy_P(buf,LargeNumberData[number],22);

    moveTo(x,y);
    for(i=0;i<11;i++) 
      writeData(buf[i*2]);

    writeData(0);
    
    moveTo(x,y+1);
    for(i=0;i<11;i++) 
      writeData(buf[2*i+1]);
    
    writeData(0);
  }


  /*
   * write a single character to the current position
   */

  inline void Nokia5110::writeCharacter(uint8_t c,bool inverted) {

    uint8_t i,buf[6];

    memcpy_P(buf,CharacterData[c-0x20],5);
    buf[5]=0;

    if(inverted) {
      for(i=0;i<sizeof(buf);i++)
        buf[i]^=0xff;
    }

    writeData(buf,sizeof(buf));
  }


  /*
   * Write a buffer of data
   */


  inline void Nokia5110::writeData(uint8_t *data,uint16_t count) {

    // select the device, set data, { load data, wait }, deselect

    GpioScreenDataCommand::set();
    GpioScreenSelect::reset();
    
    while(count--) {
      SPDR=*data++;
      while(!(SPSR & (1 << SPIF)));
    }

    GpioScreenSelect::set();
  }


  /*
   * Write a command byte
   */

  inline void Nokia5110::writeCommand(uint8_t command) {

    // set command mode and write

    GpioScreenDataCommand::reset();
    write(command);
  }


  /*
   * write a byte of data
   */

  inline void Nokia5110::writeData(uint8_t data) {

    // set data mode and write

    GpioScreenDataCommand::set();
    write(data);
  }


  /*
   * Write a value given that the D/C line is set already
   */

  inline void Nokia5110::write(uint8_t value) {

    // select the device, load, wait, deselect

    GpioScreenSelect::reset();
    SPDR=value;
    while(!(SPSR & (1 << SPIF)));
    GpioScreenSelect::set();
  }


  /*
   * Set the backlight percentage
   */

  inline void Nokia5110::setBacklight(uint8_t percent) {

    OCR1A=(1023L*static_cast<uint32_t>(percent))/100;
  }


  /*
   * Set the contrast
   */

  inline void Nokia5110::setContrast(uint8_t contrast) {
    writeCommand(LCD_EXTENDED_MODE);
    writeCommand(0x80 | contrast);
    writeCommand(LCD_NORMAL_MODE); 
  }
}
