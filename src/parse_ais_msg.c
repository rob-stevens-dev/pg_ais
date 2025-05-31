// File: src/parse_ais_msg.h
#ifndef PARSE_AIS_MSG_H
#define PARSE_AIS_MSG_H

#include "pg_ais.h"

bool parse_ais_payload(AISMessage *msg, const char *payload, int fill_bits);

#endif

// ------------------------------------------
// File: src/parse_ais_msg.c
#include "parse_ais_msg.h"
#include "bitfield.h"
#include <string.h>

bool parse_msg_1(AISMessage *msg, const char *payload) {
    msg->type = 1;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->lat = parse_lat(payload, 89);
    msg->lon = parse_lon(payload, 61);
    msg->speed = parse_speed(payload, 50);
    msg->heading = parse_heading(payload, 128);
    return true;
}

bool parse_msg_2(AISMessage *msg, const char *payload) {
    msg->type = 2;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->lat = parse_lat(payload, 89);
    msg->lon = parse_lon(payload, 61);
    msg->speed = parse_speed(payload, 50);
    msg->heading = parse_heading(payload, 128);
    return true;
}

bool parse_msg_3(AISMessage *msg, const char *payload) {
    msg->type = 3;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->lat = parse_lat(payload, 89);
    msg->lon = parse_lon(payload, 61);
    msg->speed = parse_speed(payload, 50);
    msg->heading = parse_heading(payload, 128);
    return true;
}

bool parse_msg_4(AISMessage *msg, const char *payload) {
    msg->type = 4;
    msg->mmsi = parse_uint(payload, 8, 30);
    msg->lat = parse_lat(payload, 89);
    msg->lon = parse_lon(payload, 61);
    return true;
}

bool parse_msg_5(AISMessage *msg, const char *payload) {
    msg->type = 5;
    msg->mmsi = parse_uint(payload, 8, 30);
    // Example: fetch IMO and name, would need to extend struct
    // uint32_t imo = parse_uint(payload, 40, 30);
    // char *vessel_name = parse_string(payload, 112, 120);
    // msg->vessel_name = vessel_name; // extend struct to hold this
    return true;
}

bool parse_ais_payload(AISMessage *msg, const char *payload, int fill_bits) {
    if (!payload || strlen(payload) < 1) return false;
    int msg_type = parse_uint(payload, 0, 6);
    switch (msg_type) {
        case 1: return parse_msg_1(msg, payload);
        case 2: return parse_msg_2(msg, payload);
        case 3: return parse_msg_3(msg, payload);
        case 4: return parse_msg_4(msg, payload);
        case 5: return parse_msg_5(msg, payload);
        default: return false;
    }
}
