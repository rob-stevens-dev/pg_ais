#include "bitfield.h"
#include "pg_ais.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief Normalize position, speed, heading, and timestamp in AISMessage
 *
 * This function post-processes raw decoded values from position reports,
 * and replaces sentinel values with canonical invalid representations.
 *
 * @param msg Pointer to AISMessage to normalize
 */
void normalize_position_fields(AISMessage *msg) {
    if (!msg) return;
    if (msg->lat < -90 || msg->lat > 90) msg->lat = 91.0;
    if (msg->lon < -180 || msg->lon > 180) msg->lon = 181.0;
    if (msg->speed > 102.2) msg->speed = -1;
    if (msg->course >= 360.0) msg->course = -1;
    if (msg->heading >= 511) msg->heading = -1;
    if (msg->timestamp == 60) msg->timestamp = 255;
}

/**
 * @brief Parse a binary payload from AIS into a malloc'd buffer
 *
 * Extracts a binary blob from a 6-bit ASCII encoded payload starting at a bit offset.
 * Uses 8-bit chunks to reconstruct the raw binary content and returns a pointer
 * to the buffer and its length.
 *
 * @param payload The AIS payload string
 * @param start   The bit offset to begin extraction
 * @param len_bits Number of bits to extract
 * @param out     Output pointer to malloc'd buffer (caller must free)
 * @param out_len Output length in bytes
 * @return true on success, false on bounds or allocation failure
 */
bool parse_bin_payload(const char *payload, int start, int len_bits, char **out, int *out_len) {
    if (!payload || !out || !out_len || len_bits <= 0) return false;

    int bit_capacity = (int)strlen(payload) * 6;
    if (start + len_bits > bit_capacity) return false;

    int num_bytes = len_bits / 8;
    char *buf = (char *)malloc(num_bytes);
    if (!buf) return false;

    for (int i = 0; i < num_bytes; i++) {
        uint32_t val;
        if (!parse_uint_safe(payload, start + i * 8, 8, &val)) {
            free(buf);
            return false;
        }
        buf[i] = (char)val;
    }

    *out = buf;
    *out_len = num_bytes;
    return true;
}