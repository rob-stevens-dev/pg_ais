// ------------------------------------------
// File: src/parse_ais_msg.c
#include "parse_ais_msg.h"
#include "bitfield.h"
#include <string.h>

bool parse_msg_1_2_3(AISMessage *msg, const char *payload) {
    msg->type = parse_uint(payload, 0, 6);
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->nav_status = parse_uint(payload, 38, 4);
    msg->rot = parse_uint(payload, 42, 8);  // Rate of turn (special handling may apply)
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

    // Validation
    if (msg->lat < -90 || msg->lat > 90) msg->lat = 91.0;
    if (msg->lon < -180 || msg->lon > 180) msg->lon = 181.0;
    if (msg->speed > 102.2) msg->speed = -1;
    if (msg->course >= 360.0) msg->course = -1;
    if (msg->heading >= 511) msg->heading = -1;
    if (msg->timestamp == 60) msg->timestamp = 255;

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

    // Validation
    if (msg->lat < -90 || msg->lat > 90) msg->lat = 91.0;
    if (msg->lon < -180 || msg->lon > 180) msg->lon = 181.0;
    if (msg->timestamp == 60) msg->timestamp = 255;
    if (msg->fix_type == 0) msg->fix_type = 255;
    if (msg->month == 0 || msg->month > 12) return false;
    if (msg->day == 0 || msg->day > 31) return false;
    if (msg->hour > 23) return false;
    if (msg->minute > 59) return false;
    if (msg->second > 59) return false;

    return true;
}

bool parse_msg_5(AISMessage *msg, const char *payload) {
    msg->type = 5;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->imo = parse_uint(payload, 40, 30);
    msg->callsign = parse_string(payload, 70, 42);
    msg->vessel_name = parse_string(payload, 112, 120);
    msg->ship_type = parse_uint(payload, 232, 8);
    msg->dimension_to_bow = parse_uint(payload, 240, 9);
    msg->dimension_to_stern = parse_uint(payload, 249, 9);
    msg->dimension_to_port = parse_uint(payload, 258, 6);
    msg->dimension_to_starboard = parse_uint(payload, 264, 6);
    msg->fix_type = parse_uint(payload, 270, 4);
    msg->eta_month = parse_uint(payload, 274, 4);
    msg->eta_day = parse_uint(payload, 278, 5);
    msg->eta_hour = parse_uint(payload, 283, 5);
    msg->eta_minute = parse_uint(payload, 288, 6);
    msg->draught = parse_uint(payload, 294, 8) / 10.0;
    msg->destination = parse_string(payload, 302, 120);

    // Validation
    if (msg->eta_month < 1 || msg->eta_month > 12) return false;
    if (msg->eta_day < 1 || msg->eta_day > 31) return false;
    if (msg->eta_hour > 23) return false;
    if (msg->eta_minute > 59) return false;
    if (!msg->vessel_name || strlen(msg->vessel_name) == 0) return false;
    if (!msg->destination || strlen(msg->destination) == 0) return false;
    if (!msg->callsign || strlen(msg->callsign) == 0) return false;
    if (msg->draught > 25.5) return false;

    return true;
}

bool parse_msg_6(AISMessage *msg, const char *payload) {
    msg->type = 6;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->seq_num = parse_uint(payload, 38, 2);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    msg->retransmit = parse_uint(payload, 70, 1);
    msg->spare = parse_uint(payload, 71, 1);
    msg->dac = parse_uint(payload, 72, 10);
    msg->fid = parse_uint(payload, 82, 6);

    int bin_start = 88;
    int bin_len = (int)(strlen(payload) * 6) - bin_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (!msg->bin_data || bin_bytes <= 0) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, bin_start + i * 8, 8);
    }

    // Validation
    if (msg->dac == 0 || msg->fid == 0) return false;
    if (msg->bin_len == 0 || msg->bin_data == NULL) return false;

    return true;
}

bool parse_msg_7(AISMessage *msg, const char *payload) {
    msg->type = 7;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->seq_num = parse_uint(payload, 38, 2);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    msg->retransmit = parse_uint(payload, 70, 1);
    msg->spare = parse_uint(payload, 71, 1);
    msg->dac = parse_uint(payload, 72, 10);
    msg->fid = parse_uint(payload, 82, 6);

    int bin_start = 88;
    int bin_len = (int)(strlen(payload) * 6) - bin_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (!msg->bin_data || bin_bytes <= 0) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, bin_start + i * 8, 8);
    }

    // Validation
    if (msg->dac == 0 || msg->fid == 0) return false;
    if (msg->bin_len == 0 || msg->bin_data == NULL) return false;

    return true;
}

bool parse_msg_8(AISMessage *msg, const char *payload) {
    msg->type = 8;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->spare = parse_uint(payload, 38, 2);
    msg->dac = parse_uint(payload, 40, 10);
    msg->fid = parse_uint(payload, 50, 6);
    msg->app_id = (msg->dac << 6) | msg->fid;

    int bin_start = 56;
    int bin_len = (int)(strlen(payload) * 6) - bin_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (!msg->bin_data || bin_bytes <= 0) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, bin_start + i * 8, 8);
    }

    // Validation
    if (msg->dac == 0 || msg->fid == 0 || msg->app_id == 0) return false;
    if (msg->bin_len == 0 || msg->bin_data == NULL) return false;

    return true;
}

bool parse_msg_9(AISMessage *msg, const char *payload) {
    msg->type = 9;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->altitude = parse_uint(payload, 38, 12);
    msg->speed = parse_speed(payload, 50);
    msg->accuracy = parse_uint(payload, 60, 1);
    msg->lon = parse_lon(payload, 61);
    msg->lat = parse_lat(payload, 89);
    msg->course = parse_uint(payload, 116, 12) / 10.0;
    msg->heading = parse_heading(payload, 128);
    msg->timestamp = parse_uint(payload, 137, 6);
    msg->alt_sensor = parse_uint(payload, 143, 1);
    msg->spare = parse_uint(payload, 144, 7);
    msg->dte = parse_uint(payload, 151, 1);
    msg->raim = parse_uint(payload, 152, 1);
    msg->radio = parse_uint(payload, 153, 19);

    // Validation
    if (msg->lat < -90 || msg->lat > 90) msg->lat = 91.0;
    if (msg->lon < -180 || msg->lon > 180) msg->lon = 181.0;
    if (msg->speed > 102.2) msg->speed = -1;
    if (msg->course >= 360.0) msg->course = -1;
    if (msg->heading >= 511) msg->heading = -1;
    if (msg->timestamp == 60) msg->timestamp = 255;
    if (msg->altitude == 4095) msg->altitude = -1;  // 4095 = not available

    return true;
}

bool parse_msg_10(AISMessage *msg, const char *payload) {
    msg->type = 10;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->spare = parse_uint(payload, 38, 2);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    msg->spare2 = parse_uint(payload, 70, 2);

    // Validation
    if (msg->dest_mmsi == 0 || msg->dest_mmsi > 999999999) return false;
    return true;
}


bool parse_msg_12(AISMessage *msg, const char *payload) {
    msg->type = 12;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->seq_num = parse_uint(payload, 38, 2);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    msg->retransmit = parse_uint(payload, 70, 1);
    msg->spare = parse_uint(payload, 71, 1);

    int text_start = 72;
    int bit_len = (int)(strlen(payload) * 6) - text_start;
    msg->bin_len = bit_len / 6;
    msg->bin_data = parse_string(payload, text_start, bit_len);  // 6-bit ASCII safety text

    return true;
}

bool parse_msg_13(AISMessage *msg, const char *payload) {
    msg->type = 13;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->spare = parse_uint(payload, 38, 2);

    int text_start = 40;
    int bit_len = (int)(strlen(payload) * 6) - text_start;
    msg->bin_len = bit_len / 6;
    msg->bin_data = parse_string(payload, text_start, bit_len);  // 6-bit ASCII broadcast safety message

    return true;
}

bool parse_msg_14(AISMessage *msg, const char *payload) {
    msg->type = 14;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->spare = parse_uint(payload, 38, 2);

    int text_start = 40;
    int bit_len = (int)(strlen(payload) * 6) - text_start;
    msg->bin_len = bit_len / 6;
    msg->bin_data = parse_string(payload, text_start, bit_len);  // 6-bit ASCII safety-related acknowledgement text

    return true;
}

bool parse_msg_15(AISMessage *msg, const char *payload) {
    msg->type = 15;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    msg->msg1_id = parse_uint(payload, 70, 6);
    msg->msg1_offset = parse_uint(payload, 76, 12);

    if ((int)(strlen(payload) * 6) > 88) {
        msg->dest2_mmsi = parse_uint(payload, 88, 30);
        msg->msg2_id = parse_uint(payload, 118, 6);
        msg->msg2_offset = parse_uint(payload, 124, 12);
    } else {
        msg->dest2_mmsi = 0;
        msg->msg2_id = 0;
        msg->msg2_offset = 0;
    }

    return true;
}

bool parse_msg_16(AISMessage *msg, const char *payload) {
    msg->type = 16;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->dest_mmsi = parse_uint(payload, 40, 30);
    msg->msg1_offset = parse_uint(payload, 70, 12);

    if ((int)(strlen(payload) * 6) > 82) {
        msg->dest2_mmsi = parse_uint(payload, 82, 30);
        msg->msg2_offset = parse_uint(payload, 112, 12);
    } else {
        msg->dest2_mmsi = 0;
        msg->msg2_offset = 0;
    }

    return true;
}

bool parse_msg_17(AISMessage *msg, const char *payload) {
    msg->type = 17;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->spare = parse_uint(payload, 38, 1);

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

bool parse_msg_18_19_24(AISMessage *msg, const char *payload) {
    msg->type = parse_uint(payload, 0, 6);
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->speed = parse_speed(payload, 46);
    msg->accuracy = parse_uint(payload, 56, 1);
    msg->lon = parse_lon(payload, 57);
    msg->lat = parse_lat(payload, 85);
    msg->course = parse_uint(payload, 112, 12) / 10.0;
    msg->heading = parse_heading(payload, 124);
    msg->timestamp = parse_uint(payload, 133, 6);
    msg->raim = parse_uint(payload, 148, 1);
    msg->radio = parse_uint(payload, 149, 19);

    // Validation rules
    if (msg->lat < -90 || msg->lat > 90) msg->lat = 91.0;
    if (msg->lon < -180 || msg->lon > 180) msg->lon = 181.0;
    if (msg->speed > 102.2) msg->speed = -1;
    if (msg->course >= 360.0) msg->course = -1;
    if (msg->heading >= 511) msg->heading = -1;
    if (msg->timestamp == 60) msg->timestamp = 255;  // 255 indicates invalid timestamp

    if (msg->type == 19) {
        msg->ship_type = parse_uint(payload, 143, 8);
        msg->callsign = parse_string(payload, 151, 42);
        msg->vessel_name = parse_string(payload, 193, 120);
        msg->dimension_to_bow = parse_uint(payload, 313, 9);
        msg->dimension_to_stern = parse_uint(payload, 322, 9);
        msg->dimension_to_port = parse_uint(payload, 331, 6);
        msg->dimension_to_starboard = parse_uint(payload, 337, 6);
        msg->fix_type = parse_uint(payload, 343, 4);
    } else if (msg->type == 24) {
        int part = parse_uint(payload, 38, 2);
        if (part == 0) {
            msg->vessel_name = parse_string(payload, 40, 120);
        } else if (part == 1) {
            msg->callsign = parse_string(payload, 40, 42);
            msg->ship_type = parse_uint(payload, 82, 8);
            msg->dimension_to_bow = parse_uint(payload, 90, 9);
            msg->dimension_to_stern = parse_uint(payload, 99, 9);
            msg->dimension_to_port = parse_uint(payload, 108, 6);
            msg->dimension_to_starboard = parse_uint(payload, 114, 6);
            msg->fix_type = parse_uint(payload, 120, 4);
        }
    }

    return true;
}

bool parse_msg_20(AISMessage *msg, const char *payload) {
    msg->type = 20;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);

    msg->offset1 = parse_uint(payload, 40, 12);
    msg->num_slots1 = parse_uint(payload, 52, 4);
    msg->timeout1 = parse_uint(payload, 56, 3);
    msg->increment1 = parse_uint(payload, 59, 11);

    if ((int)(strlen(payload) * 6) > 70) {
        msg->offset2 = parse_uint(payload, 70, 12);
        msg->num_slots2 = parse_uint(payload, 82, 4);
        msg->timeout2 = parse_uint(payload, 86, 3);
        msg->increment2 = parse_uint(payload, 89, 11);
    }
    if ((int)(strlen(payload) * 6) > 100) {
        msg->offset3 = parse_uint(payload, 100, 12);
        msg->num_slots3 = parse_uint(payload, 112, 4);
        msg->timeout3 = parse_uint(payload, 116, 3);
        msg->increment3 = parse_uint(payload, 119, 11);
    }
    if ((int)(strlen(payload) * 6) > 130) {
        msg->offset4 = parse_uint(payload, 130, 12);
        msg->num_slots4 = parse_uint(payload, 142, 4);
        msg->timeout4 = parse_uint(payload, 146, 3);
        msg->increment4 = parse_uint(payload, 149, 11);
    }

    // Validation
    if (msg->num_slots1 == 0 || msg->increment1 == 0 || msg->offset1 > 2240 || msg->num_slots1 > 15)
        return false;

    return true;
}

bool parse_msg_21(AISMessage *msg, const char *payload) {
    msg->type = 21;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->aid_type = parse_uint(payload, 38, 5);
    msg->vessel_name = parse_string(payload, 43, 120);
    msg->accuracy = parse_uint(payload, 163, 1);
    msg->lon = parse_lon(payload, 164);
    msg->lat = parse_lat(payload, 192);
    msg->dimension_to_bow = parse_uint(payload, 219, 9);
    msg->dimension_to_stern = parse_uint(payload, 228, 9);
    msg->dimension_to_port = parse_uint(payload, 237, 6);
    msg->dimension_to_starboard = parse_uint(payload, 243, 6);
    msg->fix_type = parse_uint(payload, 249, 4);
    msg->timestamp = parse_uint(payload, 253, 6);
    msg->off_position = parse_uint(payload, 259, 1);
    msg->raim = parse_uint(payload, 268, 1);
    msg->virtual_aid = parse_uint(payload, 269, 1);
    msg->assigned = parse_uint(payload, 270, 1);
    msg->spare = parse_uint(payload, 271, 1);
    msg->radio = parse_uint(payload, 272, 19);

    // Validation
    if (msg->lat < -90 || msg->lat > 90) msg->lat = 91.0;
    if (msg->lon < -180 || msg->lon > 180) msg->lon = 181.0;
    if (msg->timestamp == 60) msg->timestamp = 255;
    if (msg->fix_type == 0) msg->fix_type = 255;
    if (msg->aid_type == 0 || msg->aid_type > 31) return false;
    if (!msg->vessel_name || strlen(msg->vessel_name) == 0) return false;

    return true;
}

bool parse_msg_22(AISMessage *msg, const char *payload) {
    msg->type = 22;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->channel_a = parse_uint(payload, 40, 12);
    msg->channel_b = parse_uint(payload, 52, 12);
    msg->txrx_mode = parse_uint(payload, 64, 4);
    msg->power = parse_uint(payload, 68, 1);
    msg->ne_lon = parse_lon(payload, 69);
    msg->ne_lat = parse_lat(payload, 87);
    msg->sw_lon = parse_lon(payload, 105);
    msg->sw_lat = parse_lat(payload, 123);
    msg->addressed = parse_uint(payload, 141, 1);
    msg->bandwidth_a = parse_uint(payload, 142, 1);
    msg->bandwidth_b = parse_uint(payload, 143, 1);
    msg->zone_size = parse_uint(payload, 144, 3);
    msg->spare = parse_uint(payload, 147, 1);

    // Validation
    if (msg->channel_a == 0 || msg->channel_b == 0) return false;
    if (msg->txrx_mode > 15) return false;
    if (msg->ne_lat < -90 || msg->ne_lat > 90) msg->ne_lat = 91.0;
    if (msg->ne_lon < -180 || msg->ne_lon > 180) msg->ne_lon = 181.0;
    if (msg->sw_lat < -90 || msg->sw_lat > 90) msg->sw_lat = 91.0;
    if (msg->sw_lon < -180 || msg->sw_lon > 180) msg->sw_lon = 181.0;

    return true;
}

bool parse_msg_23(AISMessage *msg, const char *payload) {
    msg->type = 23;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->ne_lon = parse_lon(payload, 40);
    msg->ne_lat = parse_lat(payload, 58);
    msg->sw_lon = parse_lon(payload, 76);
    msg->sw_lat = parse_lat(payload, 94);
    msg->station_type = parse_uint(payload, 112, 4);
    msg->type_of_ship = parse_uint(payload, 116, 8);
    msg->txrx_mode = parse_uint(payload, 124, 2);
    msg->interval = parse_uint(payload, 126, 4);
    msg->quiet = parse_uint(payload, 130, 4);
    msg->spare = parse_uint(payload, 134, 6);

    // Validation
    if (msg->ne_lat < -90 || msg->ne_lat > 90) msg->ne_lat = 91.0;
    if (msg->ne_lon < -180 || msg->ne_lon > 180) msg->ne_lon = 181.0;
    if (msg->sw_lat < -90 || msg->sw_lat > 90) msg->sw_lat = 91.0;
    if (msg->sw_lon < -180 || msg->sw_lon > 180) msg->sw_lon = 181.0;
    if (msg->txrx_mode > 3) return false;
    if (msg->interval > 15) return false;
    if (msg->quiet > 15) return false;

    return true;
}

bool parse_msg_25(AISMessage *msg, const char *payload) {
    msg->type = 25;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->addressed = parse_uint(payload, 38, 1);
    msg->structured = parse_uint(payload, 39, 1);

    int app_id_start = 40;
    if (msg->addressed) {
        msg->dest_mmsi = parse_uint(payload, 40, 30);
        app_id_start += 30;
    }
    if (msg->structured) {
        msg->app_id = parse_uint(payload, app_id_start, 16);
        app_id_start += 16;
    }

    int bin_len = (int)(strlen(payload) * 6) - app_id_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (!msg->bin_data) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, app_id_start + i * 8, 8);
    }

    // Validation
    if (msg->bin_len == 0 || msg->bin_data == NULL) return false;
    if (msg->structured && msg->app_id == 0) return false;

    return true;
}

bool parse_msg_26(AISMessage *msg, const char *payload) {
    msg->type = 26;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->addressed = parse_uint(payload, 38, 1);
    msg->structured = parse_uint(payload, 39, 1);

    int app_id_start = 40;
    if (msg->addressed) {
        msg->dest_mmsi = parse_uint(payload, 40, 30);
        msg->seq_num = parse_uint(payload, 70, 2);
        app_id_start += 32;
    }
    if (msg->structured) {
        msg->app_id = parse_uint(payload, app_id_start, 16);
        app_id_start += 16;
    }

    int bin_len = (int)(strlen(payload) * 6) - app_id_start;
    int bin_bytes = bin_len / 8;
    msg->bin_len = bin_bytes;
    msg->bin_data = malloc(bin_bytes);
    if (!msg->bin_data) return false;
    for (int i = 0; i < bin_bytes; i++) {
        msg->bin_data[i] = (char)parse_uint(payload, app_id_start + i * 8, 8);
    }

    // Validation
    if (msg->bin_len == 0 || msg->bin_data == NULL) return false;
    if (msg->structured && msg->app_id == 0) return false;

    return true;
}

bool parse_msg_27(AISMessage *msg, const char *payload) {
    msg->type = 27;
    msg->repeat = parse_uint(payload, 6, 2);
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->accuracy = parse_uint(payload, 38, 1);
    msg->raim = parse_uint(payload, 39, 1);
    msg->status = parse_uint(payload, 40, 4);
    msg->lon = parse_lon(payload, 44);
    msg->lat = parse_lat(payload, 62);
    msg->speed = parse_speed(payload, 79);
    msg->course = parse_uint(payload, 85, 9) / 10.0;
    msg->gnss = parse_uint(payload, 94, 1);

    // Validation
    if (msg->lat < -90 || msg->lat > 90) msg->lat = 91.0;
    if (msg->lon < -180 || msg->lon > 180) msg->lon = 181.0;
    if (msg->speed > 102.2) msg->speed = -1;
    if (msg->course >= 360.0) msg->course = -1;

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

        case 18:
        case 19:
        case 24: return parse_msg_18_19_24(msg, payload);

        case 20: return parse_msg_20(msg, payload);
        case 21: return parse_msg_21(msg, payload);
        case 22: return parse_msg_22(msg, payload);
        case 23: return parse_msg_23(msg, payload);
        case 25: return parse_msg_25(msg, payload);
        case 26: return parse_msg_26(msg, payload);
        case 27: return parse_msg_27(msg, payload);

        default: return false;
    }
}
