#include "Keypad.h"
#include "Timer.h"
#include <Adafruit_NeoPixel.h>

const byte Rows= 4;
const byte Cols= 3;

Timer t;
bool timerOn = false;
const int resetDuration = 8;
int duration = 0;

const int maxLength = 12;
char keyPhrase[maxLength+1];
int idx = 0;

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

Keypad kpd= Keypad(makeKeymap(keymap), rPins, cPins, Rows, Cols);

#define ledPin 9
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, ledPin, NEO_GRB + NEO_KHZ800);

// Flash red three times
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

// Keeps track of current key pressed duration
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

// Save key pressed to array
void logKey(KeypadEvent keyChar)
{
    timerOn = true;
    keyPhrase[idx] = keyChar;
    idx = (idx + 1) % maxLength;
}

// Flash long green once
void longFlash()
{
    strip.setPixelColor(0, 255, 0, 0);
    strip.show();
    delay(750);
    strip.setPixelColor(0, 0, 0, 0);
    strip.show();
}

// Event callback for key pressed
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

// Reset all to base state
void resetKeyphraseAndTimer()
{
    duration = 0;
    timerOn = false;
    memset(keyPhrase, 0, sizeof(keyPhrase));
    idx = 0;
}

// Send keyphrase and reset
void sendKeyphrase()
{
    Serial.println("send");
    Serial.println(keyPhrase);
    longFlash();
    resetKeyphraseAndTimer();
    
}

// Reset state when timer duration has run out
void timerReset()
{
    blipFlash();
    resetKeyphraseAndTimer();
}

void setup()
{
     Serial.begin(9600);
     kpd.addEventListener(keyPressed);
     t.every(1000, keepCount);
     strip.begin();
     strip.show();
}

void loop()
{
     if (duration > resetDuration)
     {
        timerReset();
     }

     kpd.getKey();

     t.update();
}
