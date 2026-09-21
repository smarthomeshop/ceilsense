#include "shs_test_serial.h"
#include "esphome/components/logger/logger.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include <driver/uart.h>

namespace esphome::shs_test_serial {
static const char *const TAG = "shs_test";

void SerialTestComponent::setup() {
  capabilities_("");
}

void SerialTestComponent::loop() {
  // A fixed deadline still ends the effect if the browser closes or disconnects.
  if (running_ && uint32_t(millis() - started_at_) >= DURATION_MS) finish_();
  uint8_t bytes[256];
  int count = uart_read_bytes(logger::global_logger->get_uart_num(), bytes, sizeof(bytes), 0);
  for (int i = 0; i < count; i++) {
    mux_.feed(bytes[i], millis(), [this](const std::string &line) { command_(line); },
              [this](const std::string &frame) {
                if (forward_improv_ && improv_rx_.size() + frame.size() <= 1024)
                  improv_rx_.insert(improv_rx_.end(), frame.begin(), frame.end());
              });
  }
}

void SerialTestComponent::capabilities_(const std::string &request_id) {
  ESP_LOGI(TAG, "SHS_TEST/1 {\"type\":\"capabilities\",\"request_id\":\"%s\",\"commands\":[\"led_test\"],\"id\":\"status_led\",\"effect\":\"rainbow\",\"duration_ms\":%u,\"requires_visual_confirmation\":true}",
           request_id.c_str(), static_cast<unsigned>(DURATION_MS));
}

void SerialTestComponent::event_(const std::string &request_id, const char *status, const char *reason) {
  ESP_LOGI(TAG, "SHS_TEST/1 {\"type\":\"manual_test\",\"id\":\"status_led\",\"request_id\":\"%s\",\"status\":\"%s\",\"reason\":\"%s\",\"duration_ms\":%u,\"requires_visual_confirmation\":true}",
           request_id.c_str(), status, reason, static_cast<unsigned>(DURATION_MS));
}

void SerialTestComponent::command_(const std::string &line) {
  std::string verb, request_id;
  if (!SerialMux::parse_command(line, verb, request_id)) return;
  if (verb == "capabilities") {
    capabilities_(request_id);
    return;
  }
  if (running_) {
    // Retrying the same request only repeats its acknowledgement, never its timer.
    event_(request_id, request_id == request_id_ ? "running" : "busy",
           request_id == request_id_ ? "" : "test_in_progress");
    return;
  }
  if (request_id == completed_id_) {
    event_(request_id, "complete");
    return;
  }
  if (busy_ && busy_()) {
    event_(request_id, "busy", "led_automation_active");
    return;
  }
  if (light_->get_effect_index(EFFECT) == 0) {
    event_(request_id, "error", "effect_unavailable");
    return;
  }
  saved_values_ = light_->remote_values;
  saved_effect_ = light_->get_effect_name().c_str();
  request_id_ = request_id;
  running_ = true;
  started_at_ = millis();
  auto call = light_->turn_on();
  call.set_transition_length(0);
  call.set_brightness(0.65f);
  call.set_color_mode(light::ColorMode::RGB);
  call.set_color_brightness(1.0f);
  call.set_rgb(1.0f, 1.0f, 1.0f);
  call.set_effect(EFFECT);
  call.set_save(false);
  call.set_publish(false);
  call.perform();
  event_(request_id_, "running");
}

void SerialTestComponent::finish_() {
  auto call = light_->make_call();
  call.from_light_color_values(saved_values_);
  call.set_effect(saved_effect_);
  call.set_transition_length(0);
  call.set_save(false);
  call.set_publish(false);
  call.perform();
  running_ = false;
  completed_id_ = request_id_;
  event_(request_id_, "complete");
}

bool SerialTestComponent::peek_byte(uint8_t *data) {
  if (improv_rx_.empty()) return false;
  *data = improv_rx_.front();
  return true;
}

bool SerialTestComponent::read_array(uint8_t *data, size_t length) {
  if (improv_rx_.size() < length) return false;
  for (size_t i = 0; i < length; i++) {
    data[i] = improv_rx_.front();
    improv_rx_.pop_front();
  }
  return true;
}

void SerialTestComponent::write_array(const uint8_t *data, size_t length) {
  uart_write_bytes(logger::global_logger->get_uart_num(), data, length);
}

uart::UARTFlushResult SerialTestComponent::flush() {
  return uart_wait_tx_done(logger::global_logger->get_uart_num(), pdMS_TO_TICKS(100)) == ESP_OK
      ? uart::UARTFlushResult::UART_FLUSH_RESULT_SUCCESS
      : uart::UARTFlushResult::UART_FLUSH_RESULT_TIMEOUT;
}

}  // namespace esphome::shs_test_serial
