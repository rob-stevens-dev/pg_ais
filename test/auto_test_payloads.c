/* auto_test_payloads.c */

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "../src/parse_ais_msg.h"

#include <cmocka.h>
#include "../src/parse_ais_msg.h"

static void test_payload_2(void **state) {
    (void)state;
    const char *payload = "!AIVDM,1,1,,A,13aG?P0P00PD;88MD5MTDww@2D0T,0*1C";
    AISMessage msg = {0};
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 1);
    assert_int_equal(msg.mmsi, 366053209);
    
    free_ais_message(&msg);
}
#include <cmocka.h>
#include "../src/parse_ais_msg.h"

static void test_payload_3(void **state) {
    (void)state;
    const char *payload = "!AIVDM,1,1,,B,55NBsv02>tNDBL@E,0*1C";
    AISMessage msg = {0};
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 5);
    assert_int_equal(msg.mmsi, 123456789);
    assert_string_equal(msg.callsign, "CALL");
    assert_string_equal(msg.vessel_name, "TEST");
    free_ais_message(&msg);
}
#include <cmocka.h>
#include "../src/parse_ais_msg.h"

static void test_payload_4(void **state) {
    (void)state;
    const char *payload = "!AIVDM,1,1,,B,64aG@PP000PD;88MD5MTDww@2D0T,0*1C";
    AISMessage msg = {0};
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 6);
    assert_int_equal(msg.mmsi, 123456789);
    assert_true(msg.bin_len > 0);
    free_ais_message(&msg);
}


int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_payload_2),cmocka_unit_test(test_payload_3),cmocka_unit_test(test_payload_4),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
