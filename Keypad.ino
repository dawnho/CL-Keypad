#include "Keypad.h"
#include "Timer.h"
#include "Adafruit_NeoPixel.h"
#include "SPI.h"
#include "Ethernet2.h"
#include "EthernetUdp2.h"

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 100);
IPAddress remoteIp(192, 168, 1, 1);

unsigned int localPort = 8888;
unsigned int rmtPort = 8000;

EthernetUDP Udp;

const byte Rows= 4;
const byte Cols= 3;

Timer t;
bool timerOn = false;
const int resetDuration = 8;
int duration = 0;

const int maxLength = 12;
char keyPhrase[maxLength+2];
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
    duration = 0;
    keyPhrase[idx] = keyChar;
    idx = (idx + 1) % maxLength;
}

// Flash long green once
void longFlash()
{
    delay(450);
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
    strip.setPixelColor(0, 255, 0, 0);
    strip.show();
    Udp.beginPacket(remoteIp, rmtPort);
    Udp.write(keyPhrase, strlen(keyPhrase));
    Udp.write("\n");
    Udp.endPacket();
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
     Ethernet.begin(mac, ip);
     Udp.begin(localPort);
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
