#ifndef AIS_CORE_H
#define AIS_CORE_H

#include <stdbool.h>
#include <stdint.h>


#define AIS_MAX_SENTENCE_LEN 1024

// Caller must free: callsign, vessel_name, destination, bin_data
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
 * @brief Parse a full NMEA AIS sentence into an AISMessage struct
 *
 * This function extracts relevant fields from a validated NMEA sentence
 * and populates the AISMessage structure with type, location, navigation,
 * and voyage data.
 *
 * @param sentence The full AIS NMEA sentence (e.g. !AIVDM...)
 * @param msg Pointer to an AISMessage to populate
 * @return true on success, false on failure or unsupported message type
 */
bool parse_nmea_sentence(const char *sentence, AISMessage *msg);


/**
 * @brief Free dynamic fields allocated within an AISMessage
 *
 * Frees all pointer-based subfields of an AISMessage structure,
 * including callsign, vessel_name, destination, and bin_data.
 * Does not free the AISMessage struct itself.
 *
 * @param msg Pointer to AISMessage whose contents will be freed
 */
void free_ais_message(AISMessage *msg);


/**
 * @brief Convert navigation status code to descriptive string
 *
 * @param code Integer navigation status code (0–15)
 * @return Static string description (e.g. "At anchor") or "Unknown"
 */
const char* ais_nav_status_to_str(int code);


/**
 * @brief Convert maneuver indicator to descriptive string
 *
 * @param code Integer maneuver code (0–2)
 * @return Static string like "Not available", "No special maneuver"
 */
const char* ais_maneuver_to_str(int code);


/**
 * @brief Convert GPS fix type code to readable string
 *
 * @param code Fix type integer code
 * @return Static string such as "GPS", "GLONASS", or "Unknown"
 */
const char* ais_fix_type_to_str(int code);


/**
 * @brief Convert ship type code to vessel class description
 *
 * @param code Integer ship type code (ITU-R M.1371 Annex B)
 * @return Static string like "Cargo", "Tanker", or "Unknown"
 */
const char* ais_ship_type_to_str(int code);

#endif
