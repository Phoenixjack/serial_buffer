#include "PhoenixJack_serialbuffer_V0.h"

PhoenixJack_serialbuffer port0(&Serial);   // Serial Monitor
PhoenixJack_serialbuffer port1(&Serial1);  // Pin 2 = RX0
PhoenixJack_serialbuffer port2(&Serial2);  // Pin 12 = RX1

char printbuffer[200];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  port0.init(115200, "basic buffer example\n\n");
  port1.init(115200);
  port2.init(9600);
}

void loop() {
  if (port0.check() || port1.check() || port2.check()) {
    digitalWrite(LED_BUILTIN, HIGH);
    if (port0.is_Buffer_Ready_To_Send()) {
      sprintf(printbuffer, "PORT0 (%03.1f Kbps) START: %i \t STOP: %i \t", ((float)port0.get_baud_rate() / 1000), port0.get_start_time(), port0.get_stop_time());
      strcat(printbuffer, port0.buffer);
      Serial.println(printbuffer);
      port0.reset();
    }
    if (port1.is_Buffer_Ready_To_Send()) {
      sprintf(printbuffer, "PORT1 (%03.1f Kbps) START: %i \t STOP: %i \t", ((float)port1.get_baud_rate() / 1000), port1.get_start_time(), port1.get_stop_time());
      strcat(printbuffer, port1.buffer);
      Serial.println(printbuffer);
      port1.reset();
    }
    if (port2.is_Buffer_Ready_To_Send()) {
      sprintf(printbuffer, "PORT2 (%000.1f Kbps) START: %i \t STOP: %i \t", ((float)port2.get_baud_rate() / 1000), port2.get_start_time(), port2.get_stop_time());
      Serial.println(printbuffer);
      port2.Send_Buffer(&Serial);
      Serial.println("");
    }
    digitalWrite(LED_BUILTIN, LOW);
  }
}
