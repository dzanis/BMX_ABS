#include <Arduino.h>
#include <Ticker.h>

#define PIN_HALL_EFFECT_SENSOR 5 // пин подключенный к выходу датчика холла
#define PIN_SOLENOID_1 12 // пин подключенный к соленойду
#define PIN_SOLENOID_2 14 // пин подключенный к соленойду
#define PIN_MOTOR 4 // пин подключенный к мотору

bool itsMoving = false; // если двигается (больше 10км/ч)

Ticker checkImpulse;

static volatile int interruptCounter = 0;
static void IRAM_ATTR handleInterrupt(void)
{
  if (digitalRead(PIN_HALL_EFFECT_SENSOR) == LOW)
    interruptCounter++;
}

void checkImpulseFunc()
{
  itsMoving = false;

  if(interruptCounter > 1)
  {
    itsMoving = true;
  }
  interruptCounter = 0;
}

void setup() {

  attachInterrupt(digitalPinToInterrupt(PIN_HALL_EFFECT_SENSOR), handleInterrupt, FALLING);
  checkImpulse.attach_ms(100 , checkImpulseFunc);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  digitalWrite(LED_BUILTIN,!itsMoving);
  delay(50);
}