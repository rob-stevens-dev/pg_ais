#ifndef AIS_CORE_H
#define AIS_CORE_H

#include <stdbool.h>
#include <stdint.h>

#define AIS_MAX_SENTENCE_LEN 1024

typedef struct {
    char talker[3];       // "AI"
    char type[6];         // "VDM" or "VDO"
    int total;            // 1 for single-part
    int num;              // part number (e.g. 1 of 1)
    char seq_id;          // sequence ID (if any)
    char channel;         // A or B
    char payload[900];    // 6-bit encoded payload
    int fill_bits;
    uint8_t message_id;   // decoded message ID
    bool valid;
} AISMessage;

bool parse_ais_sentence(const char *sentence, AISMessage *msg);

#endif /* AIS_CORE_H */
