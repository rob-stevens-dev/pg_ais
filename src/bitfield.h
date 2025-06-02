#ifndef BITFIELD_H
#define BITFIELD_H

#include <stdbool.h>
#include <stdint.h>
#include "parse_ais_result.h"


/**
 * @brief Safely parse an unsigned integer from AIS 6-bit encoded payload
 *
 * This function decodes a binary value from the AIS payload, with strict bounds checking
 * to avoid over-reads and invalid access. Use this instead of any raw bit-shifting or unsafe casts.
 *
 * @param payload AIS payload string (6-bit ASCII encoded)
 * @param start   Bit offset to begin extraction (0-based)
 * @param len     Number of bits to extract (1–32)
 * @param result  Output pointer to store the resulting value
 *
 * @return ParseResult indicating success or failure, with reason
 */
ParseResult parse_uint_safe(const char *payload, int start, int len, uint32_t *result);


/**
 * @brief Parse signed integer from AIS payload with bounds checking
 *
 * Extracts a signed integer using two's complement from a 6-bit AIS payload.
 * Ensures bounds safety and character validity before performing the conversion.
 *
 * @param payload AIS 6-bit ASCII payload string (null-terminated)
 * @param start   Starting bit index (0-based)
 * @param len     Number of bits to read (max 32)
 * @param result  Output parameter for the parsed signed integer
 *
 * @return ParseResult indicating success or specific failure type
 */
ParseResult parse_int_safe(const char *payload, int start, int len, int32_t *result);


/**
 * @brief Parse scaled floating point from AIS payload with bounds checking
 *
 * Converts a signed integer extracted from the payload into a float using
 * a scaling factor (e.g., 10 for knots, 600000 for degrees).
 *
 * @param payload AIS 6-bit ASCII payload string (null-terminated)
 * @param start   Starting bit index (0-based)
 * @param len     Number of bits to read (max 32)
 * @param scale   Divisor to convert integer to float (e.g., 10, 600000.0)
 * @param result  Output parameter for float result
 *
 * @return ParseResult indicating parsing success or detailed error
 */
ParseResult parse_float_safe(const char *payload, int start, int len, double scale, double *result);

/**
 * @brief Parse boolean value from AIS payload (1-bit)
 *
 * Reads a single bit and interprets it as a boolean (0 or 1).
 *
 * @param payload AIS payload (null-terminated)
 * @param start   Starting bit index
 * @param result  Output parameter for boolean result
 * @return ParseResult with success status and failure info
 */
ParseResult parse_bool_safe(const char *payload, int start, bool *result);


/**
 * @brief Parse latitude with error reporting (in degrees)
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @param out     Pointer to output latitude
 * @return ParseResult with success status and optional message
 */
ParseResult parse_lat_safe(const char *payload, int start, double *out);

/**
 * @brief Parse longitude with error reporting (in degrees)
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @param out     Pointer to output longitude
 * @return ParseResult with success status and optional message
 */
ParseResult parse_lon_safe(const char *payload, int start, double *out);

/**
 * @brief Parse speed over ground with error reporting (in knots)
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @param out     Pointer to output speed
 * @return ParseResult with success status and optional message
 */
ParseResult parse_speed_safe(const char *payload, int start, double *out);

/**
 * @brief Parse true heading with error reporting (in degrees)
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @param out     Pointer to output heading
 * @return ParseResult with success status and optional message
 */
ParseResult parse_heading_safe(const char *payload, int start, double *out);

/**
 * @brief Parse 6-bit ASCII string from AIS payload with structured error
 *
 * This version returns a structured ParseResult and uses an output parameter.
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @param len     Length in bits
 * @param out     Output pointer to allocated null-terminated string (caller must free)
 * @return ParseResult indicating success or failure with message
 */
ParseResult parse_string_safe(const char *payload, int start, int len, char **out);

#endif
