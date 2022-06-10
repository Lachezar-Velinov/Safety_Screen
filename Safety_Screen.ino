// Built-In libraries
#include <SPI.h>
#include <Wire.h>

// Additional libraries
#include <LiquidCrystal.h>
#include <MFRC522.h>
#include <Adafruit_BMP280.h>


/* ============================================================================================= */
/* Structures, Classes, Global variables, Function declarations */


// Initialize the LCD display
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Initialize the RFID readers
const int rst_pin = 9;
const int outside_pin = 10;
const int inside_pin = 8;
MFRC522 mfrc522_outside(outside_pin, rst_pin);
MFRC522 mfrc522_inside(inside_pin, rst_pin);

Adafruit_BMP280 bmp;


String cards[30];
unsigned int people_count = 0;


String getUid(MFRC522 &mfrc522_instance);


/* ============================================================================================= */


void setup() {

    Serial.begin(9600);
    pinMode(A0, INPUT);
    
    SPI.begin();      // Initiate  SPI bus

    // Set up the LCD's number of columns and rows
    lcd.begin(16, 2);
    
    // Initialize MFRC522
    mfrc522_outside.PCD_Init();
    mfrc522_inside.PCD_Init();
    mfrc522_outside.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
    mfrc522_inside.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

    // Initialize BMP280
    bmp.begin(0x76);
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode.
                    Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling
                    Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
                    Adafruit_BMP280::FILTER_X16,      // Filtering.
                    Adafruit_BMP280::STANDBY_MS_500); // Standby time.

    // Set up the LCD display layout
    // These are messages that only need to be printed once
    lcd.setCursor(0, 0);
    lcd.print("13:37");
    lcd.setCursor(6, 0);
    lcd.print("People: ");
    lcd.setCursor(4, 1);
    lcd.print("\xDF""C");   // Celsius symbol
    lcd.setCursor(7, 1);
    lcd.print("Air: ");

    for(int i = 0; i < 30; i++) {
        cards[i] = "";      // Empty spaces are marked with an empty string
    }
}

/* --------------------------------------------------------------------------------------------- */

void loop() {

    lcd.setCursor(0, 0);
    lcd.print("13:37");   // We don't have a clock module yet so just print an example time
    lcd.setCursor(0, 1);
    lcd.print(bmp.readTemperature(), 1);
    lcd.setCursor(12, 1);
    lcd.print(analogRead(A0));
    lcd.print("    ");  // to clear the previous value
    lcd.setCursor(14, 0);
    lcd.print(people_count);
    lcd.print("   ");    // to clear the previous value


    if (mfrc522_outside.PICC_IsNewCardPresent()) {
        // Found chip/card on the outside reader

        // Check how many cards have been read
        if(people_count == 30) {
            return;     // No more cards can be read
        }
     
        // Select the card/chip
        if (!mfrc522_outside.PICC_ReadCardSerial()) {
            return;
        }

        // Check if the Uid of the card/chip has already been read
        String Uid = getUid(mfrc522_outside);
        for(int i = 0; i < people_count; i++) {
            if(cards[i] == Uid) {
                return;     // The card/chip has already been read
            }
        }

        Serial.print("OUT: " + Uid + "\n");

        for(int i = 0; i < 30; i++) {
            if(cards[i] == "") {
                cards[i] = Uid;     // Put the new card in the first empty space
                break;
            }
        }
        people_count++;
        
    }else if(mfrc522_inside.PICC_IsNewCardPresent()) {
        // Found chip/card on the inside reader

        // Select the card/chip
        if (!mfrc522_inside.PICC_ReadCardSerial()) {
            return;
        }

        // Check if the Uid of the card/chip has been read
        String Uid = getUid(mfrc522_inside);
        for(int i = 0; i < 30; i++) {
            if(cards[i] == Uid) {
                Serial.print("IN: " + Uid + "\n");
                cards[i] = "";
                if(people_count >= 1) people_count--;
            }
        }
        
    }else {
        return;     // No card/chip found on either of the readers
    }
    
    delay(200);                         // wait 200ms for next reading
}


/* ============================================================================================= */
/* Function definitions */


String getUid(MFRC522 &mfrc522_instance) {
    String Uid = "";
    for(int i = 0; i < mfrc522_instance.uid.size; i++) {
        Uid += String(mfrc522_instance.uid.uidByte[i], HEX);
    }
    return Uid;
}


/* ============================================================================================= */
