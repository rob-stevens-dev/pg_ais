#!/usr/bin/env python3
import sys
from pathlib import Path

TEMPLATE = """static void test_payload_{index}(void **state) {{
    (void)state;
    const char *payload = "{payload}";
    AISMessage msg = {{0}};
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, {type});
    assert_int_equal(msg.mmsi, {mmsi});
    {optional_asserts}
    free_ais_message(&msg);
}}
"""

def parse_test_lines(lines):
    tests = []
    for i, line in enumerate(lines):
        if not line.strip() or line.startswith('#'):
            continue
        parts = line.strip().split()
        if len(parts) < 3:
            continue
        msg_type, payload, mmsi = parts[:3]
        extra = parts[3:]

        optional_asserts = []
        for item in extra:
            if '=' in item:
                key, val = item.split('=', 1)
                if key == 'callsign':
                    optional_asserts.append(f'assert_string_equal(msg.callsign, "{val}");')
                elif key == 'vessel_name':
                    optional_asserts.append(f'assert_string_equal(msg.vessel_name, "{val}");')
            elif item.startswith('bin_len'):
                optional_asserts.append('assert_true(msg.bin_len > 0);')

        tests.append(TEMPLATE.format(
            index=i,
            payload=payload,
            type=int(msg_type),
            mmsi=int(mmsi),
            optional_asserts='\n    '.join(optional_asserts)
        ))
    return tests

def main():
    in_file = Path("test/ais_test_payloads.txt")
    out_file = Path("test/auto_test_payloads.c")

    if not in_file.exists():
        print(f"Missing {in_file}")
        sys.exit(1)

    lines = in_file.read_text().splitlines()
    tests = parse_test_lines(lines)
    with out_file.open('w') as f:
        f.write("/* auto_test_payloads.c */\n\n")
        f.write("#include <cmocka.h>\n")
        f.write("#include "../src/parse_ais_msg.h"\n\n")
        for test in tests:
            f.write(test + '\n')

        f.write("""int main(void) {
    const struct CMUnitTest tests[] = {
""")
        for i in range(len(tests)):
            f.write(f"        cmocka_unit_test(test_payload_{i}),\n")
        f.write("""    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
""")

if __name__ == "__main__":
    main()
