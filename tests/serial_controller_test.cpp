#include "shs_test_serial.h"
#include <cassert>
using esphome::shs_test_serial::SerialTestComponent;

void input(SerialTestComponent &serial, const std::string &text) {
  test_uart_rx.insert(test_uart_rx.end(), text.begin(), text.end());
  serial.loop();
}

int main() {
  esphome::light::LightState light;
  SerialTestComponent serial;
  bool automation_busy = false;
  serial.set_light(&light);
  serial.set_busy([&] { return automation_busy; });
  serial.setup();
  assert(light.calls == 0);
  input(serial, "SHS_TEST/1 capabilities discover1\n");
  assert(light.calls == 0);
  input(serial, "SHS_TEST/1 led_test one\n");
  assert(serial.is_running() && light.remote_values.on);
  assert(light.effect == "Hardware Test Rainbow");
  assert(light.calls == 1 && light.saves == 0 && light.publishes == 0);
  test_clock = 7000;
  input(serial, "SHS_TEST/1 led_test one\nSHS_TEST/1 led_test other\n");
  assert(light.calls == 1);  // Neither retry nor double-click restarts the effect.
  test_clock = 7999; serial.loop();
  assert(serial.is_running());
  test_clock = 8000; serial.loop();  // No further input/browser connection needed.
  assert(!serial.is_running() && !light.remote_values.on);
  assert(light.remote_values.brightness == 0.25f && light.effect == "None");
  assert(light.calls == 2 && light.saves == 0 && light.publishes == 0);
  input(serial, "SHS_TEST/1 led_test one\n");
  assert(light.calls == 2);

  automation_busy = true;
  input(serial, "SHS_TEST/1 led_test blocked\n");
  assert(!serial.is_running() && light.calls == 2);
  automation_busy = false;
  light.effect_exists = false;
  input(serial, "SHS_TEST/1 led_test missing\n");
  assert(!serial.is_running());
  light.effect_exists = true;

  // Restore an already illuminated ring and its original effect, across rollover.
  light.remote_values.on = true;
  light.remote_values.brightness = 0.4f;
  light.effect = "Presence Active";
  test_clock = 0xFFFFFF00;
  input(serial, "SHS_TEST/1 led_test two\n");
  test_clock += 8000; serial.loop();
  assert(!serial.is_running());
  assert(light.remote_values.on && light.remote_values.brightness == 0.4f);
  assert(light.effect == "Presence Active" && light.saves == 0);

  // The virtual UART forwards an entire binary Improv request unchanged.
  serial.set_forward_improv(true);
  std::string frame = "IMPROV";
  frame += char(1); frame += char(3); frame += char(2);
  frame += char(2); frame += char(0); frame += char(0);
  input(serial, frame);
  assert(serial.available() == frame.size());
  uint8_t out[32];
  assert(!serial.read_array(out, frame.size() + 1));
  assert(serial.peek_byte(out) && out[0] == 'I');
  assert(serial.read_array(out, frame.size()));
  assert(std::string(reinterpret_cast<char *>(out), frame.size()) == frame);
  assert(serial.available() == 0);
  serial.write_array(out, frame.size());
  assert(test_uart_tx == frame);
}
