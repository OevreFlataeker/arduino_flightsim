/*********************************************************************************************
 * TrimMicro.ino                                                                             *
 * Emulates a 1-axis josytsick using a KY40 rotary encoder on a trim wheel and moves a       *
 * visual indicator on a SG90 servo motor according to reading                               *
 * ******************************************************************************************/
 
#include <Servo.h>
#include <Joystick.h>

// https://user-images.githubusercontent.com/466120/46096980-eedad300-c1c0-11e8-95f2-2dfa35d31120.png
// Our CLK_PIN is Pin 7 (Purple box) == PE6
// Our DAT_PIN is Pin 6 (Purple box) == PD7 == A7
// Our SERVO PIN is Pin8 (Purple box) == PB4


#define DEBOUNCE_TIME 5 // ms delay before the push button changes state
#define ENC_CLK_PIN 7 // rotary encoder CLK input
#define ENC_DAT_PIN 6 // rotary encoder DATA input
#define ENC_MAX 100 // max value of the rotary encoder
#define ENC_MIN -100 // min value of the rotary encoder
#define ENC_ZERO 0 // value to jump to after pressing encoder switch
#define INCREMENT 1

#define SERVOPWM 8
#define SERVOMIN 60
#define SERVOMAX 110
#define MAXANGLE 60
#define TXLED 30 // The TX LED has a defined Arduino pin
// #define DEBUG 0


Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_JOYSTICK, 0, 0, true, false, false, false, false, false, false, false, false, false, false);
Servo servo;
byte reading, clk, clk_old;

volatile int virtualPosition = 0;
volatile int on = 0;
volatile int icnt = 0;
volatile bool doMove = true;
void isr () 
{
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  icnt++;
  // If interrupts come faster than 5ms, assume it's a bounce and ignore
  if (interruptTime - lastInterruptTime > DEBOUNCE_TIME) 
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
      
    doMove = true;
  }
  // Keep track of when we were here last (no more than every 5ms)
  lastInterruptTime = interruptTime;
}

// SETUP
void setup() {
  pinMode(ENC_CLK_PIN,INPUT_PULLUP);
  pinMode(ENC_DAT_PIN,INPUT_PULLUP);
  
  pinMode(TXLED,OUTPUT);

  Joystick.begin(false); 
  Joystick.setXAxisRange(ENC_MIN, ENC_MAX);
  servo.write(90); // Initial middle position  
  servo.attach(SERVOPWM,500,2400); // Set absolute margins (found in forum)
#ifdef DEBUG  
  Serial.begin(9600);
#endif
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(ENC_CLK_PIN), isr, FALLING );
} 

void move(int vpos)
{
  static int old_servo_val=0;   
  int servoVal = map(vpos,-100,100,SERVOMIN,SERVOMAX);
  
  if (old_servo_val!=servoVal) // Only move motor when needed
  {
#ifdef DEBUG
    servo.write(servoVal);
#endif
    old_servo_val=servoVal;    
  }  
}
void loop() {    
#ifdef DEBUG
  Serial.println(virtualPosition);
#endif
  Joystick.setXAxis(virtualPosition);
  if (doMove)
  {  
    move(virtualPosition);
    doMove=false;
  }
  Joystick.sendState();
  delay(20);    
}
