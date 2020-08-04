#include <Joystick.h>
// Ruud Boer, June 2018
// Joystick with Arduino Leonardo

#include "Joystick.h"

// https://user-images.githubusercontent.com/466120/46096980-eedad300-c1c0-11e8-95f2-2dfa35d31120.png
// Our CLK_PIN is Pin 7 (Purple box) == PE6
// Our DAT_PIN is Pin 6 (Purple box) == PD7 == A7


#define DEBOUNCE_TIME 5 // ms delay before the push button changes state
#define ENC_CLK_PIN 7 // rotary encoder CLK input
#define ENC_DAT_PIN 6 // rotary encoder DATA input
#define ENC_MAX 100 // max value of the rotary encoder
#define ENC_MIN -100 // min value of the rotary encoder
#define ENC_ZERO 0 // value to jump to after pressing encoder switch
#define INCREMENT 1

#define TXLED 30 // The TX LED has a defined Arduino pin

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK, 0, 0, true, false, false, false, false, false, false, false, false, false, false);
byte reading, clk, clk_old;

volatile int virtualPosition = 50;
volatile int on = 0;
void isr () 
{
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > 5) 
  {
    if (digitalRead(ENC_DAT_PIN) == LOW)
    {
      virtualPosition-=INCREMENT; // Could be -5 or -10
    }
    else
    {
      virtualPosition+=INCREMENT; // Could be +5 or +10
    }
    virtualPosition = min(ENC_MAX, max(ENC_MIN, virtualPosition));

    // Let LED blink to indicate activity
    if (on==0)
    {   
      on=1;
    }
    else
    {
      on=0;
    }      
    digitalWrite(TXLED,on);
  }
  // Keep track of when we were here last (no more than every 5ms)
  lastInterruptTime = interruptTime;
}

// SETUP
void setup() {
  pinMode(ENC_CLK_PIN,INPUT_PULLUP);
  pinMode(ENC_DAT_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_CLK_PIN), isr, LOW);
  pinMode(TXLED,OUTPUT);

  Joystick.begin(false); 
  Joystick.setXAxisRange(ENC_MIN, ENC_MAX);
  Serial.begin(9600);
} 

void loop() {
  Serial.println(virtualPosition);
  Joystick.setXAxis(virtualPosition);
  Joystick.sendState();
  delay(10);
}
