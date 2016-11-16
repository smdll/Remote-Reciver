#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "Servo_mod.h"

#define PAYLOAD 4

Servo powerServo[PAYLOAD];
void setup()
{
  Serial.begin(9600);//Debug Mode
  Mirf.cePin = 8;//CE=D8
  Mirf.csnPin = 7;//CSN=D7
  Mirf.spi = &MirfHardwareSpi;
  Mirf.channel = 24;//Channel
  Mirf.init();//Initialize
  Mirf.setRADDR((byte *)"RX01");
  Mirf.payload = PAYLOAD * sizeof(short); //Bytes at a time
  Mirf.config();
  powerServo[0].attach(2);//THR
  powerServo[1].attach(5);//RUD
  powerServo[2].attach(3);//AIL
  powerServo[3].attach(4);//ELE
}

void loop()
{
  short Rdata[PAYLOAD];
  while (1)
  {
    if (!Mirf.isSending() && Mirf.dataReady()) //Get Data Via 24L01
    {
      Mirf.getData((byte *)&Rdata);
      for (unsigned short i = 0; i < PAYLOAD; i++)
      {
        powerServo[i].writeMicroseconds(Rdata[i]);//Drive ESCs
      }
    }
  }
}
