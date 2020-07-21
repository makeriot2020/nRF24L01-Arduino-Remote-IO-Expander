/*********************************************************************
**   SPI-compatible                                                 **
**   CE - to digital pin 9                                          **
**   CSN - to digital pin 10  (SS pin)                               **
**   SCK - to digital pin 13 (SCK pin)                              **
**   MOSI - to digital pin 11 (MOSI pin)                            **
**   MISO - to digital pin 12 (MISO pin)                            **
**   IRQ - to digital pin 8 (MISO pin)                             **
*********************************************************************/
#include "NRF24L01.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);

#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  32  // 32 unsigned chars TX payload
unsigned char RX_ADDRESS[TX_ADR_WIDTH]  = 
{
  0x34,0x43,0x10,0x10,0x01
};



unsigned char rx_buf[TX_PLOAD_WIDTH] = {0};
unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};

//Ultrasonic
long pulse;
long distance;

// Relays
const int relay1 = 7;
const int relay2 = 4;
const int motor1 = 6;
const int motor2 = 5;
int relay1_status = HIGH;
int relay2_status = HIGH;
int motor1_status = LOW;
int motor2_status = LOW;

void setup()
{
    Serial.begin(115200);
    lcd.begin(16,2);
    lcd.backlight();
    lcd.setCursor(0,0);
    // Pinmode for Relays
    // Our Relays are ACTIVE LOW, so Write Pin High to prevent Switching
    // on on startup / reset
    digitalWrite(relay1,HIGH);
    digitalWrite(relay2,HIGH);
    pinMode(relay1,OUTPUT);
    pinMode(relay2,OUTPUT);
    digitalWrite(motor1,LOW);
    digitalWrite(motor2,LOW);
    pinMode(motor1,OUTPUT);
    pinMode(motor2,OUTPUT);
    
    NRF_Init();                        // Initialize IO     
    NRF_SetRxMode();
    Serial.println("RX_Mode start...");
}

void loop()
{
    NRF_SetRxMode();
    lcd.clear();
    if(NRF_Receive(rx_buf))
    {
        
        Serial.print("RX = ");
        for(int i = 0; i < 10; i++)
        {
            Serial.write((char)rx_buf[i]);
            
        }
        Serial.print("[0x");
        Serial.print(rx_buf[10],HEX);
        Serial.print(rx_buf[11],HEX);
        distance = word(rx_buf[10],rx_buf[11]);
        Serial.print("] ");
        Serial.print(distance);
        Serial.print(" cm [0x");
        lcd.setCursor(0,0);
        lcd.print(distance);
        lcd.print(" cm");
        Serial.print(rx_buf[12],HEX);
        Serial.print(rx_buf[13],HEX);
        pulse = word(rx_buf[12],rx_buf[13]);
        Serial.print("] ");
        Serial.print(pulse);
        Serial.print(" ms [0b");
        Serial.print(rx_buf[14],BIN);
        Serial.print("] Relay 1=");
        if (bitRead(rx_buf[14],1) == HIGH)
        {
          Serial.print(" OFF");
          digitalWrite(relay1,HIGH);
          lcd.setCursor(12,0);
          lcd.print("   ");
          
        } else {
          Serial.print(" ON");
          digitalWrite(relay1,LOW);
          lcd.setCursor(12,0);
          lcd.print("RL2");
        }
        Serial.print(" Relay 2=");
        if (bitRead(rx_buf[14],0) == HIGH)
        {
          Serial.print(" OFF");
          digitalWrite(relay2,HIGH);
          lcd.setCursor(8,0);
          lcd.print("   ");
        } else {
          Serial.print(" ON");
          digitalWrite(relay2,LOW);
          lcd.setCursor(8,0);
          lcd.print("RL1");
        }
        Serial.print(" Mode: ");
        Serial.print(rx_buf[15],DEC);
        Serial.print(" M1= ");
        if(bitRead(rx_buf[14],3) == HIGH)
        {
          motor1_status = HIGH;
          Serial.print("ON");
        } else {
          motor1_status = LOW;
          Serial.print("OFF");
        }

        if (motor1_status == HIGH) {
          analogWrite(motor1,rx_buf[16]);
          Serial.print(" Speed ");
          Serial.print(rx_buf[16],DEC);
          lcd.setCursor(0,1);
          lcd.print("M1 ");
          lcd.print(rx_buf[16]);
        } else {
          analogWrite(motor1,0);
          lcd.setCursor(0,1);
          lcd.print("M1 OFF");
        }
        
        Serial.print(" M2= ");
        if(bitRead(rx_buf[14],2) == HIGH)
        { 
          motor2_status = HIGH;
          Serial.print("ON");
        } else {
          motor2_status = LOW;
          Serial.print("OFF");
        }
        if (motor2_status == HIGH) {
          analogWrite(motor2,rx_buf[17]);
          Serial.print(" Speed ");
          Serial.print(rx_buf[17],DEC);
          lcd.setCursor(8,1);
          lcd.print("M2 ");
          lcd.print(rx_buf[17]);
        } else {
          analogWrite(motor2,0);
          lcd.setCursor(8,1);
          lcd.print("M2 OFF");
        }
        
        // clear buffer
        Serial.println();
        for(int i = 0; i < 32; i++)
            rx_buf[i] = 0;
    }
    delay(250);
}
