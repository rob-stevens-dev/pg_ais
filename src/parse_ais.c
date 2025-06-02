#include "pg_ais.h"
#include "parse_ais.h"
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
ParseResult parse_ais_fragment(const char *sentence, AISFragment *frag) {
    if (!sentence || strncmp(sentence, "!AIVDM", 6) != 0)
        return PARSE_ERROR;

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

    if (i < 7) return PARSE_ERROR;

    frag->total = atoi(tokens[1]);
    frag->seq = atoi(tokens[2]);
    frag->message_id[0] = '\0';
    if (tokens[3] && strlen(tokens[3]) < 9)
        strcpy(frag->message_id, tokens[3]);
    frag->channel = tokens[4][0];
    frag->payload = AIS_STRDUP(tokens[5]);
    frag->fill_bits = atoi(tokens[6]);
    frag->raw = AIS_STRDUP(sentence);

    return PARSE_OK;
}


/**
 * @brief Reassemble multipart AIS message fragments into one message
 *
 * Joins payloads from a buffer of AISFragment parts and emits a synthesized AISMessage.
 * Tracks parse success using pg_ais_record_parse_result.
 *
 * @param buffer Fragment buffer with parts
 * @param msg_out Output parsed AISMessage
 * @return ParseResult indicating success or reason for failure
 */
ParseResult try_reassemble(AISFragmentBuffer *buffer, AISMessage *msg_out) {
    if (!buffer || !buffer->parts[0]) return PARSE_ERROR;

    int total = buffer->parts[0]->total;
    if (buffer->received < total) return PARSE_INCOMPLETE;

    for (int i = 0; i < total; i++) {
        if (!buffer->parts[i]) return PARSE_INCOMPLETE;
    }

    char full_payload[1024] = {0};
    int fill_bits = buffer->parts[total - 1]->fill_bits;

    for (int i = 0; i < total; i++) {
        strncat(full_payload, buffer->parts[i]->payload, sizeof(full_payload) - strlen(full_payload) - 1);
    }

    ParseResult result = parse_ais_payload(msg_out, full_payload, fill_bits);
    pg_ais_record_parse_result(result == PARSE_OK);
    pg_ais_record_reassembly_attempt(result == PARSE_OK);
    return result;
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
 * @param out Pointer to store allocated string on success
 * @return ParseResult indicating success or failure
 */
ParseResult parse_string_utf8(const char *payload, int start, int bitlen, char **out) {
    if (!payload || start < 0 || bitlen <= 0 || (bitlen % 6 != 0)) return PARSE_ERROR;
    int charlen = bitlen / 6;
    int bitlen_total = (int)strlen(payload) * 6;
    if ((start + bitlen) > bitlen_total) return PARSE_ERROR;

    char *str = calloc(charlen + 1, 1);
    if (!str) return PARSE_ERROR;

    for (int i = 0; i < charlen; i++) {
        uint32_t sixbit = 0;
        if (!parse_uint_safe(payload, start + i * 6, 6, &sixbit)) {
            free(str);
            return PARSE_ERROR;
        }
        if (sixbit > 63) {
            free(str);
            return PARSE_ERROR;
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

    *out = str;
    return PARSE_OK;
}
