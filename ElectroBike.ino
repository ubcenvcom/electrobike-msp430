#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define BACKLIGHT_PIN     3

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7);  // Set the LCD I2C address

unsigned int sv1=0;
unsigned int sv2=0;
double rpm=0;
unsigned int volt=12;
volatile unsigned long rev=0;
unsigned int mVperAmp = 100;
unsigned int ACSoffset = 2500;
double Voltage = 0;
double Amps = 0;
double Wats = 0;

unsigned long timeold;
double VoltRange=5000;

void rpmTick()
{
  rev++;
}

void setup()
{
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(BACKLIGHT_ON);
  
  lcd.begin(16,2);
  lcd.home ();
  lcd.print("Turku Ekotuki");
  lcd.setCursor (0, 1);
  lcd.print ("SP-20150830-001");
  delay(1000);
  lcd.clear();
  // Amps
  lcd.setCursor(5, 0);
  lcd.print("A");
  
  // Volts
  lcd.setCursor(5, 1);
  lcd.print("V");

  // Wats
  lcd.setCursor(5, 1);
  lcd.print("W");

  lcd.setCursor(13, 0);
  lcd.print("RPM");
  
  timeold = millis();
  pinMode(P1_4, INPUT);
  
  pinMode(P1_3, INPUT_PULLUP); 
  attachInterrupt(P1_3, rpmTick, FALLING);
}

void readVoltage()
{
  sv2 = analogRead(P1_5);
  volt=(sv1/1023.0)*3.3; // Adjust for real input
}

void readAmps()
{
  sv1 = analogRead(P1_4);
  Voltage = (sv1 / 1023.0) * VoltRange; // Gets you mV
  Amps = ((Voltage - ACSoffset) / mVperAmp);
  // Don't bother with negative amps as we should not get that
  if (Amps<0)
    Amps=0;
}

void loop()
{
  unsigned long t;
  unsigned long c;
  unsigned long dif;
  
  lcd.setCursor (0, 0);
  if (Amps<2)
    lcd.print(Amps, 3);
  else
    lcd.print(Amps, 2);
    
  Wats=Amps*volt;

  lcd.setCursor (0, 1);
  if (Wats<10)
    lcd.print(Wats, 3);
  else if (Wats<100)
    lcd.print(Wats, 2);
  else
    lcd.print(Wats, 1);
    
  readAmps();
  noInterrupts();
  c=rev;
  interrupts();
  t=millis();
  dif=t-timeold;
  if (c>5 || (dif>5000 && c>1)) {            
    rpm=60.0/((dif/c)/1000.0);
    timeold=t;
    rev=0;
  }

  lcd.setCursor(8, 0);
  lcd.print(rpm, 1);
  if (rpm<100)
    lcd.print(" ");
  if (rpm<10)
    lcd.print(" ");
  
  delay(1000);
}
