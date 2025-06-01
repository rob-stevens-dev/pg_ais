// ------------------------------------------
// File: src/parse_ais_msg.c
#include "parse_ais_msg.h"
#include "bitfield.h"
#include <string.h>

bool parse_msg_1_2_3(AISMessage *msg, const char *payload) {
    msg->type = 1;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->repeat = parse_uint(payload, 6, 2);
    msg->nav_status = parse_uint(payload, 38, 4);
    msg->rot = parse_uint(payload, 42, 8);  // Rate of turn
    msg->speed = parse_speed(payload, 50);
    msg->accuracy = parse_uint(payload, 60, 1);
    msg->lon = parse_lon(payload, 61);
    msg->lat = parse_lat(payload, 89);
    msg->course = parse_uint(payload, 116, 12) / 10.0;
    msg->heading = parse_heading(payload, 128);
    msg->timestamp = parse_uint(payload, 137, 6);
    msg->maneuver = parse_uint(payload, 143, 2);
    msg->raim = parse_uint(payload, 148, 1);
    msg->radio = parse_uint(payload, 149, 19);
    return true;
}

bool parse_msg_4_11(AISMessage *msg, const char *payload) {
    msg->type = parse_uint(payload, 0, 6);
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);

    msg->year = parse_uint(payload, 38, 14);
    msg->month = parse_uint(payload, 52, 4);
    msg->day = parse_uint(payload, 56, 5);
    msg->hour = parse_uint(payload, 61, 5);
    msg->minute = parse_uint(payload, 66, 6);
    msg->second = parse_uint(payload, 72, 6);
    msg->accuracy = parse_uint(payload, 78, 1);
    msg->lon = parse_lon(payload, 79);
    msg->lat = parse_lat(payload, 107);
    msg->timestamp = parse_uint(payload, 137, 6);
    msg->maneuver = parse_uint(payload, 143, 2);
    msg->fix_type = parse_uint(payload, 143, 4);
    msg->raim = parse_uint(payload, 147, 1);
    msg->radio = parse_uint(payload, 148, 19);
    return true;
}

bool parse_msg_5(AISMessage *msg, const char *payload) {
    msg->type = 5;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->imo = parse_uint(payload, 40, 30);
    msg->callsign = parse_string(payload, 70, 42);
    msg->vessel_name = parse_string(payload, 112, 120);
    return true;
}

bool parse_msg_6(AISMessage *msg, const char *payload) {
    msg->type = 6;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->seq_num = parse_uint(payload, 38, 2);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    msg->retransmit = parse_uint(payload, 70, 1);
    msg->app_id = parse_uint(payload, 72, 16);
    int bin_start = 88;
    int bin_len = (int)(strlen(payload) * 6) - bin_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (msg->bin_data == NULL) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, bin_start + i * 8, 8);
    }
    return true;
}

bool parse_msg_7(AISMessage *msg, const char *payload) {
    msg->type = 7;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->seq_num = parse_uint(payload, 38, 2);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    msg->retransmit = parse_uint(payload, 70, 1);
    msg->app_id = parse_uint(payload, 72, 16);
    int bin_start = 88;
    int bin_len = (int)(strlen(payload) * 6) - bin_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (msg->bin_data == NULL) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, bin_start + i * 8, 8);
    }
    return true;
}

bool parse_msg_8(AISMessage *msg, const char *payload) {
    msg->type = 8;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->app_id = parse_uint(payload, 40, 16);
    int bin_start = 56;
    int bin_len = (int)(strlen(payload) * 6) - bin_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (msg->bin_data == NULL) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, bin_start + i * 8, 8);
    }
    return true;
}

bool parse_msg_9(AISMessage *msg, const char *payload) {
    msg->type = 9;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->altitude = parse_uint(payload, 38, 12);
    msg->speed = parse_speed(payload, 50);
    msg->lat = parse_lat(payload, 61);
    msg->lon = parse_lon(payload, 89);
    msg->heading = parse_heading(payload, 128);
    return true;
}

bool parse_msg_10(AISMessage *msg, const char *payload) {
    msg->type = 10;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    return true;
}

bool parse_msg_12(AISMessage *msg, const char *payload) {
    msg->type = 12;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->seq_num = parse_uint(payload, 38, 2);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    int text_start = 72;
    int bit_len = (int)(strlen(payload) * 6) - text_start;
    msg->bin_len = bit_len / 6;
    msg->bin_data = parse_string(payload, text_start, bit_len);  // Reuse for 6-bit ASCII
    return true;
}

bool parse_msg_13(AISMessage *msg, const char *payload) {
    msg->type = 13;
    msg->mmsi = parse_uint(payload, 8, 30);
    int text_start = 40;
    int bit_len = (int)(strlen(payload) * 6) - text_start;
    msg->bin_len = bit_len / 6;
    msg->bin_data = parse_string(payload, text_start, bit_len);  // Reuse for 6-bit ASCII
    return true;
}

bool parse_msg_14(AISMessage *msg, const char *payload) {
    msg->type = 14;
    msg->mmsi = parse_uint(payload, 8, 30);
    int text_start = 40;
    int bit_len = (int)(strlen(payload) * 6) - text_start;
    msg->bin_len = bit_len / 6;
    msg->bin_data = parse_string(payload, text_start, bit_len);
    return true;
}

bool parse_msg_15(AISMessage *msg, const char *payload) {
    msg->type = 15;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    msg->app_id = parse_uint(payload, 70, 10);  // first interrogation's message ID (6) + offset (4)
    // Additional interrogations (if present) not handled here
    return true;
}

bool parse_msg_16(AISMessage *msg, const char *payload) {
    msg->type = 16;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    msg->app_id = parse_uint(payload, 70, 12);  // offset1 (12 bits)
    // Additional destination/offset sets not implemented here
    return true;
}

bool parse_msg_17(AISMessage *msg, const char *payload) {
    msg->type = 17;
    msg->mmsi = parse_uint(payload, 8, 30);
    int bin_start = 40;
    int bin_len = (int)(strlen(payload) * 6) - bin_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (!msg->bin_data) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, bin_start + i * 8, 8);
    }
    return true;
}

bool parse_msg_18(AISMessage *msg, const char *payload) {
    msg->type = 18;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->speed = parse_speed(payload, 46);
    msg->lat = parse_lat(payload, 85);
    msg->lon = parse_lon(payload, 57);
    msg->heading = parse_heading(payload, 124);
    return true;
}

bool parse_msg_19(AISMessage *msg, const char *payload) {
    msg->type = 19;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->speed = parse_speed(payload, 46);
    msg->lat = parse_lat(payload, 85);
    msg->lon = parse_lon(payload, 57);
    msg->heading = parse_heading(payload, 124);
    msg->vessel_name = parse_string(payload, 143, 120);  // 20 chars * 6 bits
    return true;
}

bool parse_msg_20(AISMessage *msg, const char *payload) {
    msg->type = 20;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->app_id = parse_uint(payload, 40, 12);  // offset1 (12 bits, example placeholder)
    // Slot assignments can be parsed here if needed
    return true;
}

bool parse_msg_21(AISMessage *msg, const char *payload) {
    msg->type = 21;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->lat = parse_lat(payload, 85);
    msg->lon = parse_lon(payload, 57);
    msg->vessel_name = parse_string(payload, 112, 120);  // name of aid-to-navigation
    return true;
}

bool parse_msg_22(AISMessage *msg, const char *payload) {
    msg->type = 22;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->app_id = parse_uint(payload, 40, 22);  // start slot or channel info placeholder
    return true;
}

bool parse_msg_23(AISMessage *msg, const char *payload) {
    msg->type = 23;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->app_id = parse_uint(payload, 40, 12);  // placeholder for NE/SE slot time info
    return true;
}

bool parse_msg_24(AISMessage *msg, const char *payload) {
    msg->type = 24;
    msg->mmsi = parse_uint(payload, 8, 30);
    int part = parse_uint(payload, 38, 2);
    if (part == 0) {
        msg->vessel_name = parse_string(payload, 40, 120);  // 20 characters
    } else if (part == 1) {
        msg->callsign = parse_string(payload, 40, 42);      // 7 characters
        // Additional fields like ship type, dimensions, etc. could be added here
    }
    return true;
}

bool parse_msg_25(AISMessage *msg, const char *payload) {
    msg->type = 25;
    msg->mmsi = parse_uint(payload, 8, 30);
    int bin_start = 40;
    int bin_len = (int)(strlen(payload) * 6) - bin_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (!msg->bin_data) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, bin_start + i * 8, 8);
    }
    return true;
}

bool parse_msg_26(AISMessage *msg, const char *payload) {
    msg->type = 26;
    msg->mmsi = parse_uint(payload, 8, 30);
    int bin_start = 40;
    int bin_len = (int)(strlen(payload) * 6) - bin_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (!msg->bin_data) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, bin_start + i * 8, 8);
    }
    return true;
}

bool parse_msg_27(AISMessage *msg, const char *payload) {
    msg->type = 27;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->lat = parse_lat(payload, 85);
    msg->lon = parse_lon(payload, 61);
    return true;
}

bool parse_ais_payload(AISMessage *msg, const char *payload, int fill_bits) {
    if (!payload || strlen(payload) < 1) return false;
    int msg_type = parse_uint(payload, 0, 6);
    switch (msg_type) {
        case 1:
        case 2:
        case 3: return parse_msg_1_2_3(msg, payload);
        case 4:
        case 11: return parse_msg_4(msg, payload);
        case 5: return parse_msg_5(msg, payload);
        case 6: return parse_msg_6(msg, payload);
        case 7: return parse_msg_7(msg, payload);
        case 8: return parse_msg_8(msg, payload);
        case 9: return parse_msg_9(msg, payload);
        
        case 10: return parse_msg_10(msg, payload);
        case 12: return parse_msg_12(msg, payload);
        case 13: return parse_msg_13(msg, payload);
        case 14: return parse_msg_14(msg, payload);
        case 15: return parse_msg_15(msg, payload);
        case 16: return parse_msg_16(msg, payload);
        case 17: return parse_msg_17(msg, payload);
        case 18: return parse_msg_18(msg, payload);
        case 19: return parse_msg_19(msg, payload);

        case 20: return parse_msg_20(msg, payload);
        case 21: return parse_msg_21(msg, payload);
        case 22: return parse_msg_22(msg, payload);
        case 23: return parse_msg_23(msg, payload);
        case 24: return parse_msg_24(msg, payload);
        case 25: return parse_msg_25(msg, payload);
        case 26: return parse_msg_26(msg, payload);
        case 27: return parse_msg_27(msg, payload);

        default: return false;
    }
}
