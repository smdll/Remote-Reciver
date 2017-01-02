#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "Servo_mod.h"

#define PAYLOAD 5

Servo powerServo[PAYLOAD];

void setup()
{
  Serial.begin(115200);//Debug Mode
  Mirf.cePin = 8;//CE=D8
  Mirf.csnPin = 7;//CSN=D7
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();//Initialize
  Mirf.setRADDR((byte *)"RX01");
  Mirf.payload = PAYLOAD * sizeof(short); //Bytes at a time
  Mirf.config();
  powerServo[0].attach(2);//THR
  powerServo[1].attach(5);//RUD
  powerServo[2].attach(3);//AIL
  powerServo[3].attach(4);//ELE
  pinMode(10, OUTPUT);
}

void loop()
{
  short Rdata[PAYLOAD];
  long ti;
  byte i;
  while (1)
  {
    if (!Mirf.isSending() && Mirf.dataReady()) //Get Data Via 24L01
    {
      Mirf.getData((byte *)&Rdata);
      for (i = 0; i < PAYLOAD; i++)
      {
        powerServo[i].writeMicroseconds(Rdata[i]);//Drive ESCs
        Serial.print(Rdata[i]);
        Serial.print(" | ");
      }
      digitalWrite(10, Rdata[4]);
      Serial.println();
      ti = millis();
    }
    if(millis() - ti > 2000)
    {
      for(i = 0; i < PAYLOAD; i++)
        powerServo[i].writeMicroseconds(1000);
      while(1);
    }
  }
}
