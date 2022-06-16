
#include <Wire.h>
#include "TimerOne.h"
#include <TimerThree.h>
#include <TimerFour.h>
//INPUTS
int CLOSEDLimit = 22;
int OPENEDLimit = 24;
int GarageEnd = 26;
int GarageStart = 28;
//INPUTS
//OUTPUTS
int InLight = 44;
int OutLight = 46;
int Garage = 13;
int SPSstop = 50;
int SPSrun = 52;
//OUTPUTS
int one = 0;
int two = 0;
int four = 0;
int t[7] = {};
int currentLightTime;
int currentLightTimeout;
int currentInLight;
int currentOutLight;
int currentSPS;
int currentSPSLight;
int currentAutoClose;

bool dateAvailable = false;
bool closeRequest = false;
bool error = false;

void setup() {
  Serial.begin(115200);
  pinMode(Garage, OUTPUT);
  digitalWrite(Garage, LOW);
  pinMode(CLOSEDLimit, INPUT_PULLUP);
  pinMode(OPENEDLimit, INPUT_PULLUP);
  pinMode(GarageEnd, INPUT_PULLUP);
  pinMode(GarageStart, INPUT_PULLUP);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);

}

void loop() {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////                                         it handles the door AUTO                                     /////
  /////                                                 BEGIN                                                /////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (dateAvailable == true) {
    one = 0;
    two = 0;
    four = 0;
    TCCR1B &= ~(1 << CS10);
    TCCR1B &= ~(1 << CS12);
    TCCR3B &= ~(1 << CS10);
    TCCR3B &= ~(1 << CS12);
    TCCR4B &= ~(1 << CS10);
    TCCR4B &= ~(1 << CS12);
    digitalWrite(InLight, LOW);
    digitalWrite(OutLight, LOW);
    digitalWrite(SPSrun, LOW);
    digitalWrite(SPSstop, LOW);
    int previousGarageEnd = digitalRead(GarageEnd);

    if (currentInLight == 1) {
      digitalWrite(InLight, HIGH);
      Timer1.initialize(1000000);
      Timer1.attachInterrupt(timer1);
      Serial.println("Benti be");
    }

    if (currentOutLight == 1) {
      digitalWrite(OutLight, HIGH);
      Timer3.initialize(1000000);
      Timer3.attachInterrupt(timer3);
      Serial.println("Kinti be");
    }

    if (digitalRead(CLOSEDLimit) == 1 && digitalRead(OPENEDLimit) == 0) {
      openGarage();
      delay(5000);
    }

    ////////////////////////////////////////////ERROR//////////////////////////////////////////////////
    if (digitalRead(CLOSEDLimit) == 1) {
      digitalWrite(SPSstop, HIGH);
      delay(1000);
      digitalWrite(InLight, LOW);
      digitalWrite(OutLight, LOW);
      digitalWrite(SPSrun, LOW);
      digitalWrite(SPSstop, LOW);
    }
    ////////////////////////////////////////////ERROR//////////////////////////////////////////////////
    while (digitalRead(CLOSEDLimit) == 0 && digitalRead(OPENEDLimit) == 0) {
      Serial.println("Várok míg kinyilik");
      delay(1000);
    }

    if (digitalRead(OPENEDLimit) == 1 && digitalRead(CLOSEDLimit) == 0) {
      Serial.println("kinyilt");
      if (currentInLight == 2) {
        digitalWrite(InLight, HIGH);
        Timer1.initialize(1000000);
        Timer1.attachInterrupt(timer1);
        Serial.println("Benti be");
      }

      if (currentOutLight == 2) {
        digitalWrite(OutLight, HIGH);
        Timer3.initialize(1000000);
        Timer3.attachInterrupt(timer3);
        Serial.println("kinti be");
      }
      if (currentSPSLight == 2) {
        if (previousGarageEnd == 1) {

          while (digitalRead(GarageEnd) == 1) {
            digitalWrite(SPSrun, HIGH);
            Serial.println("ZOLD");
          }

          if (digitalRead(GarageEnd) == 0) {
            digitalWrite(SPSrun, LOW);
            digitalWrite(SPSstop, HIGH);
            Serial.println("PIROS");
            delay(3000);
            digitalWrite(SPSstop, LOW);
            Serial.println("Jelzolampak ki");
          }

        }
        if (previousGarageEnd == 0) {
          while (digitalRead(GarageStart) == 1) {
            delay(100);
          }
          if (digitalRead(GarageStart) == 0) {
            //jó utat
          }
        }
      }
      if (currentInLight == 3 && digitalRead(GarageEnd) == 0 ) {
        digitalWrite(InLight, HIGH);
        Timer1.initialize(1000000);
        Timer1.attachInterrupt(timer1);
        Serial.println("Bealltal, benti be");
      }

      if (currentOutLight == 3 && digitalRead(GarageEnd) == 0 ) {
        digitalWrite(OutLight, HIGH);
        Timer3.initialize(1000000);
        Timer3.attachInterrupt(timer3);
        Serial.println("Bealltal, kinti be");
      }

      while (previousGarageEnd == digitalRead(GarageEnd)) {
        delay(3000);
        Serial.println("varok meg be/kiallsz");
      }

      if (digitalRead(GarageStart) == 0 && digitalRead(GarageEnd) == 0 ) {    //a kocsi túl hosszú
        Serial.println("Tul hosszu auto");
        error = true;
        digitalWrite(SPSstop, HIGH);
        delay(3000);
        digitalWrite(SPSstop, LOW);
      }
      if(error == false){
      while (digitalRead(GarageStart) == 0) {
        delay(1000);
        Serial.println("varok meg be/kiallsz");
      }
      
      if (digitalRead(GarageStart) == 1) {
        Timer4.initialize(1000000);
        Timer4.attachInterrupt(timer4);
        Serial.println("Lecsukom mindjart");
      }
      }
    }
    dateAvailable = false;
    error = false;
  }

  if (closeRequest == true) {
    while (digitalRead(GarageStart) == 0) {                 //VIGYAZZON
      Serial.println("Az On biztonsaga a legfontosabb");
      delay(100);
    }
    if (digitalRead(GarageStart) == 1) {
      openGarage();
    }

    while (digitalRead(CLOSEDLimit) == 0 && digitalRead(OPENEDLimit) == 0) {
      if (digitalRead(GarageStart) == 0) {
        openGarage();
        Timer4.initialize(1000000);
        Timer4.attachInterrupt(timer4);
        Serial.println("Ujraprobalas");
        //Újrapróbálás currentAutoClose idő múlva
      }
    }
    closeRequest = false;
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /////                                         it handles the door AUTO                                     /////
  /////                                                   END                                                /////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

void receiveEvent(int howMany) {

  int i = 0;
  while (Wire.available()) {
    t[i] = Wire.read();
    i = i + 1;
  }
  currentAutoClose = t[0];
  currentLightTime = t[1];
  currentLightTimeout = t[2];
  currentInLight = t[3];
  currentOutLight = t[4];
  currentSPS = t[5];
  currentSPSLight = t[6];
  dateAvailable = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void openGarage() {
  digitalWrite(Garage, HIGH);
  delay(100);
  digitalWrite(Garage, LOW);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//InLight timer
void timer1()
{
  if (one == currentLightTime * 60) {
    digitalWrite(InLight, LOW);
    Serial.println("benti le");
    one = 0;
    TCCR1B &= ~(1 << CS10);
    TCCR1B &= ~(1 << CS12);
  }
  one++;
}

//OutLight timer
void timer3()
{
  if (two == currentLightTimeout * 60) {
    digitalWrite(OutLight, LOW);
    Serial.println("kinti le");
    two = 0;
    TCCR3B &= ~(1 << CS10);
    TCCR3B &= ~(1 << CS12);
  }
  two++;
}

//Garage timer
void timer4()
{
  Serial.println("Elindultam");
  if (four == currentAutoClose) {
    closeRequest = true;
    four = 0;
    TCCR4B &= ~(1 << CS10);
    TCCR4B &= ~(1 << CS12);
  }
  four++;
}


