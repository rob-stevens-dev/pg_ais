#ifndef PARSE_AIS_H
#define PARSE_AIS_H

#include <stdbool.h>
#include <stdint.h>
#include "pg_ais.h"

#define MAX_PARTS 5

/**
 * @brief Parse a single !AIVDM fragment into its components
 *
 * Splits a raw NMEA 0183 sentence (typically !AIVDM) into its components and stores them
 * in a lightweight AISFragment structure. Does not validate checksum or perform deep parsing.
 *
 * @param sentence Full NMEA sentence (e.g., "!AIVDM,1,1,,A,...*hh")
 * @param frag Output structure (caller must call free_buffer() or manage payload/raw)
 * @return true on success, false if malformed
 */
bool parse_ais_fragment(const char *sentence, AISFragment *frag);


/**
 * @brief Decode a 6-bit ASCII field from an AIS payload into a UTF-8 string
 *
 * Converts a string of 6-bit encoded characters into a readable string using the
 * ITU-R M.1371-5 character set. Trailing spaces are trimmed.
 *
 * @param payload AIS 6-bit ASCII payload
 * @param start Bit offset to begin decoding
 * @param bitlen Number of bits to read (must be multiple of 6)
 * @return Newly allocated string (caller must free), or NULL on error
 */
char *parse_string_utf8(const char *payload, int start, int bitlen);


/**
 * @brief Reassemble multipart AIS message fragments into one message
 *
 * Joins payloads from a buffer of AISFragment parts and emits a synthesized AISMessage.
 * For now, this is stubbed and returns a fake fixed value once all parts are present.
 *
 * @param buffer Fragment buffer with parts
 * @param msg_out Output parsed AISMessage
 * @return true if reassembly was successful
 */
bool try_reassemble(AISFragmentBuffer *buffer, AISMessage *msg_out);


/**
 * @brief Release memory and reset fragment buffer to empty state
 *
 * Frees all parts and clears the buffer. Safe to call on already-empty buffer.
 *
 * @param buffer Buffer to reset
 */
void reset_buffer(AISFragmentBuffer *buffer);


/**
 * @brief Free all fragments and deallocate the buffer itself
 *
 * This function is used to release an entire fragment buffer that was allocated dynamically.
 * It frees all internal parts and then frees the buffer structure pointer.
 *
 * @param buffer Pointer to the dynamically allocated buffer
 */
void free_buffer(AISFragmentBuffer *buffer);

#endif