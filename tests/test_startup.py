"""Run with the ESPHome environment: python -m unittest discover -s tests."""

import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

import yaml


ROOT = Path(__file__).resolve().parents[1] / "ceilsense-v1"


def read_config(path):
    # Keep ESPHome tags as data; this test does not fetch remote packages.
    return yaml.load(path.read_text(), Loader=yaml.BaseLoader)


class StartupTests(unittest.TestCase):
    def test_boot_lights_are_opt_in_on_both_networks(self):
        base = read_config(ROOT / "base.yaml")

        def light_actions(actions, enabled, failed):
            lights = []
            for action in actions:
                if "if" in action:
                    branch = action["if"]
                    condition = branch["condition"]
                    if condition == {"switch.is_on": "startup_led_test_enabled"}:
                        outcome = enabled
                    elif condition.get("lambda", "").strip() == "return !id(startup_failed);":
                        outcome = not failed
                    else:
                        # Other branches only check sensors/network, with no LEDs.
                        self.assertEqual(light_actions(branch.get("then", []), enabled, failed), [])
                        self.assertEqual(light_actions(branch.get("else", []), enabled, failed), [])
                        continue
                    lights.extend(light_actions(branch.get("then" if outcome else "else", []), enabled, failed))
                elif any(key.startswith("light.") for key in action):
                    lights.append(action)
            return lights

        for network in ("wifi.yaml", "eth.yaml"):
            boots = read_config(ROOT / network)["esphome"]["on_boot"] + base["esphome"]["on_boot"]
            actions = [action for boot in boots for action in boot["then"]]
            for failed in (False, True):
                with self.subTest(network=network, failed=failed):
                    self.assertEqual(light_actions(actions, False, failed), [])
                    lights = light_actions(actions, True, failed)
                    on = [action["light.turn_on"] for action in lights if "light.turn_on" in action]
                    self.assertEqual(len(on), 2)
                    self.assertEqual(on[0]["effect"], "Circular Loading Effect")
                    self.assertEqual(on[1]["green"], "0%" if failed else "100%")
                    self.assertIn("light.turn_off", lights[-1])

    def test_factory_begin_messages(self):
        package = read_config(ROOT / "packages/factory_hardware_test.yaml")
        begin = package["esphome"]["on_boot"][0]["then"][0]["lambda"]
        template = re.search(r'R"\((SHS_TEST/1 .*?)\)"', begin).group(1)
        variants = sorted(ROOT.glob("*-factory-hardware-test.yaml"))
        self.assertEqual(len(variants), 24)
        for path in variants:
            with self.subTest(variant=path.name):
                substitutions = read_config(path)["substitutions"]
                message = template
                for key, value in substitutions.items():
                    message = message.replace("${" + key + "}", value)
                payload = json.loads(message.removeprefix("SHS_TEST/1 "))
                # Leave room for the timestamp, tag, ANSI escapes and terminator.
                log_buffer_size = int(read_config(ROOT / "base.yaml")["logger"]["tx_buffer_size"])
                self.assertLess(len(message.encode()) + 80, log_buffer_size)
                self.assertEqual(payload["type"], "begin")
                self.assertEqual(payload["profile"], substitutions["factory_test_profile"])
                expected = {"firmware", "bh1750", "bmp3xx", "network", "status_led"}
                if "complete" in path.name:
                    expected.add("scd41")
                for radar in ("ld2412", "ld2450"):
                    if radar in path.name:
                        expected.add(radar)
                ids = [test["id"] for test in payload["tests"]]
                self.assertEqual(set(ids), expected)
                self.assertEqual(len(ids), len(expected))

    def test_local_led_controls_leave_boot_animation_running(self):
        compiler = shutil.which("c++")
        if compiler is None:
            self.skipTest("C++ compiler required for LED behavior checks")
        # Execute the actual interval lambdas with fake sensors and a light that
        # counts calls. The boot guard must protect both enabled and disabled LEDs.
        for path in sorted((ROOT / "packages").glob("led_local_*.yaml")):
            with self.subTest(package=path.name), tempfile.TemporaryDirectory() as temp:
                body = read_config(path)["interval"][0]["then"][0]["lambda"]
                expression = read_config(ROOT / "base.yaml")["substitutions"]["led_test_running_expression"]
                body = body.replace("${led_test_running_expression}", expression)
                names = set(re.findall(r"id\((\w+)\)", body))
                special = {"startup_complete", "local_led_runtime_state",
                           "local_co2_alert_active", "local_co2_alert_phase",
                           "local_co2_alert_until", "status_led"}
                declarations = "\n".join(f"Entity {name};" for name in sorted(names - special))
                source = r'''
#include <cassert>
#include <cmath>
#include <cstdint>
using std::isnan;
#define id(name) name
struct Time { int hour = 12; bool is_valid() const { return true; } };
struct Entity {
  float state = 0;
  bool has_state() { return true; }
  bool is_running() { return state != 0; }
  Time now() { return {}; }
  void execute(bool) {}
};
struct Light {
  int calls = 0;
  Light &turn_off() { return *this; }
  Light &turn_on() { return *this; }
  void set_brightness(float) {}
  void set_rgb(float, float, float) {}
  void set_effect(const char *) {}
  void perform() { calls++; }
};
uint32_t millis() { return 1000; }
bool startup_complete = false, local_co2_alert_active = false;
int local_led_runtime_state = -1, local_co2_alert_phase = 0;
uint32_t local_co2_alert_until = 0;
Light status_led;
''' + declarations + "\nvoid tick() {\n" + body + r'''
}
int main() {
  for (bool enabled : {false, true}) {
    local_led_enabled.state = enabled;
    for (int second = 0; second < 180; second++) tick();
    assert(status_led.calls == 0);
    assert(local_led_runtime_state == -1);
  }
  startup_complete = true;
  shs_serial.state = 1;
  tick();
  assert(status_led.calls == 0);
  shs_serial.state = 0;
  local_led_enabled.state = false;
  tick();
  assert(status_led.calls == 1);
  assert(local_led_runtime_state == 0);
  tick();
  assert(status_led.calls == 1);
}
'''
                source = "#include <initializer_list>\n" + source
                cpp = Path(temp) / "led.cpp"
                binary = Path(temp) / "led-test"
                cpp.write_text(source)
                built = subprocess.run([compiler, "-std=c++17", str(cpp), "-o", str(binary)],
                                       capture_output=True, text=True)
                self.assertEqual(built.returncode, 0, built.stderr)
                result = subprocess.run([str(binary)], capture_output=True, text=True)
                self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
