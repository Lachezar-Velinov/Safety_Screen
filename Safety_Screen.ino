#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>


// Initialize the LCD display
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  // Create LiquidCrystal instance

// Initialize the RFID reader
#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


unsigned int people_count = 0;


void setup() {

    analogReference(INTERNAL);
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    Serial.begin(9600);
    
    // Set up the LCD's number of columns and rows
    lcd.begin(16, 2);

    SPI.begin();      // Initiate  SPI bus
    
    // Initialize MFRC522
    mfrc522.PCD_Init();
    mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

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

    float total = 0; // reset total
    for(int i = 0; i < 64; i++) { // 64(max) analogue readings for averaging
      total += analogRead(A0); // add each value
    }
    float tempC = total * 0.001632; // Calibrate by changing the last digit(s)
    
    //double temp_raw = analogRead(A0);
    double air_quality_raw = analogRead(A1);
    //double mv = ( temp_raw/1024.0)*5000;
    //int cel = mv/10;
    // float farh = (cel*9)/5 + 32;
    int air = air_quality_raw;

    lcd.setCursor(0, 0);
    lcd.print("13:37");   // We don't have a clock module yet so just print an example time
    lcd.setCursor(0, 1);
    lcd.print(tempC, 1);
    lcd.setCursor(12, 1);
    lcd.print(air);
    lcd.print("    ");  // to clear the previous value
    lcd.setCursor(14, 0);
    lcd.print(people_count);
    

    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if (!mfrc522.PICC_IsNewCardPresent()) return;
  
    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) return;
    
    //Show UID on serial monitor
    Serial.print("UID tag :");
    String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
       Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
       Serial.print(mfrc522.uid.uidByte[i], HEX);
       content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
       content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    
    Serial.println();
    Serial.print("Message : ");
    content.toUpperCase();
    if (content.substring(1) == "37 16 60 C8" || content.substring(1) == "53 2F DB 1A") { //change here the UID of the card/cards that you want to give access
      Serial.println("Authorized access");
      people_count++;
      //delay(3000);
    }else {
      Serial.println(" Access denied");
      //delay(3000);
    }
    
    delay(500);                         // wait 200ms for next reading
}
