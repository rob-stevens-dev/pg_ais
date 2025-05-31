// File: src/parse_ais_msg.h
#ifndef PARSE_AIS_MSG_H
#define PARSE_AIS_MSG_H

#include "pg_ais.h"

bool parse_ais_payload(AISMessage *msg, const char *payload, int fill_bits);

#endif
