// --- test_pg_ais.c ---
#include <stddef.h>   // for size_t
#include <stdarg.h>   // for va_list
#include <setjmp.h>   // for jmp_buf
#include <cmocka.h>
#include "../src/pg_ais.h"
#include "../src/parse_ais.h"
#include "../src/parse_ais_msg.h"


static void test_msg_1_parsing(void **state) {
    AISMessage msg;
    const char *payload = "15Muq60001G?tTpE>Gbk0?wN0<0";
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 1);
    assert_int_equal(msg.mmsi, 123456789); // Replace with real MMSI
    free_ais_message(&msg);
}

static void test_msg_2_parsing(void **state) {
    AISMessage msg;
    const char *payload = "25Muq60001G?tTpE>Gbk0?wN0<0";
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 2);
    assert_int_equal(msg.mmsi, 123456789); // Replace with real MMSI
    free_ais_message(&msg);
}

static void test_msg_3_parsing(void **state) {
    AISMessage msg;
    const char *payload = "35Muq60001G?tTpE>Gbk0?wN0<0";
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 3);
    assert_int_equal(msg.mmsi, 123456789); // Replace with real MMSI
    free_ais_message(&msg);
}

static void test_msg_4_parsing(void **state) {
    AISMessage msg;
    const char *payload = "45Muq60001G?tTpE>Gbk0?wN0<0";
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 4);
    assert_int_equal(msg.mmsi, 123456789); // Replace with real MMSI
    free_ais_message(&msg);
}

static void test_msg_5_parsing(void **state) {
    AISMessage msg;
    const char *payload = "55Muq60001G?tTpE>Gbk0?wN0<0";
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 5);
    assert_int_equal(msg.mmsi, 123456789); // Replace with real MMSI
    free_ais_message(&msg);
}

static void test_msg_6_parsing(void **state) {
    AISMessage msg;
    const char *payload = "65Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 6
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 6);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    // Optionally assert app_id, dest_mmsi, bin_len, etc.
    free_ais_message(&msg);
}

static void test_msg_7_parsing(void **state) {
    AISMessage msg;
    const char *payload = "75Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for type 7
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 7);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    // Optionally assert app_id, dest_mmsi, bin_len, etc.
    free_ais_message(&msg);
}

static void test_msg_8_parsing(void **state) {
    AISMessage msg;
    const char *payload = "85Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 8
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 8);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    // Optionally assert app_id, bin_len, bin_data, etc.
    free_ais_message(&msg);
}

static void test_msg_9_parsing(void **state) {
    AISMessage msg;
    const char *payload = "95Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 9
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 9);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    // Optionally assert altitude, lat/lon, speed, heading
    free_ais_message(&msg);
}

static void test_msg_10_parsing(void **state) {
    AISMessage msg;
    const char *payload = ":5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 10
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 10);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_int_equal(msg.dest_mmsi, 987654321);  // Replace with expected destination MMSI
    free_ais_message(&msg);
}

static void test_msg_11_parsing(void **state) {
    AISMessage msg;
    const char *payload = ";5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 11
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 11);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    // Optionally assert lat/lon
    free_ais_message(&msg);
}

static void test_valid_fragment_parsing(void **state) {
    const char *input = "!AIVDM,2,1,1,A,55NBsv02>tNDBL@E,0*1C";
    AISFragment frag = {0};

    (void)state;

    assert_true(parse_ais_fragment(input, &frag));
    assert_int_equal(frag.total, 2);
    assert_int_equal(frag.seq, 1);
    assert_string_equal(frag.message_id, "1");
    assert_string_equal(frag.payload, "55NBsv02>tNDBL@E");
    assert_int_equal(frag.fill_bits, 0);
    free_ais_message(&msg);
}

static void test_invalid_fragment_parsing(void **state) {
    const char *input = "INVALID,NMEA,SENTENCE";
    AISFragment frag = {0};

    (void)state;

    free_ais_message(&msg);
    assert_false(parse_ais_fragment(input, &frag));
}

static void test_successful_reassembly(void **state) {
    AISFragmentBuffer buffer = {0};

    const char *part1 = "!AIVDM,2,1,2,A,part1data,0*1C";
    const char *part2 = "!AIVDM,2,2,2,A,part2data,0*6F";

    (void)state;

    AISFragment *f1 = calloc(1, sizeof(AISFragment));
    AISFragment *f2 = calloc(1, sizeof(AISFragment));
    assert_true(parse_ais_fragment(part1, f1));
    assert_true(parse_ais_fragment(part2, f2));

    buffer.parts[0] = f1;
    buffer.parts[1] = f2;
    buffer.received = 2;

    AISMessage msg = {0};
    assert_true(try_reassemble(&buffer, &msg));

    reset_buffer(&buffer);
    free_ais_message(&msg);
}

static void test_incomplete_reassembly(void **state) {
    AISFragmentBuffer buffer = {0};
    const char *part1 = "!AIVDM,2,1,3,A,part1only,0*1C";

    (void)state;

    AISFragment *f1 = calloc(1, sizeof(AISFragment));
    assert_true(parse_ais_fragment(part1, f1));

    buffer.parts[0] = f1;
    buffer.received = 1;

    AISMessage msg = {0};
    assert_false(try_reassemble(&buffer, &msg));

    reset_buffer(&buffer);
    free_ais_message(&msg);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_valid_fragment_parsing),
        cmocka_unit_test(test_invalid_fragment_parsing),
        cmocka_unit_test(test_successful_reassembly),
        cmocka_unit_test(test_incomplete_reassembly),
        cmocka_unit_test(test_msg_1_parsing),
        cmocka_unit_test(test_msg_2_parsing),
        cmocka_unit_test(test_msg_3_parsing),
        cmocka_unit_test(test_msg_4_parsing),
        cmocka_unit_test(test_msg_5_parsing),
        cmocka_unit_test(test_msg_6_parsing),
        cmocka_unit_test(test_msg_7_parsing),
        cmocka_unit_test(test_msg_8_parsing),
        cmocka_unit_test(test_msg_9_parsing),

        cmocka_unit_test(test_msg_10_parsing),
        cmocka_unit_test(test_msg_11_parsing),
        cmocka_unit_test(test_msg_12_parsing),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}