#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "parse_ais_msg.h"
#include "bitfield.h"
#include "shared_ais_utils.h"
#include "pg_ais_metrics.h"


/**
 * @brief Parses AIS message types 1, 2, and 3 (Class A position reports)
 *
 * Populates navigation and positional data including MMSI, navigation status,
 * rate of turn, speed, heading, and location.
 *
 * @param msg Pointer to AISMessage struct to populate
 * @param payload NMEA payload to parse (6-bit ASCII encoded)
 * @return ParseResult indicating success or specific failure
 */
ParseResult parse_msg_1_2_3(AISMessage *msg, const char *payload) {
    msg->type = 1;
    if (!parse_uint_safe(payload, 6, 2, &msg->repeat)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 38, 4, &msg->nav_status)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 42, 8, &msg->rot)) return PARSE_ERROR;
    msg->speed = parse_speed(payload, 50);
    if (!parse_uint_safe(payload, 60, 1, &msg->accuracy)) return PARSE_ERROR;
    msg->lon = parse_lon(payload, 61);
    msg->lat = parse_lat(payload, 89);
    uint32_t course_raw;
    if (!parse_uint_safe(payload, 116, 12, &course_raw)) return PARSE_ERROR;
    msg->course = course_raw / 10.0;
    msg->heading = parse_heading(payload, 128);
    if (!parse_uint_safe(payload, 137, 6, &msg->timestamp)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 143, 2, &msg->maneuver)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 148, 1, &msg->raim)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 149, 19, &msg->radio)) return PARSE_ERROR;
    normalize_position_fields(msg);
    return PARSE_OK;
}


/**
 * @brief Parses AIS message types 4 and 11 (Base station reports)
 *
 * Contains timestamped positional reports from base stations or UTC info.
 *
 * @param msg Pointer to AISMessage struct to populate
 * @param payload 6-bit encoded NMEA payload
 * @return ParseResult with success or specific failure
 */
ParseResult parse_msg_4_11(AISMessage *msg, const char *payload) {
    msg->type = 4;
    if (!parse_uint_safe(payload, 6, 2, &msg->repeat)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 78, 1, &msg->accuracy)) return PARSE_ERROR;
    msg->lon = parse_lon(payload, 79);
    msg->lat = parse_lat(payload, 107);
    if (!parse_uint_safe(payload, 137, 6, &msg->timestamp)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 148, 1, &msg->raim)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 149, 19, &msg->radio)) return PARSE_ERROR;
    normalize_position_fields(msg);
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 5 (Static and Voyage Related Data)
 *
 * Extracts ship name, call sign, IMO, dimensions, ETA, destination, etc.
 *
 * @param msg Pointer to AISMessage struct to populate
 * @param payload NMEA payload
 * @return ParseResult indicating parsing success or error type
 */
ParseResult parse_msg_5(AISMessage *msg, const char *payload) {
    msg->type = 5;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 40, 30, &msg->imo)) return PARSE_ERROR;
    if (parse_string_utf8(payload, 70, 42, &msg->callsign) != PARSE_OK) return PARSE_ERROR;
    if (parse_string_utf8(payload, 112, 120, &msg->vessel_name) != PARSE_OK) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 6 (Binary addressed message)
 *
 * Includes destination MMSI and application binary data payload.
 *
 * @param msg AISMessage pointer to populate
 * @param payload Raw payload string
 * @return ParseResult indicating success or failure with reason
 */
ParseResult parse_msg_6(AISMessage *msg, const char *payload) {
    msg->type = 6;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    int bin_start = 88;
    int bin_len = ((int)strlen(payload) * 6) - bin_start;
    if (!parse_bin_payload(payload, bin_start, bin_len, &msg->bin_data, &msg->bin_len)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 7 (Binary acknowledge)
 *
 * Acknowledges receipt of a binary message.
 *
 * @param msg Target AISMessage struct
 * @param payload Encoded NMEA data
 * @return ParseResult describing outcome
 */
ParseResult parse_msg_7(AISMessage *msg, const char *payload) {
    msg->type = 7;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 8 (Binary broadcast message)
 *
 * Broadcast binary data intended for all vessels.
 *
 * @param msg Output AISMessage struct
 * @param payload 6-bit encoded AIS payload
 * @return ParseResult with structured result
 */
ParseResult parse_msg_8(AISMessage *msg, const char *payload) {
    msg->type = 8;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    int bin_start = 56;
    int bin_len = ((int)strlen(payload) * 6) - bin_start;
    if (!parse_bin_payload(payload, bin_start, bin_len, &msg->bin_data, &msg->bin_len)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 9 (SAR aircraft position report)
 *
 * Reports aircraft GPS position, altitude, and velocity.
 *
 * @param msg AISMessage to fill
 * @param payload 6-bit string to decode
 * @return ParseResult containing parsing outcome
 */
ParseResult parse_msg_9(AISMessage *msg, const char *payload) {
    msg->type = 9;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    msg->lat = parse_lat(payload, 107);
    msg->lon = parse_lon(payload, 79);
    msg->speed = parse_speed(payload, 50);
    msg->heading = parse_heading(payload, 128);
    uint32_t course_raw;
    if (!parse_uint_safe(payload, 116, 12, &course_raw)) return PARSE_ERROR;
    msg->course = course_raw / 10.0;
    normalize_position_fields(msg);
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 10 (UTC/Date inquiry)
 *
 * Contains an inquiry request to a specific MMSI.
 *
 * @param msg AISMessage result struct
 * @param payload Raw payload input
 * @return ParseResult with structured status
 */
ParseResult parse_msg_10(AISMessage *msg, const char *payload) {
    msg->type = 10;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 12 (Safety-related text)
 *
 * Broadcasts short safety-related text messages.
 *
 * @param msg Target AISMessage
 * @param payload ASCII-encoded text payload
 * @return ParseResult indicating validity and status
 */
ParseResult parse_msg_12(AISMessage *msg, const char *payload) {
    msg->type = 12;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    int bitlen = ((int)strlen(payload) * 6) - 72;
    if (parse_string_utf8(payload, 72, bitlen, &msg->vessel_name) != PARSE_OK) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 13 (Safety-related acknowledge)
 *
 * Responds to message type 12 messages.
 *
 * @param msg Pointer to destination struct
 * @param payload Raw string payload
 * @return ParseResult
 */
ParseResult parse_msg_13(AISMessage *msg, const char *payload) {
    msg->type = 13;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 14 (Broadcast safety message)
 *
 * Sent to all ships with safety-related text info.
 *
 * @param msg Output message container
 * @param payload AIS encoded message
 * @return ParseResult
 */
ParseResult parse_msg_14(AISMessage *msg, const char *payload) {
    msg->type = 14;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    int bitlen = ((int)strlen(payload) * 6) - 40;
    if (parse_string_utf8(payload, 40, bitlen, &msg->vessel_name) != PARSE_OK) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 15 (Interrogation)
 *//**
 * @brief Parses AIS message type 15 (Interrogation)
 *
 * Requests data from another vessel.
 *
 * @param msg AIS struct to populate
 * @param payload Source payload string
 * @return ParseResult with decode status
 */
ParseResult parse_msg_15(AISMessage *msg, const char *payload) {
    msg->type = 15;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 16 (Assigned mode command)
 *
 * Instructs a vessel to switch to an assigned reporting interval.
 *
 * @param msg AIS struct to populate
 * @param payload Encoded command string
 * @return ParseResult with operation status
 */
ParseResult parse_msg_16(AISMessage *msg, const char *payload) {
    msg->type = 16;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 17 (DGNSS broadcast binary)
 *
 * Differential GNSS data payload broadcast to vessels.
 *
 * @param msg Output AISMessage struct
 * @param payload 6-bit encoded string
 * @return ParseResult
 */
ParseResult parse_msg_17(AISMessage *msg, const char *payload) {
    msg->type = 17;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    int bin_start = 80;
    int bin_len = ((int)strlen(payload) * 6) - bin_start;
    if (!parse_bin_payload(payload, bin_start, bin_len, &msg->bin_data, &msg->bin_len)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message types 18, 19, and 24 (Class B reports)
 *
 * Class B equipment position, static, and vessel type details.
 *
 * @param msg Pointer to AISMessage struct to populate
 * @param payload Encoded payload string
 * @return ParseResult with outcome of decoding
 */
ParseResult parse_msg_18_19_24(AISMessage *msg, const char *payload) {
    msg->type = 18;
    if (!parse_uint_safe(payload, 6, 2, &msg->repeat)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    msg->speed = parse_speed(payload, 46);
    if (!parse_uint_safe(payload, 56, 1, &msg->accuracy)) return PARSE_ERROR;
    msg->lon = parse_lon(payload, 57);
    msg->lat = parse_lat(payload, 85);
    uint32_t course_raw;
    if (!parse_uint_safe(payload, 112, 12, &course_raw)) return PARSE_ERROR;
    msg->course = course_raw / 10.0;
    msg->heading = parse_heading(payload, 128);
    if (!parse_uint_safe(payload, 134, 6, &msg->timestamp)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 148, 1, &msg->raim)) return PARSE_ERROR;
    if (!parse_uint_safe(payload, 149, 19, &msg->radio)) return PARSE_ERROR;
    normalize_position_fields(msg);
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 20 (Data link management)
 *
 * Used to control data slots for self-organizing TDMA.
 *
 * @param msg Target structure to fill
 * @param payload 6-bit binary payload
 * @return ParseResult
 */
ParseResult parse_msg_20(AISMessage *msg, const char *payload) {
    msg->type = 20;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 21 (Aids to Navigation report)
 *
 * Reports position and status of buoys, lighthouses, etc.
 *
 * @param msg Pointer to message struct
 * @param payload Raw encoded data
 * @return ParseResult status
 */
ParseResult parse_msg_21(AISMessage *msg, const char *payload) {
    msg->type = 21;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    msg->lon = parse_lon(payload, 57);
    msg->lat = parse_lat(payload, 85);
    normalize_position_fields(msg);
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 22 (Channel management)
 *
 * Instructions for VHF channel allocations by location.
 *
 * @param msg AIS message struct to populate
 * @param payload Raw 6-bit encoded string
 * @return ParseResult
 */
ParseResult parse_msg_22(AISMessage *msg, const char *payload) {
    msg->type = 22;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 23 (Group assignment command)
 *
 * Assigns AIS reporting intervals to vessels in a region.
 *
 * @param msg AISMessage output struct
 * @param payload Raw NMEA payload
 * @return ParseResult with result status
 */
ParseResult parse_msg_23(AISMessage *msg, const char *payload) {
    msg->type = 23;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 25 (Single slot binary message)
 *
 * Carries a short binary message in a single time slot.
 *
 * @param msg Message structure to fill
 * @param payload Payload string input
 * @return ParseResult with decode status
 */
ParseResult parse_msg_25(AISMessage *msg, const char *payload) {
    msg->type = 25;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    int bin_start = 40;
    int bin_len = ((int)strlen(payload) * 6) - bin_start;
    if (!parse_bin_payload(payload, bin_start, bin_len, &msg->bin_data, &msg->bin_len)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 26 (Multi-slot binary message)
 *
 * Carries binary data over multiple time slots, addressed or broadcast.
 *
 * @param msg Struct to populate with result
 * @param payload Payload string
 * @return ParseResult
 */
ParseResult parse_msg_26(AISMessage *msg, const char *payload) {
    msg->type = 26;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    int bin_start = 90;
    int bin_len = ((int)strlen(payload) * 6) - bin_start;
    if (!parse_bin_payload(payload, bin_start, bin_len, &msg->bin_data, &msg->bin_len)) return PARSE_ERROR;
    return PARSE_OK;
}


/**
 * @brief Parses AIS message type 27 (Long-range AIS broadcast)
 *
 * Provides global position report from Class A/B transceivers.
 *
 * @param msg Output struct
 * @param payload Encoded long-range payload
 * @return ParseResult
 */
ParseResult parse_msg_27(AISMessage *msg, const char *payload) {
    msg->type = 27;
    if (!parse_uint_safe(payload, 8, 30, &msg->mmsi)) return PARSE_ERROR;
    msg->lon = parse_lon(payload, 44);
    msg->lat = parse_lat(payload, 61);
    normalize_position_fields(msg);
    return PARSE_OK;
}


/**
 * @brief Entry point dispatcher for AIS message parsing
 *
 * Routes the payload to the appropriate parse_msg_* handler based on type.
 *
 * @param msg AISMessage struct to fill
 * @param payload NMEA payload to parse
 * @param fill_bits Fill bits at the end of the NMEA message
 * @return ParseResult containing structured status of parse attempt
 */
ParseResult parse_ais_payload(AISMessage *msg, const char *payload, int fill_bits) {
    ParseResult result;
    (void)fill_bits;
    if (!payload || strlen(payload) < 1) return PARSE_ERROR;
    uint32_t msg_type;
    if (!parse_uint_safe(payload, 0, 6, &msg_type)) return PARSE_ERROR;
    switch (msg_type) {
        case 1:
        case 2:
        case 3: result = parse_msg_1_2_3(msg, payload); break;
        case 4:
        case 11: result = parse_msg_4_11(msg, payload); break;
        case 5: result = parse_msg_5(msg, payload); break;
        case 6: result = parse_msg_6(msg, payload); break;
        case 7: result = parse_msg_7(msg, payload); break;
        case 8: result = parse_msg_8(msg, payload); break;
        case 9: result = parse_msg_9(msg, payload); break;
        case 10: result = parse_msg_10(msg, payload); break;
        case 12: result = parse_msg_12(msg, payload); break;
        case 13: result = parse_msg_13(msg, payload); break;
        case 14: result = parse_msg_14(msg, payload); break;
        case 15: result = parse_msg_15(msg, payload); break;
        case 16: result = parse_msg_16(msg, payload); break;
        case 17: result = parse_msg_17(msg, payload); break;
        case 18:
        case 19:
        case 24: result = parse_msg_18_19_24(msg, payload); break;
        case 20: result = parse_msg_20(msg, payload); break;
        case 21: result = parse_msg_21(msg, payload); break;
        case 22: result = parse_msg_22(msg, payload); break;
        case 23: result = parse_msg_23(msg, payload); break;
        case 25: result = parse_msg_25(msg, payload); break;
        case 26: result = parse_msg_26(msg, payload); break;
        case 27: result = parse_msg_27(msg, payload); break;
        default: result = PARSE_UNSUPPORTED; break;
        default: return PARSE_UNSUPPORTED;
    }
    pg_ais_record_parse_result(result == PARSE_OK);
    return result;
}
