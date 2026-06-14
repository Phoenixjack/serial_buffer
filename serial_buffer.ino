#include "serial_buffer.h"

PhoenixJack_serialbuffer port0(&Serial);   // Serial Monitor
PhoenixJack_serialbuffer port1(&Serial1);  // Board-dependent UART
PhoenixJack_serialbuffer port2(&Serial2);  // Board-dependent UART

char printbuffer[200];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  port0.init(115200, "basic buffer example\n\n");
  port1.init(115200);
  port2.init(9600);
}

void loop() {
  bool port0_updated = port0.check();
  bool port1_updated = port1.check();
  bool port2_updated = port2.check();

  if (port0_updated || port1_updated || port2_updated) {
    digitalWrite(LED_BUILTIN, HIGH);

    if (port0.is_Buffer_Ready_To_Send()) {
      snprintf(
        printbuffer,
        sizeof(printbuffer),
        "PORT0 (%lu bps) START: %lu \t STOP: %lu \t%s",
        (unsigned long)port0.get_baud_rate(),
        (unsigned long)port0.get_start_time(),
        (unsigned long)port0.get_stop_time(),
        port0.buffer
      );

      Serial.println(printbuffer);
      port0.reset();
    }

    if (port1.is_Buffer_Ready_To_Send()) {
      snprintf(
        printbuffer,
        sizeof(printbuffer),
        "PORT1 (%lu bps) START: %lu \t STOP: %lu \t%s",
        (unsigned long)port1.get_baud_rate(),
        (unsigned long)port1.get_start_time(),
        (unsigned long)port1.get_stop_time(),
        port1.buffer
      );

      Serial.println(printbuffer);
      port1.reset();
    }

    if (port2.is_Buffer_Ready_To_Send()) {
      snprintf(
        printbuffer,
        sizeof(printbuffer),
        "PORT2 (%lu bps) START: %lu \t STOP: %lu \t%s",
        (unsigned long)port2.get_baud_rate(),
        (unsigned long)port2.get_start_time(),
        (unsigned long)port2.get_stop_time(),
        port2.buffer
      );

      Serial.println(printbuffer);
      port2.reset();
    }

    digitalWrite(LED_BUILTIN, LOW);
  }
}
