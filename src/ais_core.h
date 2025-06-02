#ifndef AIS_CORE_H
#define AIS_CORE_H

#include <stdbool.h>
#include <stdint.h>


#define AIS_MAX_SENTENCE_LEN 1024


/**
 * @brief Decoded representation of a full AIS message
 *
 * Parsed result after decoding one or more AIS fragments. Contains fields
 * commonly used in vessel tracking and analytics.
 */
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


/**
 * @brief Free heap-allocated components of an AISMessage struct
 *
 * Releases memory from dynamic fields (callsign, vessel_name, destination, bin_data).
 *
 * @param msg Pointer to AISMessage with heap fields to release
 */
void free_ais_message(AISMessage *msg);


/**
 * @brief Convert navigation status code to descriptive string
 *
 * Maps 0–15 navigation status codes to human-readable labels.
 *
 * @param code Navigation status integer code
 * @return Constant string label or "Unknown"
 */
const char* ais_nav_status_to_str(int code);


/**
 * @brief Convert maneuver indicator code to descriptive string
 *
 * @param code Integer value (0–2)
 * @return Static description string or "Unknown"
 */
const char* ais_maneuver_to_str(int code);


/**
 * @brief Convert GPS fix type code to readable string
 *
 * @param code Fix type numeric value
 * @return Descriptive label (e.g. "GPS", "GLONASS")
 */
const char* ais_fix_type_to_str(int code);


/**
 * @brief Convert ship type numeric code to vessel class string
 *
 * Uses ITU-R M.1371 Annex B codes.
 *
 * @param code Ship type numeric code
 * @return Descriptive string like "Tanker" or "Cargo"
 */
const char* ais_ship_type_to_str(int code);


/**
 * @brief Append a JSONB object field with an enum value and its label.
 *
 * Adds a key to the current JSONB object being built, storing both the raw integer
 * code and the corresponding human-readable label from an enum converter.
 *
 * Example:
 *   "nav_status": { "code": 5, "label": "Moored" }
 *
 * @param state     Pointer to the JSONB parse state
 * @param key       Field name to add to the object
 * @param value     Enum value to encode
 * @param enum_func Function to convert value to label
 */
void append_jsonb_enum_field(JsonbParseState **state, const char *key, int value, const char *(*enum_func)(int));

#endif