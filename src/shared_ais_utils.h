#ifndef SHARED_AIS_UTILS_H
#define SHARED_AIS_UTILS_H

#include <stdbool.h>
#include "pg_ais.h"

/**
 * @brief Normalize key positional fields in an AISMessage
 *
 * This is intended to apply validation rules to latitude, longitude, speed,
 * heading, course, and timestamp using their known sentinel values.
 *
 * @param msg Pointer to AISMessage (must not be NULL)
 */
void normalize_position_fields(AISMessage *msg);

/**
 * @brief Extract binary data from a payload to a new buffer
 *
 * This helper is used in message types like 6, 8, 26 etc. that carry binary
 * addressed or broadcast data. It extracts a byte-aligned binary buffer.
 *
 * @param payload AIS sentence payload
 * @param start Bit offset to begin parsing
 * @param len_bits Number of bits to read
 * @param out Pointer to output malloc'd buffer
 * @param out_len Number of bytes parsed
 * @return true if parsing succeeded and buffer is allocated
 */
bool parse_bin_payload(const char *payload, int start, int len_bits, char **out, int *out_len);

#endif
