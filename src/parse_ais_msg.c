#include "parse_ais_msg.h"
#include "bitfield.h"
#include "shared_ais_utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**
 * @brief Parses AIS message types 1, 2, and 3 (Class A position reports)
 * Populates navigation and positional data from Class A transponders.
 */
bool parse_msg_1_2_3(AISMessage *msg, const char *payload) {
    msg->type       = 1;
    msg->repeat     = parse_uint_safe(payload, 6, 2);
    msg->mmsi       = parse_uint_safe(payload, 8, 30);
    msg->nav_status = parse_uint_safe(payload, 38, 4);
    msg->rot        = parse_uint_safe(payload, 42, 8);
    msg->speed      = parse_speed(payload, 50);
    msg->accuracy   = parse_uint_safe(payload, 60, 1);
    msg->lon        = parse_lon(payload, 61);
    msg->lat        = parse_lat(payload, 89);
    msg->course     = parse_uint_safe(payload, 116, 12) / 10.0;
    msg->heading    = parse_heading(payload, 128);
    msg->timestamp  = parse_uint_safe(payload, 137, 6);
    msg->maneuver   = parse_uint_safe(payload, 143, 2);
    msg->raim       = parse_uint_safe(payload, 148, 1);
    msg->radio      = parse_uint_safe(payload, 149, 19);

    normalize_position_fields(msg);
    return true;
}

/**
 * @brief Parses AIS message types 4 and 11 (Base Station Reports)
 */
bool parse_msg_4_11(AISMessage *msg, const char *payload) {
    msg->type      = 4;
    msg->repeat    = parse_uint_safe(payload, 6, 2);
    msg->mmsi      = parse_uint_safe(payload, 8, 30);
    msg->accuracy  = parse_uint_safe(payload, 78, 1);
    msg->lon       = parse_lon(payload, 79);
    msg->lat       = parse_lat(payload, 107);
    msg->timestamp = parse_uint_safe(payload, 137, 6);
    msg->raim      = parse_uint_safe(payload, 148, 1);
    msg->radio     = parse_uint_safe(payload, 149, 19);

    normalize_position_fields(msg);
    return true;
}

/**
 * @brief Parses AIS message type 5 (Static and Voyage Related Data)
 */
bool parse_msg_5(AISMessage *msg, const char *payload) {
    msg->type = 5;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    msg->imo = parse_uint_safe(payload, 40, 30);
    msg->callsign = parse_string_utf8(payload, 70, 42);
    msg->vessel_name = parse_string_utf8(payload, 112, 120);
    return true;
}

/**
 * @brief Parses AIS message type 6 (Binary Addressed Message)
 */
bool parse_msg_6(AISMessage *msg, const char *payload) {
    msg->type = 6;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    int bin_start = 88;
    int bin_len = ((int)strlen(payload) * 6) - bin_start;
    return parse_bin_payload(payload, bin_start, bin_len, &msg->bin_data, &msg->bin_len);
}

/**
 * @brief Parses AIS message type 7 (Binary Acknowledge)
 */
bool parse_msg_7(AISMessage *msg, const char *payload) {
    msg->type = 7;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    return true;
}

/**
 * @brief Parses AIS message type 8 (Binary Broadcast Message)
 */
bool parse_msg_8(AISMessage *msg, const char *payload) {
    msg->type = 8;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    int bin_start = 56;
    int bin_len = ((int)strlen(payload) * 6) - bin_start;
    return parse_bin_payload(payload, bin_start, bin_len, &msg->bin_data, &msg->bin_len);
}

/**
 * @brief Parses AIS message type 9 (Standard SAR Aircraft Position Report)
 */
bool parse_msg_9(AISMessage *msg, const char *payload) {
    msg->type = 9;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    msg->lat = parse_lat(payload, 107);
    msg->lon = parse_lon(payload, 79);
    msg->speed = parse_speed(payload, 50);
    msg->heading = parse_heading(payload, 128);
    msg->course = parse_uint_safe(payload, 116, 12) / 10.0;
    normalize_position_fields(msg);
    return true;
}

/**
 * @brief Parses AIS message type 10 (UTC/Date Inquiry)
 */
bool parse_msg_10(AISMessage *msg, const char *payload) {
    msg->type = 10;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    return true;
}

/**
 * @brief Parses AIS message type 12 (Safety-Related Message Addressed)
 */
bool parse_msg_12(AISMessage *msg, const char *payload) {
    msg->type = 12;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    msg->vessel_name = parse_string_utf8(payload, 72, ((int)strlen(payload) * 6) - 72);
    return true;
}

/**
 * @brief Parses AIS message type 13 (Safety-Related Acknowledge)
 */
bool parse_msg_13(AISMessage *msg, const char *payload) {
    msg->type = 13;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    return true;
}

/**
 * @brief Parses AIS message type 14 (Safety-Related Broadcast Message)
 */
bool parse_msg_14(AISMessage *msg, const char *payload) {
    msg->type = 14;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    msg->vessel_name = parse_string_utf8(payload, 40, ((int)strlen(payload) * 6) - 40);
    return true;
}

/**
 * @brief Parses AIS message type 15 (Interrogation)
 */
bool parse_msg_15(AISMessage *msg, const char *payload) {
    msg->type = 15;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    return true;
}

/**
 * @brief Parses AIS message type 16 (Assigned Mode Command)
 */
bool parse_msg_16(AISMessage *msg, const char *payload) {
    msg->type = 16;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    return true;
}

/**
 * @brief Parses AIS message type 17 (DGNSS Broadcast Binary Message)
 */
bool parse_msg_17(AISMessage *msg, const char *payload) {
    msg->type = 17;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    int bin_start = 80;
    int bin_len = ((int)strlen(payload) * 6) - bin_start;
    return parse_bin_payload(payload, bin_start, bin_len, &msg->bin_data, &msg->bin_len);
}

/**
 * @brief Parses AIS message types 18, 19, 24 (Class B Position Reports)
 */
bool parse_msg_18_19_24(AISMessage *msg, const char *payload) {
    msg->type      = 18;
    msg->repeat    = parse_uint_safe(payload, 6, 2);
    msg->mmsi      = parse_uint_safe(payload, 8, 30);
    msg->speed     = parse_speed(payload, 46);
    msg->accuracy  = parse_uint_safe(payload, 56, 1);
    msg->lon       = parse_lon(payload, 57);
    msg->lat       = parse_lat(payload, 85);
    msg->course    = parse_uint_safe(payload, 112, 12) / 10.0;
    msg->heading   = parse_heading(payload, 128);
    msg->timestamp = parse_uint_safe(payload, 134, 6);
    msg->raim      = parse_uint_safe(payload, 148, 1);
    msg->radio     = parse_uint_safe(payload, 149, 19);

    normalize_position_fields(msg);
    return true;
}

/**
 * @brief Parses AIS message type 20 (Data Link Management)
 */
bool parse_msg_20(AISMessage *msg, const char *payload) {
    msg->type = 20;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    return true;
}

/**
 * @brief Parses AIS message type 21 (Aids-to-Navigation Report)
 */
bool parse_msg_21(AISMessage *msg, const char *payload) {
    msg->type = 21;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    msg->lon = parse_lon(payload, 57);
    msg->lat = parse_lat(payload, 85);
    normalize_position_fields(msg);
    return true;
}

/**
 * @brief Parses AIS message type 22 (Channel Management)
 */
bool parse_msg_22(AISMessage *msg, const char *payload) {
    msg->type = 22;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    return true;
}

/**
 * @brief Parses AIS message type 23 (Group Assignment Command)
 */
bool parse_msg_23(AISMessage *msg, const char *payload) {
    msg->type = 23;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    return true;
}

/**
 * @brief Parses AIS message type 25 (Binary Message, Single Slot)
 */
bool parse_msg_25(AISMessage *msg, const char *payload) {
    msg->type = 25;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    int bin_start = 40;
    int bin_len = ((int)strlen(payload) * 6) - bin_start;
    return parse_bin_payload(payload, bin_start, bin_len, &msg->bin_data, &msg->bin_len);
}

/**
 * @brief Parses AIS message type 26 (Binary Message, Multiple Slot)
 */
bool parse_msg_26(AISMessage *msg, const char *payload) {
    msg->type = 26;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    int bin_start = 90;
    int bin_len = ((int)strlen(payload) * 6) - bin_start;
    return parse_bin_payload(payload, bin_start, bin_len, &msg->bin_data, &msg->bin_len);
}

/**
 * @brief Parses AIS message type 27 (Long Range AIS Broadcast Message)
 */
bool parse_msg_27(AISMessage *msg, const char *payload) {
    msg->type = 27;
    msg->mmsi = parse_uint_safe(payload, 8, 30);
    msg->lon = parse_lon(payload, 44);
    msg->lat = parse_lat(payload, 61);
    normalize_position_fields(msg);
    return true;
}

/**
 * @brief Dispatch parser for AIS payloads by message type
 *
 * Extracts the message type and routes to the appropriate parser.
 * Returns true on successful parsing.
 */
bool parse_ais_payload(AISMessage *msg, const char *payload, int fill_bits) {
    (void)fill_bits;
    if (!payload || strlen(payload) < 1) return false;
    int msg_type = parse_uint_safe(payload, 0, 6);
    switch (msg_type) {
        case 1:
        case 2:
        case 3: return parse_msg_1_2_3(msg, payload);
        case 4:
        case 11: return parse_msg_4_11(msg, payload);
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