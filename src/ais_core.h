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
    uint32_t imo;
    char *callsign;
    char *vessel_name;
    uint32_t seq_num;
    uint32_t dest_mmsi;
    uint32_t retransmit;
    uint32_t app_id;
    char *bin_data;
    uint32_t bin_len;

    // Common fields
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
} AISMessage;

bool parse_ais_sentence(const char *sentence, AISMessage *msg);
void free_ais_message(AISMessage *msg);

#endif /* AIS_CORE_H */
