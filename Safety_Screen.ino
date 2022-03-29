#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

void loop() {
    double temp_raw = analogRead(A0);
    double air_quality_raw = analogRead(A1);
    double mv = ( temp_raw/1024.0)*5000;
    double cel = mv/10;
    // float farh = (cel*9)/5 + 32;
    
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.setCursor(8, 0);
    lcd.print(cel);

    lcd.setCursor(0, 1);
    lcd.print("AirQ: ");
    lcd.setCursor(8, 1);
    lcd.print(air_quality_raw);
    
    delay(500);                         // wait 100ms for next reading
}
