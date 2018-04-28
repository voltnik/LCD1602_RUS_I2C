#include <Wire.h> 
#include <LiquidCrystal_I2C_rus.h>

LiquidCrystal_I2C_rus lcd(0x3F,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  Serial.begin(9600);
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("АБВГДЕЁЖЗИЙКЛМНО");
  lcd.setCursor(0,1);
  lcd.print("ПРСТУФХЦЧШЩЪЫЬЭЮЯ");
  lcd.setCursor(0,2);
  lcd.print("абвгдеёжзийклмно");
  lcd.setCursor(0,3);
  lcd.print("прстуфхцчшщъыьэюя");
  delay(5000);
}


void loop()
{
}

