#pragma once

#include "serial_mux.h"
#include "esphome/core/component.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/uart/uart_component.h"
#include <deque>
#include <functional>

namespace esphome::shs_test_serial {

class SerialTestComponent : public Component, public uart::UARTComponent {
 public:
  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }
  void set_light(light::LightState *light) { light_ = light; }
  void set_busy(std::function<bool()> busy) { busy_ = std::move(busy); }
  void set_forward_improv(bool enabled) { forward_improv_ = enabled; }
  bool is_running() const { return running_; }

  size_t available() override { return improv_rx_.size(); }
  bool peek_byte(uint8_t *data) override;
  bool read_array(uint8_t *data, size_t length) override;
  void write_array(const uint8_t *data, size_t length) override;
  uart::UARTFlushResult flush() override;
  // This is a virtual transport over the logger's existing UART driver.
  void load_settings(bool) override {}
  void check_logger_conflict() override {}

 protected:
  static constexpr uint32_t DURATION_MS = 8000;
  static constexpr const char *EFFECT = "Hardware Test Rainbow";
  void command_(const std::string &line);
  void capabilities_(const std::string &request_id);
  void event_(const std::string &request_id, const char *status, const char *reason = "");
  void finish_();
  SerialMux mux_;
  std::deque<uint8_t> improv_rx_;
  light::LightState *light_{nullptr};
  light::LightColorValues saved_values_;
  std::string saved_effect_;
  std::function<bool()> busy_;
  bool forward_improv_{false};
  bool running_{false};
  uint32_t started_at_{0};
  std::string request_id_;
  std::string completed_id_;
};

}  // namespace esphome::shs_test_serial
