#include<SPI.h>
#include<Mirf.h>
#include<nRF24L01.h>
#include<MirfHardwareSpiDriver.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PAYLOAD 5
#define MAXPWR 2010
#define MINPWR 990
#define FILTER_N 10

typedef struct
{
  short THR, RUD, AIL, ELE, L;
} Payloads;

LiquidCrystal_I2C lcd(0x27, 16, 2);

short Filter(int pins)
{
  int filter_sum = 0;
  for (byte i = 0; i < FILTER_N; i++)
    filter_sum += analogRead(pins);
  return filter_sum / FILTER_N;
}

void display(short *final)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(final[0]);
  lcd.print("  R: ");
  lcd.print(final[1]);

  lcd.setCursor(0, 1);
  lcd.print("A: ");
  lcd.print(final[2]);
  lcd.print("  E: ");
  lcd.print(final[3]);
}

void Sends(Payloads *final)//Send Data Via 24L01
{
  static short msg[PAYLOAD], count = 0;
  msg[0] = map(final->THR, 1, 717, MINPWR, MAXPWR);
  msg[1] = map(final->RUD, 292, 1023, MINPWR, MAXPWR) - 48;
  msg[2] = map(final->AIL, 217, 990, MINPWR, MAXPWR) + 14;
  msg[3] = map(final->ELE, 144, 878, MINPWR, MAXPWR) + 23;
  msg[4] = final->L;
  for (byte i = 0; i < PAYLOAD - 1; i++)
  {
    if (msg[i] < MINPWR) msg[i] = MINPWR;
    else if (msg[i] > MAXPWR) msg[i] = MAXPWR;
  }
  Mirf.send((byte *)&msg);
  while (Mirf.isSending());
  count++;
  if (count >= 15)
  {
    display(msg);
    count = 0;
  }
}

void setup()
{
  Mirf.cePin = 8;//CE=D8
  Mirf.csnPin = 10;//CSN=D10
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();//Initialize
  Mirf.setTADDR((byte *)"RX01");
  Mirf.payload = PAYLOAD * sizeof(short); //Bytes At A Time
  Mirf.config();
  lcd.begin();
  lcd.backlight();
}

void loop()
{
  static Payloads final;
  final.THR = 1023 - Filter(A1); //Throttle
  final.RUD = Filter(A0); //Rudder
  final.AIL = Filter(A2); //Aileron
  final.ELE = Filter(A3); //Elevator
  final.L = digitalRead(2);
  Sends(&final);
}
