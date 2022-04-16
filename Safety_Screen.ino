#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>


// Initialize the LCD display
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  // Create LiquidCrystal instance

// Initialize the RFID reader
#define RST_PIN 9
#define OUTSIDE_PIN 10
#define INSIDE_PIN 8

// Create MFRC522 instances
MFRC522 mfrc522_outside(OUTSIDE_PIN, RST_PIN);
MFRC522 mfrc522_inside(INSIDE_PIN, RST_PIN);

// Create Adafruit_BMP280 instance
Adafruit_BMP280 bmp;


unsigned int people_count = 0;


void setup() {

    lcd.setCursor(0, 0);
    lcd.print("13:37");
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    Serial.begin(9600);
    
    // Set up the LCD's number of columns and rows
    lcd.begin(16, 2);

    SPI.begin();      // Initiate  SPI bus
    
    // Initialize MFRC522
    mfrc522_outside.PCD_Init();
    mfrc522_inside.PCD_Init();
    mfrc522_outside.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    mfrc522_inside.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

    // Initialize BMP280
    bmp.begin(0x76);
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    // Set up the LCD display layout
    // These are messages that only need to be printed once
    lcd.setCursor(6, 0);
    lcd.print("People: ");
    lcd.setCursor(4, 1);
    lcd.print("\xDF""C");
    lcd.setCursor(7, 1);
    lcd.print("Air: ");
}


void loop() {

    //double temp_raw = analogRead(A0);
    double air_quality_raw = analogRead(A1);
    int air = air_quality_raw;

    lcd.setCursor(0, 0);
    lcd.print("13:37");   // We don't have a clock module yet so just print an example time
    lcd.setCursor(0, 1);
    lcd.print(bmp.readTemperature(), 1);
    lcd.setCursor(12, 1);
    lcd.print(air);
    lcd.print("    ");  // to clear the previous value
    lcd.setCursor(14, 0);
    lcd.print(people_count);
    lcd.print("   ");    // to clear the previous value
    

    if (mfrc522_outside.PICC_IsNewCardPresent()) {
        // Found chip/card on the outside reader

        // Select one of the cards
        if (!mfrc522_outside.PICC_ReadCardSerial()) return;

        people_count++;
        
    }else if(mfrc522_inside.PICC_IsNewCardPresent()) {
        // Found chip/card on the outside reader

        // Select one of the cards
        if (!mfrc522_inside.PICC_ReadCardSerial()) return;

        if(people_count >= 1) people_count--;
        
    }else {
        // No chip/card found on either of the readers
        return;
    }
    
    delay(500);                         // wait 200ms for next reading
}
