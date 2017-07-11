/* the tutorial code for 3x4 Matrix Keypad with Arduino is as
This code prints the key pressed on the keypad to the serial port*/

#include "Keypad.h"
#include "Timer.h"
#include <Adafruit_NeoPixel.h>

const byte Rows= 4; //number of rows on the keypad i.e. 4
const byte Cols= 3; //number of columns on the keypad i,e, 3

Timer t;
bool timerOn = false;
const int resetDuration = 8;
int duration = 0;

const int maxLength = 12;
char keyPhrase[maxLength+1];
int idx = 0;

//we will definne the key map as on the key pad:

char keymap[Rows][Cols]=
{
{'1', '2', '3'},
{'4', '5', '6'},
{'7', '8', '9'},
{'*', '0', '#'}
};

//keypad connections to the arduino terminals is given as:

byte rPins[Rows]= {8,7,6,5}; //Rows 0 to 3
byte cPins[Cols]= {4,3,2}; //Columns 0 to 2

// command for library forkeypad
//initializes an instance of the Keypad class
Keypad kpd= Keypad(makeKeymap(keymap), rPins, cPins, Rows, Cols);

#define ledPin 9
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, ledPin, NEO_GRB + NEO_KHZ800);

void blipFlash()
{
    strip.setPixelColor(0, 0, 255, 0);
    strip.show();
    delay(200);
    strip.setPixelColor(0, 0, 0, 0);
    strip.show();
    delay(100);
    strip.setPixelColor(0, 0, 255, 0);
    strip.show();
    delay(200);
    strip.setPixelColor(0, 0, 0, 0);
    strip.show();
    delay(100);
    strip.setPixelColor(0, 0, 255, 0);
    strip.show();
    delay(200);
    strip.setPixelColor(0, 0, 0, 0);
    strip.show(); 
}

void keepCount()
{
    if (timerOn)
    {
        if (duration == 0) {
            strip.setPixelColor(0, 255, 255, 255);
            strip.show();
        }
        duration += 1;
    }
}

void logKey(KeypadEvent keyChar)
{
    timerOn = true;
    keyPhrase[idx] = keyChar;
    idx = (idx + 1) % maxLength;
}

void longFlash()
{
    strip.setPixelColor(0, 255, 0, 0);
    strip.show();
    delay(750);
    strip.setPixelColor(0, 0, 0, 0);
    strip.show();
}

void keyPressed( KeypadEvent key )
{
    if (kpd.getState() == PRESSED)
    {
        if (key == '#') {
            sendKeyphrase();
        }
        else {
            logKey(key);
        }
    }
}

void resetKeyphraseAndTimer()
{
    duration = 0;
    timerOn = false;
    memset(keyPhrase, 0, sizeof(keyPhrase));
    idx = 0;
}

void sendKeyphrase()
{
    Serial.println("send");
    Serial.println(keyPhrase);
    longFlash();
    resetKeyphraseAndTimer();
    
}

void timerReset()
{
    blipFlash();
    resetKeyphraseAndTimer();
}

void setup()
{
     Serial.begin(9600);  // initializing serail monitor
     kpd.addEventListener(keyPressed);
     t.every(1000, keepCount);
     strip.begin();
     strip.show();
}

//If key is pressed, this key is stored in 'keypressed' variable
//If key is not equal to 'NO_KEY', then this key is printed out
void loop()
{
     if (duration > resetDuration)
     {
        timerReset();
     }

     kpd.getKey();

     t.update();
}

