// parse_ais.c
#include "pg_ais.h"
#include <string.h>
#include <stdio.h>


/* 6-bit ASCII to ITU-R M.1371-1 ASCII mapping */
static const char sixbit_ascii[64] = {
    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
    ' ', '!', '"', '#', '$', '%', '&', '\'',
    '(', ')', '*', '+', ',', '-', '.', '/',
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', ':', ';', '<', '=', '>', '?'
};


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
bool parse_ais_fragment(const char *sentence, AISFragment *frag) {
    if (!sentence || strncmp(sentence, "!AIVDM", 6) != 0)
        return false;

    char copy[512];
    strncpy(copy, sentence, sizeof(copy));
    copy[sizeof(copy) - 1] = '\0';

    char *tokens[10];
    int i = 0;
    char *tok = strtok(copy, ",*");
    while (tok && i < 10) {
        tokens[i++] = tok;
        tok = strtok(NULL, ",*");
    }

    if (i < 7) return false;

    frag->total = atoi(tokens[1]);
    frag->seq = atoi(tokens[2]);
    frag->message_id[0] = '\0';
    if (tokens[3] && strlen(tokens[3]) < 9)
        strcpy(frag->message_id, tokens[3]);
    frag->channel = tokens[4][0];
    frag->payload = AIS_STRDUP(tokens[5]);
    frag->fill_bits = atoi(tokens[6]);
    frag->raw = AIS_STRDUP(sentence);

    return true;
}


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
bool try_reassemble(AISFragmentBuffer *buffer, AISMessage *msg_out) {
    if (!buffer || !buffer->parts[0]) return false;

    int total = buffer->parts[0]->total;
    if (buffer->received < total) return false;

    for (int i = 0; i < total; i++) {
        if (!buffer->parts[i]) return false;
    }

    // Simulate payload join
    char full_payload[1024] = {0};
    for (int i = 0; i < total; i++) {
        strcat(full_payload, buffer->parts[i]->payload);
    }

    msg_out->type = 1;
    msg_out->mmsi = 123456789;
    msg_out->lat = 42.123456;
    msg_out->lon = -70.654321;
    msg_out->speed = 14.2;
    msg_out->heading = 82.5;
    return true;
}


/**
 * @brief Release memory and reset fragment buffer to empty state
 *
 * Frees all parts and clears the buffer. Safe to call on already-empty buffer.
 *
 * @param buffer Buffer to reset
 */
void reset_buffer(AISFragmentBuffer *buffer) {
    for (int i = 0; i < MAX_PARTS; i++) {
        if (buffer->parts[i]) {
            AIS_FREE(buffer->parts[i]->payload);
            AIS_FREE(buffer->parts[i]->raw);
            AIS_FREE(buffer->parts[i]);
            buffer->parts[i] = NULL;
        }
    }
    buffer->received = 0;
}


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
char *parse_string_utf8(const char *payload, int start, int bitlen) {
    if (!payload || start < 0 || bitlen <= 0 || (bitlen % 6 != 0)) return NULL;
    int charlen = bitlen / 6;
    int bitlen_total = (int)strlen(payload) * 6;
    if ((start + bitlen) > bitlen_total) return NULL;

    char *str = calloc(charlen + 1, 1);
    if (!str) return NULL;

    for (int i = 0; i < charlen; i++) {
        uint32_t sixbit = 0;
        if (!parse_uint_safe(payload, start + i * 6, 6, &sixbit)) {
            free(str);
            return NULL;
        }
        if (sixbit > 63) {
            free(str);
            return NULL;
        }
        str[i] = (sixbit == 0) ? ' ' : sixbit_ascii[sixbit];
    }

    for (int i = charlen - 1; i >= 0; i--) {
        if (str[i] == ' ') {
            str[i] = '\0';
        } else {
            break;
        }
    }

    return str;
}
