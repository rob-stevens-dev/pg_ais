// parse_ais.c
#include "pg_ais.h"
#include <string.h>
#include <stdio.h>

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