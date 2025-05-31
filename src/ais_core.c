#include "ais_core.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static uint8_t decode_payload_type(const char *payload) {
    if (!payload || strlen(payload) < 1) return 0;
    unsigned char ch = payload[0];
    if (ch < 48 || ch > 119) return 0;
    ch -= 48;
    if (ch > 40) ch -= 8;
    return ch >> 2;  // top 6 bits contain message type
}

static bool validate_checksum(const char *sentence) {
    const char *star = strrchr(sentence, '*');
    if (!star || star - sentence > AIS_MAX_SENTENCE_LEN - 3) return false;

    unsigned int checksum = 0;
    for (const char *p = sentence + 1; *p && *p != '*'; ++p)
        checksum ^= (unsigned char)*p;

    unsigned int expected;
    if (sscanf(star + 1, "%2x", &expected) != 1) return false;
    return checksum == expected;
}

bool parse_ais_sentence(const char *sentence, AISMessage *msg) {
    if (!sentence || !msg || sentence[0] != '!') return false;
    if (!validate_checksum(sentence)) return false;

    char copy[AIS_MAX_SENTENCE_LEN];
    strncpy(copy, sentence, AIS_MAX_SENTENCE_LEN);
    copy[AIS_MAX_SENTENCE_LEN - 1] = '\0';

    char *tokens[10] = {0};
    int tok_count = 0;
    for (char *p = strtok(copy, ","); p && tok_count < 10; p = strtok(NULL, ","))
        tokens[tok_count++] = p;

    if (tok_count < 7) return false;

    strncpy(msg->talker, tokens[0] + 1, 2);  // skip '!'
    msg->talker[2] = '\0';
    strncpy(msg->type, tokens[0] + 3, 3);
    msg->type[3] = '\0';

    msg->total = atoi(tokens[1]);
    msg->num = atoi(tokens[2]);
    msg->seq_id = tokens[3][0];
    msg->channel = tokens[4][0];
    strncpy(msg->payload, tokens[5], sizeof(msg->payload) - 1);
    msg->payload[sizeof(msg->payload) - 1] = '\0';
    msg->fill_bits = atoi(tokens[6]);
    msg->message_id = decode_payload_type(tokens[5]);
    msg->valid = true;

    return true;
}
