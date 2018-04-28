// Based on the work by DFRobot

#include "LiquidCrystal_I2C_rus.h"
#include <inttypes.h>

const char utf_recode[] PROGMEM=
       { 0x41,0xa0,0x42,0xa1,0xe0,0x45,0xa3,0xa4,0xa5,0xa6,0x4b,0xa7,0x4d,0x48,0x4f,
         0xa8,0x50,0x43,0x54,0xa9,0xaa,0x58,0xe1,0xab,0xac,0xe2,0xad,0xae,0x62,0xaf,0xb0,0xb1,
         0x61,0xb2,0xb3,0xb4,0xe3,0x65,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0x6f,
         0xbe,0x70,0x63,0xbf,0x79,0xe4,0x78,0xe5,0xc0,0xc1,0xe6,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7
        };     

//-----------------------------------

#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#define printIIC(args)	Wire.write(args)
 size_t LiquidCrystal_I2C_rus::write(uint8_t value)// {
//	return 1;
//}

#else
#include "WProgram.h"

#define printIIC(args)	Wire.send(args)
 void LiquidCrystal_I2C_rus::write(uint8_t value)// {
//	send(value, Rs);
//}

#endif
{
uint8_t out_char=value;
/*  if (_dram_model == LCD_DRAM_WH1601) {  
    uint8_t ac=recv(LOW) & 0x7f;
    if (ac>7 && ac<0x14) command(LCD_SETDDRAMADDR | (0x40+ac-8));
  }*/

  if (value>=0x80) { // UTF-8 handling
    if (value >= 0xc0) {
      utf_hi_char = value - 0xd0;
    } else {
      value &= 0x3f;
      if (!utf_hi_char && (value == 1)) 
        send(0xa2,HIGH); // 
      else if ((utf_hi_char == 1) && (value == 0x11)) 
        send(0xb5,HIGH); // 
      else 
        send(pgm_read_byte_near(utf_recode + value + (utf_hi_char<<6) - 0x10), HIGH);
    }    
  } else send(out_char, HIGH);

#if defined(ARDUINO) && ARDUINO >= 100
  return 1; // assume sucess 
#endif
}
#include "Wire.h"
//***************
/*#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif



#if defined(ARDUINO) && ARDUINO >= 100
  size_t LiquidCrystal_I2C_rus::write(uint8_t value)
#else
  void   LiquidCrystal_I2C_rus::write(uint8_t value)
#endif
{
  uint8_t out_char=value;

  if (_dram_model == LCD_DRAM_WH1601) {  
    uint8_t ac=recv(LOW) & 0x7f;
    if (ac>7 && ac<0x14) command(LCD_SETDDRAMADDR | (0x40+ac-8));
  }

  if (value>=0x80) { // UTF-8 handling
    if (value >= 0xc0) {
      utf_hi_char = value - 0xd0;
    } else {
      value &= 0x3f;
      if (!utf_hi_char && (value == 1)) 
        send(0xa2,HIGH); // 
      else if ((utf_hi_char == 1) && (value == 0x11)) 
        send(0xb5,HIGH); // 
      else 
        send(pgm_read_byte_near(utf_recode + value + (utf_hi_char<<6) - 0x10), HIGH);
    }    
  } else send(out_char, HIGH);
#if defined(ARDUINO) && ARDUINO >= 100
  return 1; // assume sucess 
#endif
}                     */

//------------------------------------


// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LiquidCrystal_I2C_rus::LiquidCrystal_I2C_rus(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows)
{
  _Addr = lcd_Addr;
  _cols = lcd_cols;
  _rows = lcd_rows;
  _backlightval = LCD_NOBACKLIGHT;
}

void LiquidCrystal_I2C_rus::init(){
	init_priv();
}

void LiquidCrystal_I2C_rus::init_priv()
{
	Wire.begin();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	begin(_cols, _rows);  
}

void LiquidCrystal_I2C_rus::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50); 
  
	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	  // we start in 8bit mode, try to set 4 bit mode
   write4bits(0x03 << 4);
   delayMicroseconds(4500); // wait min 4.1ms
   
   // second try
   write4bits(0x03 << 4);
   delayMicroseconds(4500); // wait min 4.1ms
   
   // third go!
   write4bits(0x03 << 4); 
   delayMicroseconds(150);
   
   // finally, set to 4-bit interface
   write4bits(0x02 << 4); 


	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();
	
	// clear it off
	clear();
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
	
	home();
  
}

/********** high level commands, for the user! */
void LiquidCrystal_I2C_rus::clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal_I2C_rus::home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal_I2C_rus::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal_I2C_rus::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C_rus::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal_I2C_rus::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C_rus::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal_I2C_rus::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C_rus::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal_I2C_rus::scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal_I2C_rus::scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal_I2C_rus::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal_I2C_rus::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal_I2C_rus::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal_I2C_rus::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal_I2C_rus::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

//createChar with PROGMEM input
void LiquidCrystal_I2C_rus::createChar(uint8_t location, const char *charmap) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
	    	write(pgm_read_byte_near(charmap++));
	}
}

// Turn the (optional) backlight off/on
void LiquidCrystal_I2C_rus::noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void LiquidCrystal_I2C_rus::backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}



/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal_I2C_rus::command(uint8_t value) {
	send(value, 0);
}

/*#if defined(ARDUINO) && ARDUINO >= 100
  size_t LiquidCrystal_I2C_rus::write(uint8_t value)
#else
  void   LiquidCrystal_I2C_rus::write(uint8_t value)
#endif
{
  uint8_t out_char=value;

  if (_dram_model == LCD_DRAM_WH1601) {  
    uint8_t ac=recv(LOW) & 0x7f;
    if (ac>7 && ac<0x14) command(LCD_SETDDRAMADDR | (0x40+ac-8));
  }

  if (value>=0x80) { // UTF-8 handling
    if (value >= 0xc0) {
      utf_hi_char = value - 0xd0;
    } else {
      value &= 0x3f;
      if (!utf_hi_char && (value == 1)) 
        send(0xa2,HIGH); // 
      else if ((utf_hi_char == 1) && (value == 0x11)) 
        send(0xb5,HIGH); // 
      else 
        send(pgm_read_byte_near(utf_recode + value + (utf_hi_char<<6) - 0x10), HIGH);
    }    
  } else send(out_char, HIGH);
#if defined(ARDUINO) && ARDUINO >= 100
  return 1; // assume sucess 
#endif
}     */



/************ low level data pushing commands **********/

void LiquidCrystal_I2C_rus::send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
       write4bits((highnib)|mode);
	write4bits((lownib)|mode); 
}



void LiquidCrystal_I2C_rus::write4bits(uint8_t value) {
	expanderWrite(value);
	pulseEnable(value);
}

void LiquidCrystal_I2C_rus::expanderWrite(uint8_t _data){                                        
	Wire.beginTransmission(_Addr);
	printIIC((int)(_data) | _backlightval);
	Wire.endTransmission();   
}

void LiquidCrystal_I2C_rus::pulseEnable(uint8_t _data){
	expanderWrite(_data | En);	// En high
	delayMicroseconds(1);		// enable pulse must be >450ns
	
	expanderWrite(_data & ~En);	// En low
	delayMicroseconds(50);		// commands need > 37us to settle
} 


// Alias functions

void LiquidCrystal_I2C_rus::cursor_on(){
	cursor();
}

void LiquidCrystal_I2C_rus::cursor_off(){
	noCursor();
}

void LiquidCrystal_I2C_rus::blink_on(){
	blink();
}

void LiquidCrystal_I2C_rus::blink_off(){
	noBlink();
}

void LiquidCrystal_I2C_rus::load_custom_character(uint8_t char_num, uint8_t *rows){
		createChar(char_num, rows);
}

void LiquidCrystal_I2C_rus::setBacklight(uint8_t new_val){
	if(new_val){
		backlight();		// turn backlight on
	}else{
		noBacklight();		// turn backlight off
	}
}

void LiquidCrystal_I2C_rus::printstr(const char c[]){
	//This function is not identical to the function used for "real" I2C displays
	//it's here so the user sketch doesn't have to be changed 
	print(c);
}


// unsupported API functions
void LiquidCrystal_I2C_rus::off(){}
void LiquidCrystal_I2C_rus::on(){}
void LiquidCrystal_I2C_rus::setDelay (int cmdDelay,int charDelay) {}
uint8_t LiquidCrystal_I2C_rus::status(){return 0;}
uint8_t LiquidCrystal_I2C_rus::keypad (){return 0;}
uint8_t LiquidCrystal_I2C_rus::init_bargraph(uint8_t graphtype){return 0;}
void LiquidCrystal_I2C_rus::draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
void LiquidCrystal_I2C_rus::draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
void LiquidCrystal_I2C_rus::setContrast(uint8_t new_val){}

	
