#include <DS3231.h>
#include <Bounce2.h>

//#define DEBUG

int vklop = 10;
int stikalopin = 6;
int motorA = 8;
int motorB = 7;

bool gibanje = false;
bool smer = false; //false - smer A, odpiranje; true - smer B, zapiranje

DS3231 rtc(SDA, SCL);
Time t;

unsigned long cas;
unsigned long ekspozicija;
//unsigned long ekspozicija1 = 10;
unsigned long ekspozicija2 = 10;
unsigned long ekspozicija3 = 30;

Bounce stikalo = Bounce();

unsigned long perioda = 50;
float dcA = 0.2; // duty cicle za smer A
float dcB = 0.15; // duty cicle za smer B

void setup() {
  Serial.begin(9600); //potrebno, četudi neuporabljen
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);

  pinMode(vklop, INPUT_PULLUP);
  pinMode(stikalopin, INPUT_PULLUP);
  stikalo.attach(stikalopin);
  stikalo.interval(50);

  rtc.begin();
  
  #ifdef DEBUG
    Serial.println("začetek");
  #endif
}

void loop() {
  if (digitalRead(vklop) == LOW)  {//on off stikalo
    t = rtc.getTime();
    #ifdef DEBUG
      Serial.println(rtc.getTimeStr());
    #endif

    if (t.sec == 0 && smer==true && t.hour > 8 && t.hour < 15) {
      if (t.min == 0) { // polna ura
        ekspozicija = ekspozicija2;
        gibanje = true;
        smer = false;
        if (t.hour == 12) { //poldan
          ekspozicija = ekspozicija3;
          gibanje = true;
          smer = false;
        }
      }
//      else if (t.min == 30) { // pol ure
//        ekspozicija = ekspozicija1;
//        gibanje = true;
//        smer = false;
//      }
    }
    stikalo.update();

    if (stikalo.fell()) { // zaklop na koncnem polozaju A ali B
      gibanje = false;
      cas = rtc.getUnixTime(t);
      #ifdef DEBUG
          Serial.println("stikalo");
      #endif DEBUG
    }

    if (!gibanje && smer == false && ((rtc.getUnixTime(t) - cas) >= ekspozicija)) { // zaslonka odprta za ekspozicijski cas
      gibanje = true;
      smer = true; // zapiranje
      #ifdef DEBUG
          Serial.println("gibanje");
      #endif
    }


    if (gibanje) {
      if (smer == false) {
        digitalWrite(motorA, HIGH);
        delay(dcA * perioda);
        digitalWrite(motorA, LOW);
        delay((1 - dcA)*perioda);
      }
      else {
        digitalWrite(motorB, HIGH);
        delay(dcB * perioda);
        digitalWrite(motorB, LOW);
        delay((1 - dcB)*perioda);
      }
      if (perioda < 50) {
        delay(50 - perioda);
      }
    }
    else {
      delay(50);
    }
  }
}
