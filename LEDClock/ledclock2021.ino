#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "RTClib.h"

/*
 * Constants
 */

// led
const byte LED_SIZE = 106;  // (13*2)*4 + 2
const byte LED_PIN = 9;

// clock
const byte CELL_SIZE = 13;  // size of single time cell. Each time cell contains 13 led cells.
const byte HOUR1_POS = 0;   // start position of hour 1 time cell.
const byte HOUR2_POS = 26;  // start position of hour 2 time cell.
const byte COLON1_POS = 52; // start position of colon 1 time cell.
const byte COLON2_POS = 53; // start position of colon 2 time cell.
const byte MIN1_POS = 54;   // start position of minute 1 time cell.
const byte MIN2_POS = 80;   // start position of minute 2 time cell.

// button
const byte BUTTON_PIN = 0;

/**?
* 네오픽셀을 사용하기 위해 객체 하나를 생성한다.
* 첫번째 인자값은 네오픽셀의 LED 의 개수
* 두번째 인자값은 네오픽셀이 연결된 아두이노의 핀번호
* 세번째 인자값은 네오픽셀의 타입에 따라 바뀌는 flag
*/
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_SIZE, LED_PIN, NEO_GRB + NEO_KHZ800);

//RTC_DS3231 객체를 생성한다.
RTC_DS3231 rtc;

/**?
 * @var h12 : Flag value to indicate display time in 12-hours way or 24-hours way.
 */
bool h12=false;              //시간 표시 형식의 flag 값. true 일 시 12시간으로 표시. false 일시 24시간으로 표시.

/**?
 * Hard-coded LED Cell Patterns
 * led cell : a single hexagon containing two led.
 * time cell : group of 13 led cells, to display a single number in hexagonal pattern.
 */

const bool LedNumbers[10][13] = {
        //0
        {true,true,true,true,
        true,false,false,false,true,
        true,true,true,true},
        //1
        {false, false, false, false,
        true, true, true, true, true,
        false,false,false,false},
        //2
        {true, false, true, true,
        true, false, true, false, true,
        true,true,false,true},
        //3
        {true, false, false, true,
        true, false, true, false, true,
        true,true,true,true},
        //4
        {true, true, false, false,
        false, false, true, false, false,
        true,true,true,true},
        //5
        {true, true, false, true,
        true, false, true, false, true,
        true,false,true,true},
        //6
        {true, true, true, true,
        true, false, true, false, true,
        true,false,true,true},
        //7
        {true, true, false, false,
        false, false, false, false, true,
        true,true,true,true},
        //8
        {true, true, true, true,
        true, false, true, false, true,
        true,true,true,true},
        //9
        {true, true, false, false,
        false, false, true, false, true,
        true,true,true,true}
};

/**?
 * Setup arduino program
 * 1. Initialize and prepare NeoPixel led strip.
 * 2. Prepare Serial & Wire connection
 */
void setup()
{

    Serial.begin(9600);

    // Awful, but simple goto syntax.
    waitForRTC:
    delay(3000);
    if (!rtc.begin())
    {
        Serial.println("RTC를 찾을 수 없습니다");
        goto waitForRTC;
    }

    if (rtc.lostPower())
    {
        Serial.println("RTC가 전원이 없습니다. 시간을 설정합니다."); // RTC LOST-POWER fallback.
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // adjust rtc time.
    }

    strip.begin();

}

void loop()
{
    uint8_t c = strip.Color(255, 255, 255);
//    showClock(c, c, c, c);
    _testSingleCell(ledData[0], c)
}

/*
 * LED Clock display functions
 */

/**?
 * void showClock()
 * Parse each number of current time (ex: 12:34 -> 1,2,3,4)
 * and show them on LED strip.
 * Two led are in single cell.
 *
 * @param colorH1 : Color value of hour 1 time cell
 * @param colorH2 : Color value of hour 2 time cell
 * @param colorM1 : Color value of minute 1 time cell
 * @param colorM2 : Color value of minute 2 time cell
 */
void showClock(uint8_t colorH1, uint8_t colorH2, uint8_t colorM1, uint8_t colorM2)
{
    DateTime now = rtc.now(); //Retrieve current time from RTC.

    //DEBUG
    Serial.println("Current Date & Time: "); //현재 날짜와 시간
    Serial.print(now.year(), DEC); //년
    Serial.print('/');
    Serial.print(now.month(), DEC); // 월
    Serial.print('/');
    Serial.print(now.day(), DEC); // 날
    Serial.println();
    Serial.print(now.hour(), DEC); // 시간
    Serial.print(':');
    Serial.print(now.minute(), DEC); //분
    Serial.print(':');
    Serial.print(now.second(), DEC); // 초
    Serial.println();
    Serial.print(rtc.getTemperature()); //온도 표시도 가능합니다.
    Serial.println('C');

    byte h1, h2 = now.hour() / 10, now.hour() % 10;
    byte m1, m2 = now.minute() / 10, now.minute() % 10;

    _changeSingleCellStatus(LedNumbers[h1], HOUR1_POS, colorH1);  // 시간 첫째 자리
    _changeSingleCellStatus(LedNumbers[h2], HOUR2_POS, colorH2);  // 시간 둘째 자리
    _changeSingleCellStatus(LedNumbers[m1], MIN1_POS, colorM1);  // 분 첫째 자리
    _changeSingleCellStatus(LedNumbers[m2], MIN2_POS, colorM2);  // 분 둘째 자리
}

/**?
 * test first cell to debug.
 * @param ledData : array of boolean values which indicates a guideline to turn on led in a single cell to show number.
 * @param color : RGB color value to change cell color.
 */
void _testSingleCell(bool[] ledData, uint8_t color)
{
    _changeSingleCellStatus(ledData, HOUR1_POS, color);
}

/**?
 * change single cell's color.
 * @param ledData : array of boolean values which indicates a guideline to turn on led in a single cell to show number.
 * @param pos : start position on target cell. required for applying this function on every cells.
 * @param color : RGB color value to change cell color.
 */
void _changeSingleCellStatus(bool[] ledData, byte pos, uint8_t color)
{
    for (int i=0; i < CELL_SIZE; i ++)
    {
        if (ledData[i])
        {
            strip.setPixelColor(2*i+pos, color);
            strip.setPixelColor(2*i+pos+1, color);
            strip.show();
        }
    }
}

void setColonColor(uint8_t colon1. uint8_t colon2)
{
    strip.setPixelColor(COLON1_POS, colon1);
    strip.setPixelColor(COLON2_POS, colon2);
}
