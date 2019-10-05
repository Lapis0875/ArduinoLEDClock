#include <Wire.h>
#include <DS3231.h>
#include <TimeLib.h>

#include <Adafruit_NeoPixel.h>

#define PIN 9

//네오픽셀을 사용하기 위해 객체 하나를 생성한다. 
//첫번째 인자값은 네오픽셀의 LED의 개수
//두번째 인자값은 네오픽셀이 연결된 아두이노의 핀번호
//세번째 인자값은 네오픽셀의 타입에 따라 바뀌는 flag
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(106, PIN, NEO_GRB + NEO_KHZ800);

DS3231 Clock;
 
byte Year, Month, Date, DoW, Hour, Minute, Second;    //RTC 값을 저장할 변수들
bool Century=false, h12=false, PM=false;
byte preMinute;



void setup() {
  // Start the serial port
  Serial.begin(4800);
  // Start the I2C interface
  Wire.begin();
  //strip.begin(); //네오픽셀을 초기화하기 위해 모든LED를 off시킨다
  //strip.show(); 
}

void loop() {

  // If something is coming in on the serial line, it's
  // a time correction so set the clock accordingly.
  if(Serial.available()){
     GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);
     preMinute = Clock.getMinute();
     Serial.print("preMinute : ");
     Serial.println(preMinute);
     Clock.setClockMode(h12);  // set to 24h
     //setClockMode(true);   // set to 12h
 
     Clock.setYear(Year);
     Clock.setMonth(Month);
     Clock.setDate(Date);
     Clock.setDoW(DoW);
     Clock.setHour(Hour);
     Clock.setMinute(Minute);
     Clock.setSecond(Second);
  }
  if(preMinute != Clock.getMinute()){
    Serial.print("Previous preMinute value : ");
    Serial.println(preMinute);
    Serial.println("Minute value changed.");
    preMinute = Clock.getMinute();
    Serial.print("preMinute : ");
    Serial.println(preMinute);
    showClock();
    //showClockTimeValues();
  }
  showRTCTimeValues();
  //testLEDs(13, PIN, 1);
}

/*
 * RTC Functions
 * -> Get real time values from Serial, and set them to RTC module.
 */

void GetDateStuff(byte& Year, byte& Month, byte& Day, byte& DoW,
        byte& Hour, byte& Minute, byte& Second) {
    // Call this if you notice something coming in on
    // the serial port. The stuff coming in should be in
    // the order YYMMDDwHHMMSS, with an 'x' at the end.
    boolean GotString = false;
    char InChar;
    byte Temp1, Temp2;
    char InString[14];
 
    byte j=0;
    //YYMMDDwHHMMSSx 형식의 날짜 정보를 가지고 있지 않을때, 시리얼 포트를 통해 정보를 입력받는다.
    while (!GotString) {
        if (Serial.available()) {
            //시리얼 포트에서 전송된 j번째 글자를 InChar에 저장, 그리고 InChar의 값을 InString[j](InString 배열의 j번째 칸)에 저장한다.
            InChar = Serial.read();
            Serial.print("InChar : ");
            Serial.println(InChar);
            InString[j] = InChar;
            j += 1; //다음 글자를 읽어내기 위해 j를 1 증가시킨다.
            //마지막 글자가 x일때, 글자를 읽어내는 작업을 종료한다.
            if (InChar == 'x') {
                GotString = true;
                Serial.println("Raw Serial data received. Starting data processing sequence");
            }
        }
    }
    //읽어낸 문자열 표시
    Serial.println(InString);
    // Read Year first
    Temp1 = (byte)InString[0] -48;
    Temp2 = (byte)InString[1] -48;
    Year = Temp1*10 + Temp2;
    Serial.println(Year);
    // now month
    Temp1 = (byte)InString[2] -48;
    Temp2 = (byte)InString[3] -48;
    Month = Temp1*10 + Temp2;
    Serial.println(Month);
    // now date
    Temp1 = (byte)InString[4] -48;
    Temp2 = (byte)InString[5] -48;
    Day = Temp1*10 + Temp2;
    Serial.println(Day);
    // now Day of Week
    DoW = (byte)InString[6];
    Serial.println(DoW);      
    // now Hour
    Temp1 = (byte)InString[7] -48;
    Temp2 = (byte)InString[8] -48;
    Hour = Temp1*10 + Temp2;
    Serial.println(Hour);
    // now Minute
    Temp1 = (byte)InString[9] -48;
    Temp2 = (byte)InString[10] -48;
    Minute = Temp1*10 + Temp2;
    Serial.println(Minute);
    // now Second
    Temp1 = (byte)InString[11] -48;
    Temp2 = (byte)InString[12] -48;
    Second = Temp1*10 + Temp2;
    Serial.println(Second);
    Serial.println("RTC data received. Ending data processing sequence. Turning LEDs on.");
}
//On Developing. Dumb Coder wrote it, and gonna fix it. T^T
void showRTCTimeValues(){
  String sCommand;
  char InChar;
  bool bGotCommand=false;
  while (!bGotCommand){
      if(Serial.available()){
        if (Serial.available()) {
            if (InChar == '/'){
              //시리얼 포트에서 전송된 j번째 글자를 InChar에 저장, 그리고 InChar의 값을 InString[j](InString 배열의 j번째 칸)에 저장한다.
              InChar = Serial.read();
              Serial.print("InChar : ");
              Serial.println(InChar);
              sCommand += InChar;
              //마지막 글자가 _ 일때, 글자를 읽어내는 작업을 종료한다.
              if(InChar = '_'){
                bGotCommand = true;
                Serial.println("Serial Command received. Activating command sequence.");
              }
            }
        }
     }
  } 
  if(bGotCommand) {
    if(sCommand == "/showRTCtime_"){
      Serial.print("Year : ");
      Serial.println(Clock.getYear());
      Serial.print("Month : ");
      Serial.println(Clock.getMonth(Century));
      Serial.print("Date : ");
      Serial.println(Clock.getDate());
      Serial.print("DoW : ");
      Serial.println(Clock.getDoW());
      Serial.print("Hour : ");
      Serial.println(Clock.getHour(h12, PM));
      Serial.print("Minute : ");
      Serial.println(Clock.getMinute());
      Serial.print("Second : ");
      Serial.println(Clock.getSecond());
    }
  }
}

/*
 * LED Pattern Functions
 * -> define, show the shape of numbers on clock.
 * -> 1 cell size : 13(0~12) array patters.
 * ref : LEDClock.123D file contains the complete version of led clock, and it's led number info.
 */

bool LedNumbers[10][13] = {{true,true,true,true,
                            true,false,false,false,true,
                            true,true,true,true},
                           {false, false, false, false, 
                            true, true, true, true, true,
                            false,false,false,false},
                           {true, false, true, true, 
                            true, false, true, false, true,
                            true,true,false,true},
                           {true, false, false, true, 
                            true, false, true, false, true,
                            true,true,true,true},
                           {true, true, false, false, 
                            false, false, true, false, false,
                            true,true,true,true},
                           {true, true, false, true, 
                            true, false, true, false, true,
                            true,false,true,true},
                           {true, true, true, true, 
                            true, false, true, false, true,
                            true,false,true,true},
                           {true, true, false, false, 
                            false, false, false, false, true,
                            true,true,true,true},
                           {true, true, true, true, 
                            true, false, true, false, true,
                            true,true,true,true},
                           {true, true, false, false, 
                            false, false, true, false, true,
                            true,true,true,true}};
 
//DEPRPECATED : Emergency LED test function.
void testLEDs(byte ledcnt, byte pinLED, byte mode){
  Serial.println("Starting LED test function. This function is made to test whether the leadded LEDs are working properly or not.");
  Adafruit_NeoPixel testStrip = Adafruit_NeoPixel(ledcnt, pinLED, NEO_GRB + NEO_KHZ800);
  testStrip.begin();
  switch(mode){
    case 0:
      for(int i=0; i<ledcnt; i++){
        testStrip.setPixelColor(i,testStrip.Color(29,219,22));
      }
      break;
    case 1:
      byte Hour1 = Clock.getHour(h12, PM)/10;
      Serial.print("Hour1 : ");
      Serial.println(Hour1);
      //1st cell - Hour1
      for(int i=0; i < 13; i++){
        //불을 켜야 할 때 판단
        //debugging code
        Serial.print(2*i);
        Serial.print(", ");
        Serial.print(2*i+1);
        Serial.print("번째 LED의 상태 : ");
        Serial.println(LedNumbers[Hour1][i]);
        if(LedNumbers[Hour1][i]){
          testStrip.setPixelColor(2*i,testStrip.Color(255,187,0));
          testStrip.setPixelColor(2*i+1,testStrip.Color(255,187,0));
          //01 23 45 67 89 1011 1213 1415 1617 1819 2021 2223 2425
        }
     }
     break;
  }
  
  testStrip.show();
}

                            

void showClock(){
  //시, 분 각각 두 칸 씩 숫자가 필요하므로 각각 구한다.
  byte Hour1 = Clock.getHour(h12, PM)/10;
  Serial.print("Hour1 : ");
  Serial.println(Hour1);
  byte Hour2 = Clock.getHour(h12, PM)%10;
  Serial.print("Hour2 : ");
  Serial.println(Hour2);
  byte Minute1 = Clock.getMinute()/10;
  Serial.print("Minute1 : ");
  Serial.println(Minute1);
  byte Minute2 = Clock.getMinute()%10;
  Serial.print("Minute2 : ");
  Serial.println(Minute2);

  //1st cell - Hour1
  for(int i=0; i < 13; i++){
    //불을 켜야 할 때 판단
      //debugging code
      Serial.print(2*i);
      Serial.print(", ");
      Serial.print(2*i+1);
      Serial.print("번째 LED의 상태 : ");
      Serial.println(LedNumbers[Hour1][i]);
      /*
    if(LedNumbers[Hour1][i]){
      strip.setPixelColor(2*i,strip.Color(255,187,0));
      strip.setPixelColor(2*i+1,strip.Color(255,187,0));
      //01 23 45 67 89 1011 1213 1415 1617 1819 2021 2223 2425
    }*/
  }
  //2nd cell - Hour2
  for(int i=0; i < 13; i++){
    //불을 켜야 할 때 판단
      //debugging code
      Serial.print(2*i+26);
      Serial.print(", ");
      Serial.print(2*i+27);
      Serial.print("번째 LED의 상태 : ");
      Serial.println(LedNumbers[Hour1][i]);
      /*
    if(LedNumbers[Hour2][i]){
      strip.setPixelColor(2*i+26,strip.Color(255,187,0));
      strip.setPixelColor(2*i+27,strip.Color(255,187,0));
      //2627 2829 3031 3233 3435 3637 3839 4041 4243 4445 4647 4849 5051
    }*/
  }
  //':' between Hour and Minute
      //debugging code
      Serial.print(52);
      Serial.print(", ");
      Serial.print(53);
      Serial.print("번째 LED의 상태 : ");
      Serial.println(true);
      /*
  strip.setPixelColor(52,strip.Color(255,187,0));
  strip.setPixelColor(53,strip.Color(255,187,0));
  */
  
  //3rd cell - Minute1
  for(int i=0; i < 13; i++){
    //불을 켜야 할 때 판단
      //debugging code
      Serial.print(2*i+54);
      Serial.print(", ");
      Serial.print(2*i+55);
      Serial.print("번째 LED의 상태 : ");
      Serial.println(LedNumbers[Hour1][i]);
      /*
    if(LedNumbers[Minute1][i]){
      strip.setPixelColor(2*i+54,strip.Color(103,153,255));
      strip.setPixelColor(2*i+55,strip.Color(103,153,255));
      //5455 5657 5859 6061 6263 6465 6667 6869 7071 7273 7475 7677 7879
    }*/
  }
  //4th cell - Minute2
  for(int i=0; i < 13; i++){
    //불을 켜야 할 때 판단
      //debugging code
      Serial.print(2*i+80);
      Serial.print(", ");
      Serial.print(2*i+81);
      Serial.print("번째 LED의 상태 : ");
      Serial.println(LedNumbers[Hour1][i]);
      /*
    if(LedNumbers[Minute2][i]){
      strip.setPixelColor(2*i+80,strip.Color(103,153,255));
      strip.setPixelColor(2*i+81,strip.Color(103,153,255));
      //8081 8283 8485 8687 8889 9091 9293 9495 9697 9899 100101 102103 104105
    }*/
  }
  //strip.show();
  delay(50);
}
/*
//DEPRECATED FUNCTION - Only Use When You Don't Have Strip LED But You Need To Test This Code
void showClockTimeValues(){
  
  //시, 분 각각 두 칸 씩 숫자가 필요하므로 각각 구한다.
  byte Hour1 = Clock.getHour(h12, PM)/10;
  Serial.print("Hour1 : ");
  Serial.println(Hour1);
  byte Hour2 = Clock.getHour(h12, PM)%10;
  Serial.print("Hour2 : ");
  Serial.println(Hour2);
  byte Minute1 = Clock.getMinute()/10;
  Serial.print("Minute1 : ");
  Serial.println(Minute1);
  byte Minute2 = Clock.getMinute()%10;
  Serial.print("Minute2 : ");
  Serial.println(Minute2);
  
}*/
