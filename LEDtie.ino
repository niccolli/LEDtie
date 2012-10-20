#include <SoftwareSerial.h>

#define NUMBER_OF_LED 8

#define STATE_ONE_LED          'O'
#define STATE_BACK_AND_FORTH   'B'
#define STATE_DIRECT           'D'

int LEDs[] = { 9, 8, 7, 6, 5, 4, 3, 2 };
int debugLED = 13;

SoftwareSerial xbee(10, 11);

void setup(){
  pinMode(debugLED, OUTPUT);
  
  for(int i=0; i<NUMBER_OF_LED; i++){
    pinMode(LEDs[i], OUTPUT);
    digitalWrite(LEDs[i], LOW);
  }
  
  xbee.begin(9600);  
  Serial.begin(9600);
}

byte count = 1;
int sleep = 500; // ms
byte state = STATE_ONE_LED;
//byte state = STATE_BACK_AND_FORTH;
boolean SBAF_direction = true;

void loop(){
  if(xbee.available() > 16){
    if(xbee.read() == 0x7E){
      // データが受信したことを示す
      digitalWrite(debugLED, HIGH);
      delay(10);
      digitalWrite(debugLED, LOW);
      // データのいらない部分は飛ばす
      for(int i = 0; i <= 13; i++){
        byte discard = xbee.read();
        Serial.write(discard);
      }
      byte newState = xbee.read();
      byte newCount = xbee.read();
      Serial.write(newState);
      Serial.write(newCount);
      switch (newState){
        case STATE_ONE_LED:
        case STATE_BACK_AND_FORTH:
          state = newState;
          count = 1;
          SBAF_direction = true;
          break;
        case STATE_DIRECT:
          state = newState;
          count = newCount;
        default:
          break;
      }
    }
  }
  
  switch (state){
    case STATE_ONE_LED:
      setLEDarray(count);
      if(count == B10000000){
        count = 1;
      } else {
        count = count << 1;
      }
      delay(sleep);
      break;
    case STATE_BACK_AND_FORTH:
      setLEDarray(count);
      if(SBAF_direction){
        count = count << 1;
      } else {
        count = count >> 1;
      }
      if(count == B10000000){
        SBAF_direction = false;
      } else if(count == B00000001){
        SBAF_direction = true;
      }
      delay(100);
    case STATE_DIRECT:
      setLEDarray(count);
      break;
    default:
      break;
  }
}

void setLEDarray(byte order){
  int pinnum = min(NUMBER_OF_LED, 8);
  for(int i=0; i<pinnum; i++){
    if(bitRead(order, i) == 1){
      digitalWrite(LEDs[i], HIGH);
    } else {
      digitalWrite(LEDs[i], LOW);
    }
  }
}

