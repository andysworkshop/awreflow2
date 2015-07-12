/*
 * Andy's Workshop Reflow Controller ATMega328p firmware
 * Copyright (c) 2015 Andy Brown. http://www.andybrown.me.uk
 * Please see website for licensing terms.
 */

#pragma once


namespace awreflow {


  /*
   * Management class for EEPROM access
   */

  struct Eeprom {

    /*
     * EEPROM locations
     */

    enum class Location : uint8_t {
      MAGIC           = 0,     // 2 byte signature
      PROFILE         = 2,     // 1 byte
      PROPORTIONAL    = 3,     // 2 bytes
      INTEGER         = 5,     // 2 bytes
      DERIVATIVE      = 7,     // 2 bytes
      ZERO_PERCENTAGE = 9,     // 1 byte
      MODE            = 10,    // 1 byte
      REFLOW_READY    = 11,    // 1 byte
      LCD_OPTIONS     = 12,    // 1 byte
      LCD_BACKLIGHT   = 13,    // 1 byte
      LCD_CONTRAST    = 14     // 1 byte
    };

    
    /*
     * The magic number that indicates valid EEPROM content
     */

    enum {
      MAGIC_NUMBER = 0xaa55
    };


    static void verifyState();
    static void writeDefaults();

    /*
     * High level read access
     */

    class Reader {
      
      protected:
        static uint16_t validate(uint16_t value);

      public:
        static uint16_t magic();
        static uint16_t constant(Location l);
        static uint8_t zeroPercentage();
        static uint8_t readByte(Location l);
        static uint8_t lcdContrast();
        static uint8_t lcdBacklight();
    };


    /*
     * High level write access
     */

    struct Writer {
      static void magic();
      static void constant(Location l,uint16_t c);
      static void zeroPercentage(uint8_t percentage);
      static void lcdContrast(uint8_t contrast);
      static void lcdBacklight(uint8_t backlight);
      static void writeByte(Location l,uint8_t b);
    };
  };


  /*
   * Read a byte
   */

  inline uint8_t Eeprom::Reader::readByte(Location l) {
    
    // read and return the byte

    return eeprom_read_byte(reinterpret_cast<uint8_t *>(l));
  }


  /*
   * Read a constant value
   */

  inline uint16_t Eeprom::Reader::constant(Location l) {
    return validate(eeprom_read_word(reinterpret_cast<uint16_t *>(l)));
  }


  /*
   * Validate a constant
   */

  inline uint16_t Eeprom::Reader::validate(uint16_t value) {
    return min<uint16_t>(999,value);
  }


  /*
   * Read the magic number
   */

  inline uint16_t Eeprom::Reader::magic() {
    return eeprom_read_word(reinterpret_cast<uint16_t *>(Location::MAGIC));
  }


  /*
   * Read the percentage that represents zero on your oven
   */

  inline uint8_t Eeprom::Reader::zeroPercentage() {
    return eeprom_read_byte(reinterpret_cast<uint8_t *>(Location::ZERO_PERCENTAGE));
  }


  /*
   * Read the LCD backlight percentage
   */

  inline uint8_t Eeprom::Reader::lcdBacklight() {
    return eeprom_read_byte(reinterpret_cast<uint8_t *>(Location::LCD_BACKLIGHT));
  }


  /*
   * Read the LCD contrast percentage
   */

  inline uint8_t Eeprom::Reader::lcdContrast() {
    return eeprom_read_byte(reinterpret_cast<uint8_t *>(Location::LCD_CONTRAST));
  }


  /*
   * Write a byte to the location
   */

  inline void Eeprom::Writer::writeByte(Location l,uint8_t b) {
    eeprom_write_byte(reinterpret_cast<uint8_t *>(l),b);
  }


  /*
   * Write a constant value
   */

  inline void Eeprom::Writer::constant(Location l,uint16_t c) {
    eeprom_write_word(reinterpret_cast<uint16_t *>(l),c);
  }


  /*
   * Write the magic number
   */

  inline void Eeprom::Writer::magic() {
    eeprom_write_word(reinterpret_cast<uint16_t *>(Location::MAGIC),MAGIC_NUMBER);
  }


  /*
   * Write the zero percentage value
   */

  inline void Eeprom::Writer::zeroPercentage(uint8_t percentage) {
    eeprom_write_byte(reinterpret_cast<uint8_t *>(Location::ZERO_PERCENTAGE),percentage);
  }


  /*
   * Write the LCD contrast
   */

  inline void Eeprom::Writer::lcdContrast(uint8_t contrast) {
    eeprom_write_byte(reinterpret_cast<uint8_t *>(Location::LCD_CONTRAST),contrast);
  }


  /*
   * Write the LCD backlight
   */

  inline void Eeprom::Writer::lcdBacklight(uint8_t backlight) {
    eeprom_write_byte(reinterpret_cast<uint8_t *>(Location::LCD_BACKLIGHT),backlight);
  }


  /*
   * Verify the content and default it if invalid
   */

  inline void Eeprom::verifyState() {
   
    // check for the signature and return if OK

    if(Reader::magic()==MAGIC_NUMBER)
      return;

    writeDefaults();
  }


  /*
   * Write the default values to EEPROM
   */

  inline void Eeprom::writeDefaults() {

    // write out default content

    Writer::writeByte(Location::PROFILE,static_cast<uint8_t>(ReflowProfileName::LEADED));
    Writer::writeByte(Location::MODE,0);
    Writer::writeByte(Location::REFLOW_READY,0);
    Writer::constant(Location::PROPORTIONAL,5);
    Writer::constant(Location::INTEGER,3);
    Writer::constant(Location::DERIVATIVE,3);
    Writer::zeroPercentage(0);
    Writer::lcdBacklight(100);
    Writer::lcdContrast(80);

    Writer::magic();
  }
}
