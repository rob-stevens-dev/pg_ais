#ifndef PARSE_AIS_MSG_H
#define PARSE_AIS_MSG_H

#include "pg_ais.h"
#include "ais_core.h"

bool parse_ais_payload(AISMessage *msg, const char *payload, int fill_bits);

bool parse_msg_1_2_3(AISMessage *msg, const char *payload);
bool parse_msg_4_11(AISMessage *msg, const char *payload);
bool parse_msg_5(AISMessage *msg, const char *payload);
bool parse_msg_6(AISMessage *msg, const char *payload);
bool parse_msg_7(AISMessage *msg, const char *payload);
bool parse_msg_8(AISMessage *msg, const char *payload);
bool parse_msg_9(AISMessage *msg, const char *payload);
bool parse_msg_10(AISMessage *msg, const char *payload);
bool parse_msg_12(AISMessage *msg, const char *payload);
bool parse_msg_13(AISMessage *msg, const char *payload);
bool parse_msg_14(AISMessage *msg, const char *payload);
bool parse_msg_15(AISMessage *msg, const char *payload);
bool parse_msg_16(AISMessage *msg, const char *payload);
bool parse_msg_17(AISMessage *msg, const char *payload);
bool parse_msg_18_19_24(AISMessage *msg, const char *payload);
bool parse_msg_20(AISMessage *msg, const char *payload);
bool parse_msg_21(AISMessage *msg, const char *payload);
bool parse_msg_22(AISMessage *msg, const char *payload);
bool parse_msg_23(AISMessage *msg, const char *payload);
bool parse_msg_25(AISMessage *msg, const char *payload);
bool parse_msg_26(AISMessage *msg, const char *payload);
bool parse_msg_27(AISMessage *msg, const char *payload);
bool parse_ais_payload(AISMessage *msg, const char *payload, int fill_bits);

#endif