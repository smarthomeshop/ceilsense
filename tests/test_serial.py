from pathlib import Path
import json
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class SerialTests(unittest.TestCase):
    def test_led_lifecycle_and_responses(self):
        compiler = shutil.which("c++")
        if not compiler:
            self.skipTest("C++ compiler required")
        with tempfile.TemporaryDirectory() as temp:
            temp = Path(temp)
            for name in ("core/component.h", "core/hal.h", "core/log.h",
                         "components/light/light_state.h", "components/uart/uart_component.h",
                         "components/logger/logger.h"):
                header = temp / "esphome" / name
                header.parent.mkdir(parents=True, exist_ok=True)
                header.write_text('#include "serial_test_stubs.h"\n')
            (temp / "driver").mkdir()
            (temp / "driver/uart.h").write_text('#include "serial_test_stubs.h"\n')
            binary = temp / "controller-test"
            built = subprocess.run([
                compiler, "-std=c++17", "-I", str(temp), "-I", str(ROOT / "tests"),
                "-I", str(ROOT / "components/shs_test_serial"),
                str(ROOT / "tests/serial_controller_test.cpp"),
                str(ROOT / "components/shs_test_serial/shs_test_serial.cpp"),
                "-o", str(binary),
            ], capture_output=True, text=True)
            self.assertEqual(built.returncode, 0, built.stderr)
            result = subprocess.run([str(binary)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            messages = [json.loads(line.removeprefix("SHS_TEST/1 ")) for line in result.stdout.splitlines()]
            self.assertEqual(messages[0]["commands"], ["led_test"])
            self.assertEqual(messages[1]["request_id"], "discover1")
            states = [(m["request_id"], m["status"]) for m in messages if m["type"] == "manual_test"]
            self.assertEqual(states, [("one", "running"), ("one", "running"), ("other", "busy"),
                                      ("one", "complete"), ("one", "complete"), ("blocked", "busy"),
                                      ("missing", "error"), ("two", "running"), ("two", "complete")])
            self.assertTrue(all(m["requires_visual_confirmation"] for m in messages))

    def test_command_and_improv_framing(self):
        compiler = shutil.which("c++")
        if not compiler:
            self.skipTest("C++ compiler required")
        with tempfile.TemporaryDirectory() as temp:
            binary = Path(temp) / "serial-test"
            built = subprocess.run([
                compiler, "-std=c++17", "-Wall", "-Wextra", "-Werror",
                "-I", str(ROOT / "components/shs_test_serial"),
                str(ROOT / "tests/serial_mux_test.cpp"), "-o", str(binary),
            ], capture_output=True, text=True)
            self.assertEqual(built.returncode, 0, built.stderr)
            result = subprocess.run([str(binary)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
