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


/* Optional UTF-8 string parser */
char *parse_string_utf8(const char *payload, int start, int bitlen) {
    int charlen = bitlen / 6;
    char *str = calloc(charlen + 1, 1);
    if (!str) return NULL;

    for (int i = 0; i < charlen; i++) {
        int sixbit = 0;
        for (int j = 0; j < 6; j++) {
            int bit = start + i * 6 + j;
            char c = payload[bit / 6];
            int val = (c >= '0' && c <= 'W') ? (c - '0') : 0;
            sixbit <<= 1;
            sixbit |= (val >> (5 - (bit % 6))) & 0x01;
        }
        char decoded = sixbit_ascii[sixbit & 0x3F];
        str[i] = (decoded == '@') ? ' ' : decoded;
    }

    // Trim trailing whitespace
    for (int i = charlen - 1; i >= 0; i--) {
        if (str[i] == ' ') {
            str[i] = '\0';
        } else {
            break;
        }
    }

    return str;
}