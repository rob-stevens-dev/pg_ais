#ifndef AIS_CORE_H
#define AIS_CORE_H

#include <stdbool.h>
#include <stdint.h>

#define AIS_MAX_SENTENCE_LEN 1024

typedef struct {
    int type;
    int mmsi;
    float lat;
    float lon;
    float speed;
    float heading;

    // Identification
    uint32_t imo;
    char *callsign;
    char *vessel_name;

    // Messaging and addressing
    uint32_t seq_num;
    uint32_t dest_mmsi;
    uint32_t retransmit;
    uint32_t app_id;
    uint8_t dac;
    uint8_t fid;
    uint8_t spare;

    // Binary payload
    char *bin_data;
    uint32_t bin_len;

    // Navigation status
    uint8_t repeat;
    uint8_t nav_status;
    int8_t rot;
    uint8_t accuracy;
    float course;
    uint8_t timestamp;
    uint8_t maneuver;
    uint8_t raim;
    uint32_t radio;

    // UTC and Fix Type
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t fix_type;

    // Static and voyage related (Message 5)
    uint8_t ship_type;
    uint16_t dimension_to_bow;
    uint16_t dimension_to_stern;
    uint8_t dimension_to_port;
    uint8_t dimension_to_starboard;
    uint8_t eta_month;
    uint8_t eta_day;
    uint8_t eta_hour;
    uint8_t eta_minute;
    float draught;
    char *destination;
} AISMessage;

bool parse_ais_sentence(const char *sentence, AISMessage *msg);
void free_ais_message(AISMessage *msg);

const char* nav_status_enum(int code);
const char* nav_status_enum(int code);
const char* maneuver_enum(int code);
const char* fix_type_enum(int code);
const char* ship_type_enum(int code);

#endif /* AIS_CORE_H */
