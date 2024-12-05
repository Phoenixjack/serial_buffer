#include <stdint.h>
/*
TITLE: PhoenixJack_serialbuffer
VERSION: 0.0.0
DATE: 2024-11-21
AUTHOR: CHRIS MCCREARY
INTENT: Create a fast NON-blocking way of getting UART data in and out
BACKGROUND: Mimics the RS232 RTR/RTS/CTS functionality for ease of understanding;
            This functions similar to a spinlock for dualcore
            AND
            RTS/CTS can be tied to GPIOs for data signaling
            get_start_time() and get_stop_time() are available for precision time sync
*/


const uint8_t _max_buffer_size = 128;
struct PhoenixJack_serialbuffer {
private:
  uint32_t _baudrate = 9600;
  uint32_t _start_time = 0;
  uint32_t _lock_time = 0;
  bool _RS232_BUFFER_RTR = true;
  bool _RS232_BUFFER_RTS = false;
  bool _RS232_BUFFER_FULL = false;
  bool _RS232_REMOTE_CTS = false;
  bool _flush_on_overbuffer = false;
public:
  explicit PhoenixJack_serialbuffer(HardwareSerial* stream_source, bool flush_on_overbuffer);  // flush on overbuffer will cause the buffer to dump the contents and reset if buffer fills without an EOM
  HardwareSerial* port = nullptr;
  const char NL = (char)10;
  const char CR = (char)13;
  char buffer[_max_buffer_size];
  uint8_t index = 0;
  void init(uint32_t user_baud, String printme);
  bool isEOM(char c);
  void make_ready();
  void closeout();
  void reset();
  bool check();
  bool is_Buffer_Ready_To_Receive();
  bool is_Buffer_Ready_To_Send();
  void Remote_Unit_is_Ready_To_Receive(bool yes_or_no);
  void Send_Buffer(HardwareSerial* stream_destination);
  uint32_t get_baud_rate();
  uint32_t get_start_time();
  uint32_t get_stop_time();
};

PhoenixJack_serialbuffer::PhoenixJack_serialbuffer(HardwareSerial* stream_source = &Serial, bool flush_on_overbuffer = false) {
  port = stream_source;
  _flush_on_overbuffer = flush_on_overbuffer;
  make_ready();
};
void PhoenixJack_serialbuffer::init(uint32_t user_baud = 9600, String printme = "none") {
  if (user_baud == 4800 || user_baud == 9600 || user_baud == 115200) { _baudrate = user_baud; }  // sanity check
  port->begin(_baudrate);
  if (printme != "none") {
    delay(2000);
    port->print("\n\n\n\n\n");
    port->print(printme);
  }
};
bool PhoenixJack_serialbuffer::isEOM(char c) {
  if (c == (char)0 || c == NL || c == CR) {
    return true;
  } else {
    return false;
  }
};
void PhoenixJack_serialbuffer::make_ready() {
  index = 0;
  _RS232_BUFFER_RTR = true;
  _RS232_BUFFER_RTS = false;
  _RS232_BUFFER_FULL = false;
};
void PhoenixJack_serialbuffer::closeout() {
  _lock_time = millis();      // in case the user wants to synchronize data
  _RS232_BUFFER_RTR = false;  // lock the buffer for incoming data
  _RS232_BUFFER_RTS = true;   // let any external entity know we're ready
  buffer[index] = (char)0;    // set end of string (NULL)
};
void PhoenixJack_serialbuffer::reset() {
  port->flush();
  make_ready();
};
bool PhoenixJack_serialbuffer::check() {
  if (_RS232_BUFFER_RTR) {
    if (port->available()) {
      char rcvByte = port->read();
      if (isEOM(rcvByte)) {
        closeout();
      } else {
        if (index == 0) { _start_time = millis(); }
        buffer[index] = rcvByte;
        index++;
      }
      if (index >= _max_buffer_size - 1) {
        _RS232_BUFFER_FULL = true;
        if (_flush_on_overbuffer) {
          make_ready();
        } else {
          closeout();
        }
      }
    }
  }
  return _RS232_BUFFER_RTS;
};
bool PhoenixJack_serialbuffer::is_Buffer_Ready_To_Receive() {
  return _RS232_BUFFER_RTR;
};
bool PhoenixJack_serialbuffer::is_Buffer_Ready_To_Send() {
  return _RS232_BUFFER_RTS;
};
void PhoenixJack_serialbuffer::Remote_Unit_is_Ready_To_Receive(bool yes_or_no) {
  _RS232_REMOTE_CTS = yes_or_no;
};
void PhoenixJack_serialbuffer::Send_Buffer(HardwareSerial* stream_destination = &Serial) {  // will dump the contents of the buffer to the destination specified; defaults to Serial Monitor; then flushes and resets the buffer
  stream_destination->print(buffer);
  reset();
};
uint32_t PhoenixJack_serialbuffer::get_baud_rate() {
  return _baudrate;
};
uint32_t PhoenixJack_serialbuffer::get_start_time() {
  return _start_time;
};
uint32_t PhoenixJack_serialbuffer::get_stop_time() {
  return _lock_time;
};