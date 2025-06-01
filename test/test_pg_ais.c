// --- test_pg_ais.c ---
#include <stddef.h>   // for size_t
#include <stdarg.h>   // for va_list
#include <setjmp.h>   // for jmp_buf
#include <cmocka.h>
#include "../src/pg_ais.h"
#include "../src/parse_ais.h"
#include "../src/parse_ais_msg.h"


static void test_msg_1_2_3_parsing(void **state) {
    AISMessage msg;

    const char *payload_1 = "15Muq60001G?tTpE>Gbk0?wN0<0";  // Type 1
    bool ok = parse_ais_payload(&msg, payload_1, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 1);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected value
    assert_true(msg.lat != 0.0);
    assert_true(msg.lon != 0.0);
    assert_true(msg.speed >= 0);
    assert_true(msg.heading >= 0);
    assert_int_equal(msg.repeat, msg.repeat);
    assert_int_equal(msg.nav_status, msg.nav_status);
    assert_int_equal(msg.rot, msg.rot);
    assert_int_equal(msg.accuracy, msg.accuracy);
    assert_true(msg.course >= 0);
    assert_int_equal(msg.timestamp, msg.timestamp);
    assert_int_equal(msg.maneuver, msg.maneuver);
    assert_int_equal(msg.raim, msg.raim);
    assert_int_equal(msg.radio, msg.radio);
    free_ais_message(&msg);

    const char *payload_2 = "25Muq60001G?tTpE>Gbk0?wN0<0";  // Type 2
    ok = parse_ais_payload(&msg, payload_2, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 2);
    assert_int_equal(msg.mmsi, 123456789);
    free_ais_message(&msg);

    const char *payload_3 = "35Muq60001G?tTpE>Gbk0?wN0<0";  // Type 3
    ok = parse_ais_payload(&msg, payload_3, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 3);
    assert_int_equal(msg.mmsi, 123456789);
    free_ais_message(&msg);
}

static void test_msg_4_11_parsing(void **state) {
    AISMessage msg;
    const char *payload = "45Muq60001G?tTpE>Gbk0?wN0<0";  // Example payload for message 4
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 4);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_int_equal(msg.year, msg.year);  // Placeholder validation
    assert_int_equal(msg.month, msg.month);
    assert_int_equal(msg.day, msg.day);
    free_ais_message(&msg);

    const char *payload_11 = ";5Muq60001G?tTpE>Gbk0?wN0<0";  // Example payload for message 11
    ok = parse_ais_payload(&msg, payload_11, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 11);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_true(msg.lat != 0.0);
    assert_true(msg.lon != 0.0);
    free_ais_message(&msg);
}

static void test_msg_5_parsing(void **state) {
    AISMessage msg;
    const char *payload = "55Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual full message 5 payload
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 5);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_int_equal(msg.imo, msg.imo);
    assert_non_null(msg.callsign);
    assert_non_null(msg.vessel_name);
    assert_true(msg.ship_type >= 0);
    assert_true(msg.draught >= 0);
    assert_non_null(msg.destination);
    free_ais_message(&msg);
}

static void test_msg_6_parsing(void **state) {
    AISMessage msg;
    const char *payload = "65Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 6
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 6);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_true(msg.seq_num >= 0);
    assert_true(msg.dac > 0);
    assert_true(msg.fid > 0);
    assert_int_equal(msg.spare, msg.spare);
    assert_true(msg.bin_len > 0);
    free_ais_message(&msg);
}

static void test_msg_7_parsing(void **state) {
    AISMessage msg;
    const char *payload = "75Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 7
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 7);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_true(msg.seq_num >= 0);
    assert_true(msg.dac > 0);
    assert_true(msg.fid > 0);
    assert_int_equal(msg.spare, msg.spare);
    assert_true(msg.bin_len > 0);
    free_ais_message(&msg);
}

static void test_msg_8_parsing(void **state) {
    AISMessage msg;
    const char *payload = "85Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 8
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 8);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_true(msg.dac > 0);
    assert_true(msg.fid > 0);
    assert_int_equal(msg.spare, msg.spare);
    assert_true(msg.bin_len > 0);
    free_ais_message(&msg);
}

static void test_msg_9_parsing(void **state) {
    AISMessage msg;
    const char *payload = "95Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 9
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 9);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_true(msg.lat != 0.0);
    assert_true(msg.lon != 0.0);
    assert_true(msg.speed >= 0);
    assert_true(msg.heading >= 0);
    assert_int_equal(msg.alt_sensor, msg.alt_sensor);
    assert_int_equal(msg.altitude, msg.altitude);
    assert_int_equal(msg.dte, msg.dte);
    assert_int_equal(msg.radio, msg.radio);
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
    assert_int_equal(msg.spare, msg.spare);
    assert_int_equal(msg.spare2, msg.spare2);
    free_ais_message(&msg);
}

static void test_msg_12_parsing(void **state) {
    AISMessage msg;
    const char *payload = "<5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 12
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 12);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_true(msg.seq_num >= 0);
    assert_true(msg.dest_mmsi > 0);
    assert_true(msg.retransmit <= 1);
    assert_non_null(msg.bin_data);
    assert_true(msg.bin_len > 0);
    free_ais_message(&msg);
}

static void test_msg_13_parsing(void **state) {
    AISMessage msg;
    const char *payload = "=5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 13
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 13);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_int_equal(msg.spare, msg.spare);
    assert_non_null(msg.bin_data);
    assert_true(msg.bin_len > 0);
    free_ais_message(&msg);
}

static void test_msg_14_parsing(void **state) {
    AISMessage msg;
    const char *payload = ">5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 14
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 14);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_int_equal(msg.spare, msg.spare);
    assert_non_null(msg.bin_data);
    assert_true(msg.bin_len > 0);
    free_ais_message(&msg);
}

static void test_msg_15_parsing(void **state) {
    AISMessage msg;
    const char *payload = "?5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 15
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 15);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_int_equal(msg.dest_mmsi, 987654321);  // Replace with expected MMSI
    assert_true(msg.msg1_id > 0);
    assert_true(msg.msg1_offset >= 0);
    if (msg.dest2_mmsi > 0) {
        assert_true(msg.msg2_id > 0);
        assert_true(msg.msg2_offset >= 0);
    }
    free_ais_message(&msg);
}

static void test_msg_16_parsing(void **state) {
    AISMessage msg;
    const char *payload = "@5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 16
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 16);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_true(msg.dest_mmsi > 0);
    assert_true(msg.msg1_offset >= 0);
    if (msg.dest2_mmsi > 0) {
        assert_true(msg.msg2_offset >= 0);
    }
    free_ais_message(&msg);
}

static void test_msg_17_parsing(void **state) {
    AISMessage msg;
    const char *payload = "A5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 17
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 17);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_int_equal(msg.spare, msg.spare);
    assert_true(msg.bin_len > 0);
    assert_non_null(msg.bin_data);
    free_ais_message(&msg);
}

static void test_msg_18_parsing(void **state) {
    AISMessage msg;
    const char *payload = "B5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 18
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 18);
    assert_true(msg.lat != 91.0);
    assert_true(msg.lon != 181.0);
    assert_true(msg.speed >= 0);
    assert_true(msg.heading >= 0);
    free_ais_message(&msg);
}

static void test_msg_19_parsing(void **state) {
    AISMessage msg;
    const char *payload = "C5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 19
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 19);
    assert_non_null(msg.vessel_name);
    assert_non_null(msg.callsign);
    assert_true(msg.dimension_to_bow > 0);
    free_ais_message(&msg);
}

static void test_msg_20_parsing(void **state) {
    AISMessage msg;
    const char *payload = "D5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 20
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 20);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_true(msg.offset1 >= 0 && msg.offset1 <= 2240);
    assert_true(msg.num_slots1 > 0 && msg.num_slots1 <= 15);
    assert_true(msg.timeout1 >= 0);
    assert_true(msg.increment1 > 0);
    free_ais_message(&msg);
}

static void test_msg_21_parsing(void **state) {
    AISMessage msg;
    const char *payload = "E5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 21
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 21);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_non_null(msg.vessel_name);
    assert_true(strlen(msg.vessel_name) > 0);
    assert_true(msg.lat != 91.0);
    assert_true(msg.lon != 181.0);
    assert_true(msg.dimension_to_bow > 0);
    assert_true(msg.radio > 0);
    free_ais_message(&msg);
}

static void test_msg_22_parsing(void **state) {
    AISMessage msg;
    const char *payload = "F5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 22
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 22);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_true(msg.channel_a > 0);
    assert_true(msg.channel_b > 0);
    assert_true(msg.txrx_mode <= 15);
    assert_true(msg.ne_lat != 91.0);
    assert_true(msg.sw_lon != 181.0);
    free_ais_message(&msg);
}

static void test_msg_23_parsing(void **state) {
    AISMessage msg;
    const char *payload = "G5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 23
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 23);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    assert_true(msg.ne_lat != 91.0);
    assert_true(msg.sw_lon != 181.0);
    assert_true(msg.txrx_mode <= 3);
    assert_true(msg.interval <= 15);
    assert_true(msg.quiet <= 15);
    free_ais_message(&msg);
}

static void test_msg_24_parsing(void **state) {
    AISMessage msg;
    const char *payload = "H5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 24
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 24);
    assert_true(msg.callsign || msg.vessel_name);
    free_ais_message(&msg);
}

static void test_msg_25_parsing(void **state) {
    AISMessage msg;
    const char *payload = "I5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 25
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 25);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    if (msg.structured) {
        assert_true(msg.app_id != 0);
    }
    assert_true(msg.bin_len > 0);
    assert_non_null(msg.bin_data);
    free_ais_message(&msg);
}

static void test_msg_26_parsing(void **state) {
    AISMessage msg;
    const char *payload = "J5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 26
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 26);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
    if (msg.structured) {
        assert_true(msg.app_id != 0);
    }
    assert_true(msg.bin_len > 0);
    assert_non_null(msg.bin_data);
    free_ais_message(&msg);
}


static void test_msg_27_parsing(void **state) {
    AISMessage msg;
    const char *payload = "K5Muq60001G?tTpE>Gbk0?wN0<0";  // Replace with actual payload for msg 27
    bool ok = parse_ais_payload(&msg, payload, 0);
    assert_true(ok);
    assert_int_equal(msg.type, 27);
    assert_int_equal(msg.mmsi, 123456789);  // Replace with expected MMSI
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
        cmocka_unit_test(test_msg_1_2_3_parsing),
        cmocka_unit_test(test_msg_4_11_parsing),
        cmocka_unit_test(test_msg_5_parsing),
        cmocka_unit_test(test_msg_6_parsing),
        cmocka_unit_test(test_msg_7_parsing),
        cmocka_unit_test(test_msg_8_parsing),
        cmocka_unit_test(test_msg_9_parsing),

        cmocka_unit_test(test_msg_10_parsing),
        cmocka_unit_test(test_msg_12_parsing),
        cmocka_unit_test(test_msg_13_parsing),
        cmocka_unit_test(test_msg_14_parsing),
        cmocka_unit_test(test_msg_15_parsing),
        cmocka_unit_test(test_msg_16_parsing),
        cmocka_unit_test(test_msg_17_parsing),
        cmocka_unit_test(test_msg_18_parsing),
        cmocka_unit_test(test_msg_19_parsing),

        cmocka_unit_test(test_msg_20_parsing),
        cmocka_unit_test(test_msg_21_parsing),
        cmocka_unit_test(test_msg_22_parsing),
        cmocka_unit_test(test_msg_23_parsing),
        cmocka_unit_test(test_msg_24_parsing),
        cmocka_unit_test(test_msg_25_parsing),
        cmocka_unit_test(test_msg_26_parsing),
        cmocka_unit_test(test_msg_27_parsing),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}