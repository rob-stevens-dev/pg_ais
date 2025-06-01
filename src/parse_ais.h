#ifndef PARSE_AIS_H
#define PARSE_AIS_H

#include <stdbool.h>
#include <stdint.h>
#include "pg_ais.h"

#define MAX_PARTS 5

bool parse_ais_fragment(const char *sentence, AISFragment *frag);
char *parse_string_utf8(const char *payload, int start, int bitlen);
bool try_reassemble(AISFragmentBuffer *buffer, AISMessage *msg_out);
void reset_buffer(AISFragmentBuffer *buffer);
void free_buffer(AISFragmentBuffer *buffer);

#endif