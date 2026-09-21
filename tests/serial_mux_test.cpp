#include "serial_mux.h"
#include <cassert>
#include <vector>
using esphome::shs_test_serial::SerialMux;

int main() {
  SerialMux mux;
  std::vector<std::string> commands, frames;
  uint32_t now = 10;
  auto feed = [&](const std::string &bytes) {
    for (uint8_t byte : bytes)
      mux.feed(byte, now++, [&](const auto &line) { commands.push_back(line); },
               [&](const auto &frame) { frames.push_back(frame); });
  };
  feed("SHS_TEST/1 capa");
  feed("bilities req_1\r\nSHS_TEST/1 led_test req-2\n");
  assert(commands == std::vector<std::string>({"capabilities req_1", "led_test req-2"}));

  // Binary payloads can contain arbitrary bytes, newlines, and an entire SHS
  // command. Only the Improv consumer may receive these bytes.
  std::string payload = "SHS_TEST/1 led_test not_a_command\n";
  payload.push_back('\0');
  std::string frame = "IMPROV";
  frame += char(1); frame += char(3); frame += char(payload.size());
  frame += payload;
  uint8_t checksum = 0;
  for (uint8_t c : frame) checksum += c;
  frame += char(checksum);
  feed(frame);
  assert(frames == std::vector<std::string>({frame}));
  assert(commands.size() == 2);
  feed("\nSHS_TEST/1 led_test after_improv\n");
  assert(commands.back() == "led_test after_improv");

  // No partial, overlong, or malformed input can turn on the LEDs.
  size_t before = commands.size();
  feed("SHS_TEST/1 " + std::string(5000, 'a') + "SHS_TEST/1 led_test hidden\n");
  assert(commands.size() == before);
  feed("SHS_TEST/1 led_");
  now += 1100;
  feed("test stale\n");
  assert(commands.size() == before);
  feed("IMPROV");
  now += 1100;
  feed("SHS_TEST/1 led_test recovered\n");
  assert(commands.back() == "led_test recovered");
  feed("SHX nonsense\nSHS_TEST/1 led_test ok\n");
  assert(commands.back() == "led_test ok");

  std::string verb, request_id;
  assert(SerialMux::parse_command("led_test abc-123_", verb, request_id));
  assert(verb == "led_test" && request_id == "abc-123_");
  assert(SerialMux::parse_command("capabilities abc", verb, request_id));
  for (const auto &line : {"led_test", "led_test ", "led_test a b", "led_test \"x\"", "reboot abc"})
    assert(!SerialMux::parse_command(line, verb, request_id));
  assert(!SerialMux::parse_command("led_test " + std::string(33, 'a'), verb, request_id));
  assert(SerialMux::parse_command("led_test " + std::string(32, 'a'), verb, request_id));
}
