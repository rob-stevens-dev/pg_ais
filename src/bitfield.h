#ifndef BITFIELD_H
#define BITFIELD_H

#include <stdbool.h>
#include <stdint.h>


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
 * @return true on success, false if input is invalid or out of bounds
 */
bool parse_uint_safe(const char *payload, int start, int len, uint32_t *result);


/**
 * @brief Parse signed integer from AIS payload with bounds checking
 *
 * @param payload AIS 6-bit ASCII payload string (null-terminated)
 * @param start   Starting bit index (0-based)
 * @param len     Number of bits to read (max 32)
 * @param result  Output parameter for the parsed signed integer
 * @return true if parsing succeeded, false on bounds or input error
 */
bool parse_int_safe(const char *payload, int start, int len, int32_t *result);


/**
 * @brief Parse scaled floating point from AIS payload with bounds checking
 *
 * @param payload AIS 6-bit ASCII payload string (null-terminated)
 * @param start   Starting bit index (0-based)
 * @param len     Number of bits to read (max 32)
 * @param scale   Divisor to convert integer to float (e.g., 10, 600000.0)
 * @param result  Output parameter for float result
 * @return true if parsing succeeded, false otherwise
 */
bool parse_float_safe(const char *payload, int start, int len, double scale, double *result);

/**
 * @brief Parse boolean value from AIS payload (1-bit)
 *
 * @param payload AIS payload (null-terminated)
 * @param start   Starting bit index
 * @param result  Output parameter for boolean result
 * @return true on success, false on invalid input or range
 */
bool parse_bool_safe(const char *payload, int start, bool *result);


/**
 * @brief Parse latitude from AIS payload (in degrees)
 *
 * @param payload AIS 6-bit encoded payload
 * @param start   Bit offset to begin parsing
 * @return Latitude in decimal degrees, or NAN on error
 */
double parse_lat(const char *payload, int start);


/**
 * @brief Parse longitude from AIS payload (in degrees)
 *
 * @param payload AIS 6-bit encoded payload
 * @param start   Bit offset to begin parsing
 * @return Longitude in decimal degrees, or NAN on error
 */
double parse_lon(const char *payload, int start);


/**
 * @brief Parse speed over ground from AIS payload (in knots)
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @return Speed in knots, or -1.0 if unavailable (1023 encoded)
 */
double parse_speed(const char *payload, int start);


/**
 * @brief Parse heading from AIS payload (in degrees)
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @return Heading in degrees, or -1.0 if unavailable (511 encoded)
 */
double parse_heading(const char *payload, int start);


/**
 * @brief Parse 6-bit ASCII string from AIS payload
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @param len     Length in bits
 * @return Decoded null-terminated string (caller must free), or NULL on failure
 */
char *parse_string(const char *payload, int start, int len);

#endif