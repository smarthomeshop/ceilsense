#pragma once

#include <cstdint>
#include <string>

namespace esphome::shs_test_serial {

// Frame before dispatch: an SHS command embedded in an Improv Wi-Fi password
// must never become a command. Buffers and time spent per input are bounded.
class SerialMux {
 public:
  template<typename Command, typename Improv>
  void feed(uint8_t byte, uint32_t now, Command command, Improv improv) {
    if (mode_ != IDLE && uint32_t(now - last_byte_) > 1000) reset_();
    last_byte_ = now;
    if (mode_ == IDLE) {
      if (byte == 'S') mode_ = SHS;
      else if (byte == 'I') mode_ = IMPROV;
      else return;
    }
    if (mode_ == DISCARD) {
      if (byte == '\n') reset_();
      return;
    }
    buffer_.push_back(static_cast<char>(byte));
    if (mode_ == SHS) {
      constexpr const char *prefix = "SHS_TEST/1 ";
      if (buffer_.size() <= 11 && byte != prefix[buffer_.size() - 1]) {
        reset_();
      } else if (byte == '\n') {
        buffer_.pop_back();
        if (!buffer_.empty() && buffer_.back() == '\r') buffer_.pop_back();
        command(buffer_.substr(11));
        reset_();
      } else if (buffer_.size() > 96) {
        buffer_.clear();
        mode_ = DISCARD;
      }
      return;
    }
    constexpr const char *prefix = "IMPROV";
    if (buffer_.size() <= 6 && byte != prefix[buffer_.size() - 1]) {
      reset_();
    } else if (buffer_.size() >= 10 && buffer_.size() == 10U + uint8_t(buffer_[8])) {
      // Includes header, version, type, length, payload and checksum.
      improv(buffer_);
      reset_();
    }
  }

  static bool parse_command(const std::string &line, std::string &verb, std::string &request_id) {
    auto space = line.find(' ');
    if (space == std::string::npos) return false;
    verb = line.substr(0, space);
    request_id = line.substr(space + 1);
    if (request_id.empty() || request_id.size() > 32) return false;
    for (char c : request_id) {
      if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-')) return false;
    }
    return verb == "capabilities" || verb == "led_test";
  }

 private:
  enum Mode { IDLE, SHS, IMPROV, DISCARD } mode_{IDLE};
  std::string buffer_;
  uint32_t last_byte_{0};
  void reset_() { mode_ = IDLE; buffer_.clear(); }
};

}  // namespace esphome::shs_test_serial
