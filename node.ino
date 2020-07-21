#include "NRF24L01.h"

#define TX_ADR_WIDTH    5   // 5 unsigned chars TX(RX) address width
#define TX_PLOAD_WIDTH  32   // 3 unsigned chars TX payload
unsigned char TX_ADDRESS[TX_ADR_WIDTH]  = 
{
    0x34,0x43,0x10,0x10,0x01
}; 

unsigned char rx_buf[TX_PLOAD_WIDTH] = {0}; // initialize value
unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};

//Ultrasonic Sensor
long pulse; // duration of the received pulse in ms
int  distance; // the distance of the object
const int trigger = 6 ; // pin for the trigger
const int echo = 7; // pin for the echo return
// Button and Mode
int mode = 0;
const int button = 2;




void setup()
{
    Serial.begin(115200);

    // Set pins to correct mode for ultrasonic sensor to work
    pinMode(trigger,OUTPUT);
    pinMode(echo,INPUT);
    // Set pins for Button
    pinMode(button,INPUT_PULLUP);
        
    
    
    
    

    NRF_Init();                        
    Serial.println("TX_Mode Start");    
}

void loop()
{
    switch(mode)
    {
      case 0: // All Relays OFF
       tx_buf[14] = 0x03;
       tx_buf[16] = 0x00;
       tx_buf[17] = 0x00;
       break;
      case 1: // Relay One On, Two Off
       tx_buf[14] = 0x02;
       tx_buf[16] = 0x00;
       tx_buf[17] = 0x00;
       break;
      case 2:
       tx_buf[14] = 0x01; // Relay Two On, One Off
       tx_buf[16] = 0x00;
       tx_buf[17] = 0x00;
       break;
      case 3:
       tx_buf[14] = 0x00; // Both Relays On
       tx_buf[16] = 0x00;
       tx_buf[17] = 0x00;
       break;
      case 4:
       tx_buf[14] = 0x0b; //motor1 on slow
       tx_buf[16] = 0x96;
       break;
      case 5:
       tx_buf[14] = 0x07; //motor2 on slow
       tx_buf[17] = 0x96;
       break; 
      case 6:
       tx_buf[14] = 0x0f; //both motors on slow
       tx_buf[16] = 0x96;
       tx_buf[17] = 0x96;
       break; 
      case 7:
       tx_buf[14] = 0x0f; //both motors on medium
       tx_buf[16] = 0xDC;
       tx_buf[17] = 0xDC;
       break; 
      case 8:
       tx_buf[14] = 0x0f; //both motors on fast
       tx_buf[16] = 0xff;
       tx_buf[17] = 0xff;
       break; 
      case 9:
       tx_buf[14] = 0x0f; //both motors on 1 slow, 2 fast
       tx_buf[16] = 0x96;
       tx_buf[17] = 0xff;
      break;
      case 10:
       tx_buf[14] = 0x0f; //both motors on 1 fast 2 slow
       tx_buf[16] = 0xff;
       tx_buf[17] = 0x96;
      break;
      case 11:
      break;
        
    }
    tx_buf[15] = mode;
    if(digitalRead(button)==LOW)
    {
      while(digitalRead(button)==LOW);
      mode++;
      if (mode==11)
      mode=0;
    }
    
    NRF_SeTxMode();
   
    get_distance();
     /*
    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.print(" cm. Pulse Duration : ");
    Serial.print(pulse);
    Serial.println(" ms ");
    */
    do
    { 
        NRF_Send(tx_buf);
    }
    while(!NRF_CheckAck());
    
    delay(100);
    
}

void get_distance()
{
   digitalWrite(trigger,LOW);
   delayMicroseconds(2);
   digitalWrite(trigger,HIGH);
   delayMicroseconds(10);
   digitalWrite(trigger,LOW);
   pulse = pulseIn(echo,HIGH);
   distance = pulse/29/2;
   tx_buf[10] = highByte(distance);
   tx_buf[11] = lowByte(distance);
   tx_buf[12] = highByte(pulse);
   tx_buf[13] = lowByte(pulse);
   delay(25);
}
