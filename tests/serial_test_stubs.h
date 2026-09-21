// Small hardware doubles for executing the actual serial test controller on host.
#pragma once
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <deque>
#include <string>

inline uint32_t test_clock = 0;
inline std::deque<uint8_t> test_uart_rx;
inline std::string test_uart_tx;
#define ESP_OK 0
#define pdMS_TO_TICKS(value) (value)
#define ESP_LOGI(tag, format, ...) std::printf(format "\n", __VA_ARGS__)
inline int uart_read_bytes(int, uint8_t *data, size_t size, int) {
  size_t count = 0;
  while (count < size && !test_uart_rx.empty()) {
    data[count++] = test_uart_rx.front(); test_uart_rx.pop_front();
  }
  return count;
}
inline void uart_write_bytes(int, const uint8_t *data, size_t size) {
  test_uart_tx.append(reinterpret_cast<const char *>(data), size);
}
inline int uart_wait_tx_done(int, int) { return ESP_OK; }

namespace esphome {
inline uint32_t millis() { return test_clock; }
namespace setup_priority { constexpr float AFTER_WIFI = 200; }
class Component {
 public:
  virtual void setup() {}
  virtual void loop() {}
  virtual float get_setup_priority() const { return 0; }
};
namespace uart {
enum class UARTFlushResult { UART_FLUSH_RESULT_SUCCESS, UART_FLUSH_RESULT_TIMEOUT };
class UARTComponent {
 public:
  virtual size_t available() = 0;
  virtual bool peek_byte(uint8_t *) = 0;
  virtual bool read_array(uint8_t *, size_t) = 0;
  virtual void write_array(const uint8_t *, size_t) = 0;
  virtual UARTFlushResult flush() = 0;
  virtual void load_settings(bool) = 0;
  virtual void check_logger_conflict() = 0;
};
}
namespace logger {
struct Logger { int get_uart_num() { return 0; } };
inline Logger test_logger;
inline Logger *global_logger = &test_logger;
}
namespace light {
enum class ColorMode { RGB };
struct LightColorValues { bool on = false; float brightness = 0.25f; };
class LightState;
struct LightCall {
  LightState *parent;
  LightColorValues values;
  std::string effect;
  bool save = true, publish = true;
  void set_transition_length(int) {}
  void set_brightness(float value) { values.brightness = value; }
  void set_color_mode(ColorMode) {}
  void set_color_brightness(float) {}
  void set_rgb(float, float, float) {}
  void set_effect(const std::string &value) { effect = value; }
  void set_save(bool value) { save = value; }
  void set_publish(bool value) { publish = value; }
  void from_light_color_values(const LightColorValues &value) { values = value; }
  void perform();
};
class LightState {
 public:
  LightColorValues remote_values;
  std::string effect = "None";
  int calls = 0, saves = 0, publishes = 0;
  bool effect_exists = true;
  int get_effect_index(const char *) { return effect_exists ? 1 : 0; }
  std::string get_effect_name() { return effect; }
  LightCall make_call() { return {this, remote_values, effect}; }
  LightCall turn_on() { auto call = make_call(); call.values.on = true; return call; }
};
inline void LightCall::perform() {
  parent->calls++; parent->saves += save; parent->publishes += publish;
  parent->remote_values = values; parent->effect = effect;
}
}
}
