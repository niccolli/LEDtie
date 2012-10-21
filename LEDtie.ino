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

byte state = STATE_ONE_LED;
//byte state = STATE_BACK_AND_FORTH;

void loop(){
  byte directNumber = 0;
  
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
      byte directNumber = xbee.read();
      switch (newState){
        case STATE_ONE_LED:
        case STATE_BACK_AND_FORTH:
        case STATE_DIRECT:
          state = newState;
          break;
        default:
          break;
      }
    }
  }
  
  switch (state){
    case STATE_ONE_LED:
      setOneLED();
      delay(sleep);
      break;
    case STATE_BACK_AND_FORTH:
      setBackAndForth();
      break;
    case STATE_DIRECT:
      setLEDarray(directNumber);
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

void setOneLED(){
  static byte count = 1;
  static int lastLEDchanged = 0;
  
  // プログラムの実行時間を取得し、最後にLEDの点灯を変えた時間と比較
  // Get the running time, then compare the last time LED changed.
  int now = millis();
  if ( (now - lastLEDchanged) > 500 ){
    setLEDarray(count);
    lastLEDchanged = now;
    
    // LEDを点灯させる位置を変える
    // Changing the turn on LEDs
    if(count == B10000000){
      count = 1;
    } else {
      count = count << 1;
    }
  }
}

void setBackAndForth(){
  static byte count = 1;
  static int lastLEDchanged = 0;
  static boolean LEDdirection = true;

  int now = millis();
  if ( (now - lastLEDchanged) > 100 ){
    setLEDarray(count);
    lastLEDchanged = now;
    
    // LEDを点灯させる位置を変える
    // Changing the turn on LEDs
    if(SBAF_direction){
      count = count << 1;
    } else {
      count = count >> 1;
    }
    if(count == B10000000){
      LEDdirection = false;
    } else if(count == B00000001){
      LEDdirection = true;
    }
  }    
}
