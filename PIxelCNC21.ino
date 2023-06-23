// 4 Arduino NANO
#include "avdweb_AnalogReadFast.h" // 5xFaster analog read from https://github.com/avandalen/avdweb_AnalogReadFast 
#include "LedControl.h" 
// pin 11 is connected to the DataIn (Z+- on CNC shield)
// pin 10 is connected to the CLK (Y+- on CNC shield)
// pin 9 is connected to LOAD (X+- on CNC shield)
LedControl lc=LedControl(11,10,9,1);

// Stepper Motor XY on CNS shield v4 
  const int XstepPin = 5; //X.STEP
  const int XdirPin = 2; // X.DIR
  const int YstepPin = 6; //Y.STEP
  const int YdirPin = 3; // Y.DIR
int SMSpeed = 400; // Stepper Motor Speed 


byte matrix [8][8][15];
int aread[15]; //serial analog read data
int calibrate[15]; // calibration parameter
int pulseLenght = 100;
int delta;
byte i = 0;
byte n = 0;
byte m = 0;
byte x;
byte y;
byte z;
int rx;
int by;
boolean par=false; // rows parity flag - makes direction of Y axis (dy)


byte sensorPin = 0; //Abort on CNC shield
byte buttonPin = 13;
byte gatePin = 12;

void pulse() // sent pulse to coil and read responce
{
    digitalWrite(gatePin,HIGH); 
    delayMicroseconds(pulseLenght); 
    digitalWrite(gatePin,LOW);
    delayMicroseconds(20);
    for (n = 0; n <15; n++) { aread[n]=analogReadFast(sensorPin); }
   // delay(3);  
}

void rodX()
{
  digitalWrite(XdirPin, LOW);
  for (rx=0; rx<400; rx++) //make 2 turn of rod (50mm step with 8mm diameter)
  {
  digitalWrite(XstepPin, HIGH);
  delayMicroseconds(SMSpeed);
  digitalWrite(XstepPin, LOW);
  delayMicroseconds(SMSpeed);
  }
}

void beltY()
{
  digitalWrite(YdirPin, LOW);
  for (by=0; by<250; by++) // move coil to 50mm with 20-teeth gear 4 GT2 belt
  {
  digitalWrite(YstepPin, HIGH);
  delayMicroseconds(SMSpeed);
  digitalWrite(YstepPin, LOW);
  delayMicroseconds(SMSpeed);
  }
}

void homeY()
{
  digitalWrite(YdirPin, HIGH);
  for (by=0; by<2000; by++) // move coil to 50mm with 20-teeth gear 4 GT2 belt
  {
  digitalWrite(YstepPin, HIGH);
  delayMicroseconds(SMSpeed);
  digitalWrite(YstepPin, LOW);
  delayMicroseconds(SMSpeed);
  }
}

void setup()

{
  //The MAX72XX is in power-saving mode on startup, we have to do a wakeup call
  lc.shutdown(0,false);
  // Set the brightness to a medium values 
  lc.setIntensity(0,8);
  // and clear the display 
  lc.clearDisplay(0);
  
Serial.begin(9600);

pinMode(gatePin, OUTPUT); // MOFET gate pin
pinMode(buttonPin, INPUT_PULLUP);
 pinMode(XstepPin,OUTPUT); 
 pinMode(XdirPin,OUTPUT);
 pinMode(YstepPin,OUTPUT); 
 pinMode(YdirPin,OUTPUT);

 
for (i = 0; i <200; i++) {pulse();} // empty cycles for system heating
 

  for (i = 0; i <32; i++) // calibrate on avererage from 32 pulses
  {
  pulse();
  for (m=0; m<15; m++) {calibrate[m]=calibrate[m]+aread[m];}
  }
  for (i = 0; i <15; i++) {calibrate[i]=calibrate[i]/32;}

   
//  while ( digitalRead(buttonPin)==HIGH ){delay(1);}  //wait 4 button pressed 2 scan

  }

void loop()
{
for (x=0; x<8; x++ )
{
  for (y=0; y<8; y++)
  {
   lc.setLed(0,x,y,true);
    pulse();
   for(z=0; z<15 ; z++)
   {
    if (calibrate[z]-aread[z] > 75 ) 
    {
     matrix[x][y][z]=true; // set led in row to on if metal responce present *********************************
    }
    else {matrix[x][y][z]=false;}
   }
   lc.setLed(0,x,y,false);
   beltY(); 
  }
  rodX();
  homeY();
  delay(100);
}

par=true;
while(par==true)
  {
    
   for (z=0; z<15; z++) // display one of layers
   {
    for (x=0; x<8; x++)
    {
      for (y=0; y<8; y++)
      {
        if (matrix[x][y][z]==true) {lc.setLed(0,7-x,y,true);}
      }
    }
    //while ( digitalRead(buttonPin)==HIGH ){delay(1);}  //wait 4 button pressed 2 next layer
    delay(1000);
   }
     
  }

}
  
