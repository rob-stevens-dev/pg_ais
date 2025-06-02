#include "bitfield.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ais_ascii.h" // must define sixbit_ascii[64]


/**
 * @brief Convert a 6-bit AIS character to its numeric value
 *
 * This helper decodes an ASCII character from the AIS payload (chars 48–119)
 * into a 6-bit unsigned integer according to the ITU-R M.1371 spec.
 *
 * @param c Input character from AIS payload
 * @return Value from 0–63, or 0 if input is invalid
 */
static inline uint8_t sixbit_to_uint(char c) {
    if (c < 48 || c > 119) return 0;
    c -= 48;
    if (c > 40) c -= 8;
    return c;
}


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
bool parse_uint_safe(const char *payload, int start, int len, uint32_t *result) {
    if (!payload || !result || start < 0 || len <= 0 || len > 32) {
        return false;
    }

    int bit_len = (int)strlen(payload) * 6;
    if ((start + len) > bit_len) {
        return false;  // would read past end of buffer
    }

    uint32_t value = 0;
    for (int i = 0; i < len; i++) {
        int bit_idx = start + i;
        int byte_idx = bit_idx / 6;
        int bit_offset = 5 - (bit_idx % 6);
        char c = payload[byte_idx];
        if (c < 48 || c > 119) {
            return false;
        }
        int bit = ((c - 48) >> bit_offset) & 1;
        value = (value << 1) | bit;
    }

    *result = value;
    return true;
}


/**
 * @brief Parse signed integer from AIS payload with bounds checking
 *
 * @param payload AIS 6-bit ASCII payload string (null-terminated)
 * @param start   Starting bit index (0-based)
 * @param len     Number of bits to read (max 32)
 * @param result  Output parameter for the parsed signed integer
 * @return true if parsing succeeded, false on bounds or input error
 */
bool parse_int_safe(const char *payload, int start, int len, int32_t *result) {
    if (!payload || !result || len <= 0 || len > 32 || start < 0) {
        return false;
    }

    int bit_len = (int)strlen(payload) * 6;
    if ((start + len) > bit_len) {
        return false;
    }
    
    uint32_t val = 0;
    for (int i = 0; i < len; i++) {
        int bit_idx = start + i;
        int byte_idx = bit_idx / 6;
        int bit_offset = 5 - (bit_idx % 6);
        char c = payload[byte_idx];
        if (c < 48 || c > 119) return false;
        int bit = ((c - 48) >> bit_offset) & 1;
        val = (val << 1) | bit;
    }
    
    if ((val >> (len - 1)) & 1) {
        val |= (~0U << len); // Sign extend
    }
    
    *result = (int32_t)val;
    return true;
}


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
bool parse_float_safe(const char *payload, int start, int len, double scale, double *result) {
    int32_t val = 0;
    if (!result || scale == 0.0) return false;
    if (!parse_int_safe(payload, start, len, &val)) return false;
    *result = val / scale;
    return true;
}


/**
 * @brief Parse boolean value from AIS payload (1-bit)
 *
 * @param payload AIS payload (null-terminated)
 * @param start   Starting bit index
 * @param result  Output parameter for boolean result
 * @return true on success, false on invalid input or range
 */
bool parse_bool_safe(const char *payload, int start, bool *result) {
    uint32_t bit = 0;
    if (!result) return false;
    if (!parse_uint_safe(payload, start, 1, &bit)) return false;
    *result = (bit != 0);
    return true;
}


/**
 * @brief Parse latitude from AIS payload (in degrees)
 *
 * @param payload AIS 6-bit encoded payload
 * @param start   Bit offset to begin parsing
 * @return Latitude in decimal degrees, or NAN on error
 */
double parse_lat(const char *payload, int start) {
    int32_t val = 0;
    if (!parse_int_safe(payload, start, 27, &val)) return NAN;
    return val / 600000.0;
}


/**
 * @brief Parse longitude from AIS payload (in degrees)
 *
 * @param payload AIS 6-bit encoded payload
 * @param start   Bit offset to begin parsing
 * @return Longitude in decimal degrees, or NAN on error
 */
double parse_lon(const char *payload, int start) {
    int32_t val = 0;
    if (!parse_int_safe(payload, start, 28, &val)) return NAN;
    return val / 600000.0;
}


/**
 * @brief Parse speed over ground from AIS payload (in knots)
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @return Speed in knots, or -1.0 if unavailable (1023 encoded)
 */
double parse_speed(const char *payload, int start) {
    uint32_t raw = 0;
    if (!parse_uint_safe(payload, start, 10, &raw)) return -1.0;
    if (raw == 1023) return -1.0;
    return raw / 10.0;
}


/**
 * @brief Parse heading from AIS payload (in degrees)
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @return Heading in degrees, or -1.0 if unavailable (511 encoded)
 */
double parse_heading(const char *payload, int start) {
    uint32_t raw = 0;
    if (!parse_uint_safe(payload, start, 9, &raw)) return -1.0;
    if (raw == 511) return -1.0;
    return raw;
}


/**
 * @brief Parse 6-bit ASCII string from AIS payload
 *
 * This function decodes a bitfield into a null-terminated UTF-8 string using
 * the ITU-R M.1371 6-bit ASCII character set.
 *
 * @param payload AIS payload
 * @param start   Bit offset
 * @param len     Length in bits
 * @return Decoded string (malloc'd, caller must free), or NULL on error
 *
 * @note This function assumes sixbit_ascii[64] is declared externally.
 *       Trims trailing spaces. Returns NULL on bounds or memory failure.
 */
char *parse_string(const char *payload, int start, int len) {
    if (!payload || start < 0 || len <= 0) return NULL;
    int bit_len = (int)strlen(payload) * 6;
    if ((start + len) > bit_len) return NULL;

    int byte_len = (len + 5) / 6;
    char *out = malloc(byte_len + 1);
    if (!out) return NULL;

    for (int i = 0; i < byte_len; i++) {
        uint32_t sixbit = 0;
        if (!parse_uint_safe(payload, start + i * 6, 6, &sixbit)) {
            free(out);
            return NULL;
        }
        if (sixbit > 63) {
            free(out);
            return NULL;
        }
        out[i] = (sixbit == 0) ? ' ' : sixbit_ascii[sixbit];
    }
    out[byte_len] = '\0';

    // Trim trailing spaces
    for (int i = byte_len - 1; i >= 0; i--) {
        if (out[i] == ' ') {
            out[i] = '\0';
        } else {
            break;
        }
    }

    return out;
}
