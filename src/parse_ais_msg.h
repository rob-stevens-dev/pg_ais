#ifndef PARSE_AIS_MSG_H
#define PARSE_AIS_MSG_H

#include "pg_ais.h"
#include "ais_core.h"
#include "shared_ais_utils.h"

/**
 * @brief Parses AIS message types 1, 2, and 3 (Class A position reports)
 */
bool parse_msg_1_2_3(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message types 4 and 11 (Base Station Reports)
 */
bool parse_msg_4_11(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 5 (Static and Voyage Related Data)
 */
bool parse_msg_5(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 6 (Binary Addressed Message)
 */
bool parse_msg_6(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 7 (Binary Acknowledge)
 */
bool parse_msg_7(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 8 (Binary Broadcast Message)
 */
bool parse_msg_8(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 9 (Standard SAR Aircraft Position Report)
 */
bool parse_msg_9(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 10 (UTC/Date Inquiry)
 */
bool parse_msg_10(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 12 (Safety-Related Message Addressed)
 */
bool parse_msg_12(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 13 (Safety-Related Acknowledge)
 */
bool parse_msg_13(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 14 (Safety-Related Broadcast Message)
 */
bool parse_msg_14(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 15 (Interrogation)
 */
bool parse_msg_15(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 16 (Assigned Mode Command)
 */
bool parse_msg_16(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 17 (DGNSS Broadcast Binary Message)
 */
bool parse_msg_17(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message types 18, 19, 24 (Class B Position Reports)
 */
bool parse_msg_18_19_24(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 20 (Data Link Management)
 */
bool parse_msg_20(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 21 (Aids-to-Navigation Report)
 */
bool parse_msg_21(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 22 (Channel Management)
 */
bool parse_msg_22(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 23 (Group Assignment Command)
 */
bool parse_msg_23(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 25 (Binary Message, Single Slot)
 */
bool parse_msg_25(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 26 (Binary Message, Multiple Slot)
 */
bool parse_msg_26(AISMessage *msg, const char *payload);

/**
 * @brief Parses AIS message type 27 (Long Range AIS Broadcast Message)
 */
bool parse_msg_27(AISMessage *msg, const char *payload);

/**
 * @brief Delegates to appropriate parser based on AIS message type
 */
bool parse_ais_payload(AISMessage *msg, const char *payload, int fill_bits);

#endif
