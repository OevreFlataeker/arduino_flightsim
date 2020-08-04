#include <Joystick.h>
// Ruud Boer, June 2018
// Joystick with Arduino Leonardo
// CONFIG
#define MAX_SWITCHES 13 // the number of switches
byte switch_pin[MAX_SWITCHES] = {2,3,4,5,6,7,8,9,10,11,12,14,15}; // digital input pins
#define DEBOUNCE_TIME 5 // ms delay before the push button changes state
#define MAX_ANALOG 4 // the number of analog inputs
byte analog_pin[MAX_ANALOG] = {A0,A1,A2,A3}; // analog input pins X,Y,Z,THROT
#define ENC_CLK_PIN 3 // rotary encoder CLK input
#define ENC_DAT_PIN A5 // rotary encoder DATA input
#define ENC_MAX 100 // max value of the rotary encoder
#define ENC_MIN -100 // min value of the rotary encoder
#define ENC_ZERO 0 // value to jump to after pressing encoder switch
#define INCREMENT 1
// END CONFIG
// DECLARATIONS
#include "Joystick.h"
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK, 0, 0, true, false, false, false, false, false, false, false, false, false, false);
byte reading, clk, clk_old;
byte switch_state[MAX_SWITCHES];
byte switch_state_old[MAX_SWITCHES];
int analog_value[MAX_ANALOG+1]; // +1 for the rotary encoder value
unsigned long debounce_time[MAX_SWITCHES+1]; // +1 for CLK of rotary encoder
// END DECLARATIONS
// FUNCTIONS
int read_rotary_encoder() {
clk = digitalRead(ENC_CLK_PIN);
if (clk == clk_old) debounce_time[MAX_SWITCHES] = millis() + (unsigned long)DEBOUNCE_TIME;
else if (millis() > debounce_time[MAX_SWITCHES]) { // clk has changed and is stable long enough
clk_old = clk;
if (clk) {if (digitalRead(ENC_DAT_PIN)) return 100; else return -100;}
}
return 0;
}
// END FUNCTIONS
volatile int virtualPosition = 50;
volatile int on = 0;
void isr () {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > 5) {
    if (digitalRead(ENC_DAT_PIN) == LOW)
    {
      virtualPosition-=INCREMENT; // Could be -5 or -10
    }
    else {
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
      on =0;
    }
    digitalWrite(13,on);
}
// Keep track of when we were here last (no more than every 5ms)
lastInterruptTime = interruptTime;
}

// SETUP
void setup() {
for (byte i=0; i<MAX_SWITCHES; i++) pinMode(switch_pin[i],INPUT_PULLUP);
pinMode(ENC_CLK_PIN,INPUT_PULLUP);
pinMode(ENC_DAT_PIN,INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(ENC_CLK_PIN), isr, LOW);
pinMode(13,OUTPUT); // on board LED
digitalWrite(13,0);
Joystick.begin(false); 
Joystick.setXAxisRange(ENC_MIN, ENC_MAX);
//Joystick.setYAxisRange(-511, 511);
//Joystick.setZAxisRange(-511, 511);


// Joystick.setRudderRange(-ENC_MIN, ENC_MAX);
Serial.begin(9600);
Serial.println("Go!");
} // END SETUP
// LOOP
void loop() {
Serial.println(virtualPosition);
Joystick.setXAxis(virtualPosition);
//Joystick.setRudder(virtualPosition);
Joystick.sendState();
delay(10);
} // END LOOP
