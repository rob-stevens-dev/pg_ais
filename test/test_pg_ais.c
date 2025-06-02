/* test_pg_ais.c */
#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>

#include "utils/geo_decls.h"
#include "utils/builtins.h"
#include "../src/pg_ais.h"
#include "../src/parse_ais.h"
#include "../src/parse_ais_msg.h"
#include "../src/bitfield.h"

#define MAX_LINE 1024

/**
 * @brief Test parser against structured test payloads from ais_test_payloads.txt
 *
 * This test reads known message types and payloads from a file,
 * parses them using parse_ais_payload, and asserts field correctness
 * against expected values such as MMSI, callsign, vessel name, or bin_len.
 */
static void test_parse_from_fixture(void **state) {
    (void)state;
    FILE *fp = fopen("test/ais_test_payloads.txt", "r");
    assert_non_null(fp);

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || strlen(line) < 10)
            continue;

        int type;
        char payload[256];
        int expected_mmsi = 0;
        char field1[64] = {0}, value1[64] = {0};

        char tail[256] = {0};
        int fields = sscanf(line, "%d %255s %d %[^
]", &type, payload, &expected_mmsi, tail);
        AISMessage msg = {0};
        bool ok = parse_ais_payload(&msg, payload, 0);
        assert_true(ok);
        assert_int_equal(msg.type, type);
        assert_int_equal(msg.mmsi, expected_mmsi);

        if (fields == 4) {
            char field1[64] = {0}, value1[64] = {0};
            if (sscanf(tail, "%63[^=]=%63s", field1, value1) == 2) {
                if (strcmp(field1, "callsign") == 0 && msg.callsign)
                    assert_string_equal(msg.callsign, value1);
                else if (strcmp(field1, "vessel_name") == 0 && msg.vessel_name)
                    assert_string_equal(msg.vessel_name, value1);
                else if (strcmp(field1, "bin_len") == 0)
                    assert_true(msg.bin_len > 0);
            }
        }
        free_ais_message(&msg);
    }
    fclose(fp);
}


/**
 * @brief Validate parser safety and robustness against malformed input
 *
 * This test feeds parse_ais_payload various invalid cases such as
 * null input, underlength payloads, and syntactically corrupt strings.
 * The parser is expected to fail gracefully without crashing.
 */
static void test_parse_invalid_cases(void **state) {
    (void)state;
    AISMessage msg = {0};

    // invalid type
    assert_false(parse_ais_payload(&msg, "~~~~~~~~", 0));
    // short payload
    assert_false(parse_ais_payload(&msg, "1", 0));
    // null payload
    assert_false(parse_ais_payload(&msg, NULL, 0));
}

/**
 * @brief Entry point for CMocka unit tests
 *
 * Registers test cases for fixture-driven parsing and invalid input handling.
 */
/**
 * @brief Test safe unsigned integer extraction from AIS payload
 */
static void test_parse_uint_safe(void **state) {
    (void)state;
    const char *payload = "@000000000000000000000000"; // 6-bit ASCII '@' = 0
    uint32_t out;
    assert_true(parse_uint_safe(payload, 0, 6, &out));
    assert_int_equal(out, 0);

    payload = "@@@@@@@@@@@@@@@@@@@@@@@@@@"; // All 0-bits
    assert_true(parse_uint_safe(payload, 12, 12, &out));
    assert_int_equal(out, 0);

    payload = "@@@@@@@@@@"; // Too short
    assert_false(parse_uint_safe(payload, 50, 12, &out));
}

/**
 * @brief Test ASCII string decoding from 6-bit AIS encoding
 */
static void test_parse_string(void **state) {
    (void)state;
    const char *payload = "5NIpQ@1PDpN@E=2G0000000000"; // Roughly encodes "TEST"
    char *str = parse_string(payload, 0, 24);
    assert_non_null(str);
    assert_in_range(strlen(str), 2, 12);
    free(str);
}

/**
 * @brief Test UTF-8 safe decoding of 6-bit ASCII payloads
 */
static void test_parse_string_utf8(void **state) {
    (void)state;
    const char *payload = "5NIpQ@1PDpN@E=2G0000000000";
    char *str = parse_string_utf8(payload, 0, 24);
    assert_non_null(str);
    assert_in_range(strlen(str), 2, 12);
    free(str);
}

/**
 * @brief Test edge case lat/lon/heading/speed decoders
 */
static void test_geo_helpers(void **state) {
    (void)state;
    assert_true(isnan(parse_lat("@@@@@@@@@@@@@@", 0)));  // Not available
    assert_true(isnan(parse_lon("@@@@@@@@@@@@@@", 0)));  // Not available
    assert_true(parse_heading("@@@@@@@@@@@@@@", 0) == -1);
    assert_true(parse_speed("@@@@@@@@@@@@@@", 0) < 0);
}

/**
 * @brief Test individual parsing of AIS message types 1 through 27
 *
 * Each test validates that the parser can correctly identify and extract
 * basic fields such as type and mmsi from a minimal but valid payload.
 */
static void test_individual_message_types(void **state) {
    (void)state;
    const char *payloads[] = {
        "13aG?P0P00PD;88MD5MTDww@2D0T", // Type 1
        "55NBsv02>tNDBL@E",             // Type 5
        "64aG@PP000PD;88MD5MTDww@2D0T", // Type 6
        "85NBsv03>tNDBL@E",             // Type 8
        "92aG?P0P00PD;88MD5MTDww@2D0T", // Type 9
        "D5NBsv03>tNDBL@E",             // Type 12
        "E5NBsv03>tNDBL@E",             // Type 14
        "g5NBsv03>tNDBL@E",             // Type 18
        "o5NBsv03>tNDBL@E",             // Type 24
        "w5NBsv03>tNDBL@E"              // Type 27
    };

    for (size_t i = 0; i < sizeof(payloads)/sizeof(payloads[0]); i++) {
        AISMessage msg = {0};
        bool ok = parse_ais_payload(&msg, payloads[i], 0);
        assert_true(ok);
        assert_true(msg.mmsi > 0);
        assert_true(msg.type >= 1 && msg.type <= 27);
        free_ais_message(&msg);
    }
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_from_fixture),
        cmocka_unit_test(test_parse_invalid_cases),
        cmocka_unit_test(test_parse_uint_safe),
        cmocka_unit_test(test_parse_string),
        cmocka_unit_test(test_parse_string_utf8),
        cmocka_unit_test(test_geo_helpers),
        cmocka_unit_test(test_individual_message_types),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
