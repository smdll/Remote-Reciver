#include<SPI.h>
#include<Mirf.h>
#include<nRF24L01.h>
#include<MirfHardwareSpiDriver.h>

#define PAYLOAD 4
#define MAXPWR 2000
#define MINPWR 990

#define THRmid 505
#define RUDmid 508
#define AILmid 506
#define ELEmid 505

typedef struct
{
  short THR, RUD, AIL, ELE;
} Motor;

void Sends(Motor *final)//Send Data Via 24L01
{
  short msg[PAYLOAD];
  msg[0] = map(final->THR, 0, 1023, MINPWR, MAXPWR);
  msg[1] = map(final->RUD, 0, 1023, MINPWR, MAXPWR);
  msg[2] = map(final->AIL, 0, 1023, MINPWR, MAXPWR);
  msg[3] = map(final->ELE, 0, 1023, MINPWR, MAXPWR);
  Mirf.send((byte *)&msg);
  while (Mirf.isSending());
}

void setup()
{
  Mirf.cePin = 8;//CE=D8
  Mirf.csnPin = 10;//CSN=D10
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();//Initialize
  Mirf.channel = 24;//Channel
  Mirf.setTADDR((byte *)"RX01");
  Mirf.payload = PAYLOAD * sizeof(short); //Bytes At A Time
  Mirf.config();
}

void loop()
{
  Motor final;
  final.THR = analogRead(A0) + (512 - THRmid); //Throttle
  final.RUD = analogRead(A1) + (512 - RUDmid); //Rudder
  final.AIL = analogRead(A2) + (512 - AILmid); //Aileron
  final.ELE = analogRead(A3) + (512 - ELEmid); //Elevator
  Sends(&final);
}
