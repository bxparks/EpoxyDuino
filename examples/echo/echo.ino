#include <Arduino.h>

void setup(void)
{
  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("\nEcho test"));
}

void loop(void)
{
  while (true) {
    if (Serial.available())
      Serial.print((char)Serial.read());
  }
}
