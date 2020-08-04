#include <Joystick.h>

#define DEBOUNCE_TIME 5 // ms delay before the push button changes state

#define ENC_CLK_PIN 3 // rotary encoder CLK input
#define ENC_DAT_PIN A5 // rotary encoder DATA input
#define ENC_MAX 100 // max value of the rotary encoder
#define ENC_MIN -100 // min value of the rotary encoder
#define ENC_ZERO 0 // value to jump to after pressing encoder switch
#define INCREMENT 1

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK, 0, 0, true, false, false, false, false, false, false, false, false, false, false);
byte reading, clk, clk_old;

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

pinMode(ENC_CLK_PIN,INPUT_PULLUP);
pinMode(ENC_DAT_PIN,INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(ENC_CLK_PIN), isr, LOW);
pinMode(13,OUTPUT); // on board LED
digitalWrite(13,0);
Joystick.begin(false); 
Joystick.setXAxisRange(ENC_MIN, ENC_MAX);

Serial.begin(9600);
Serial.println("Go!");
}

void loop() {
Serial.println(virtualPosition);
Joystick.setXAxis(virtualPosition);

Joystick.sendState();
delay(10);
}
