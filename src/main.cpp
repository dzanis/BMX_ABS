#include <Arduino.h>
#include <Ticker.h>

#define PIN_HALL_EFFECT_SENSOR 5 // пин подключенный к выходу датчика холла
#define PIN_VALVE 12 // пин подключенный к клапану

#define PIN_PUMP 4 // пин подключенный к насосу

#define IMPULSES_PER_SEC 25 // импульсов в секунду (10км/ч)
#define CHECK_INTERVAL_MS 80 // интервал проверки в миллисекундах
// максимальное количество импульсов за интервал проверки 
#define MAX_IMPULSES_PER_CHECK_INTERVAL (CHECK_INTERVAL_MS/(1000/IMPULSES_PER_SEC))

// через какое время отключить клапан и насос ( значения в миллисекундах)
#define ALL_OFF_IF_WHEEL_DONT_UNLOK_MS 1000 // если колесо не разблокировалось

bool absActive = false; //если true то abs активна (скорость 10км/ч и больше)
bool wheelLocked = false; // если true то колесо заблокировано

Ticker checkImpulse;
Ticker tickerAllOff;

static volatile int interruptCounter = 0;
static void IRAM_ATTR handleInterrupt(void)
{
  if (digitalRead(PIN_HALL_EFFECT_SENSOR) == LOW)
    interruptCounter++;
}

void solenoidValveClose()
{
  digitalWrite(PIN_VALVE, HIGH);
}

void solenoidValveOpen()
{
  digitalWrite(PIN_VALVE, LOW);
}


void pumpOn()
{
  digitalWrite(PIN_PUMP, HIGH);
}

void pumpOff()
{
  digitalWrite(PIN_PUMP, LOW);
}

void allOff()
{
  // TODO вероятно,что здесь надо ставить wheelLocked = false;
  solenoidValveOpen();
  pumpOff();
}

void checkImpulseFunc()
{
  if(!interruptCounter && absActive)
  {
    wheelLocked = true;
    solenoidValveClose();
    pumpOn();
    tickerAllOff.once_ms(ALL_OFF_IF_WHEEL_DONT_UNLOK_MS , allOff);
  }
  else if(interruptCounter && wheelLocked)
  {
    wheelLocked = false;
    tickerAllOff.detach();
    allOff();
  }
  absActive = interruptCounter >= MAX_IMPULSES_PER_CHECK_INTERVAL;
  interruptCounter = 0;
}

void setup() {

  pinMode(PIN_VALVE, OUTPUT);
  pinMode(PIN_PUMP, OUTPUT);
  allOff();
  attachInterrupt(digitalPinToInterrupt(PIN_HALL_EFFECT_SENSOR), handleInterrupt, FALLING);
  checkImpulse.attach_ms(CHECK_INTERVAL_MS , checkImpulseFunc);
  // для индикации светодиодом на плате Node MCU
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  digitalWrite(LED_BUILTIN,!absActive); // включить светодиод если ABS активно
  delay(50);
}