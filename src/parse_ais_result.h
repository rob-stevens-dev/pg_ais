#ifndef PARSE_AIS_RESULT_H
#define PARSE_AIS_RESULT_H

/* parse_ais_result.h - structured error model for AIS parsing */
#pragma once

/**
 * @brief Error codes returned by parsing functions.
 */
typedef enum {
    PARSE_OK = 0,                  ///< No error occurred
    PARSE_ERR_TOO_SHORT,          ///< Payload was too short for bit extraction
    PARSE_ERR_INVALID_BITFIELD,   ///< Bitfield could not be parsed into expected type
    PARSE_ERR_UNSUPPORTED_TYPE,   ///< Message type is unsupported
    PARSE_ERR_STRING_DECODE,      ///< Failed to decode string
    PARSE_ERR_PAYLOAD_NULL        ///< Payload pointer was NULL
} ParseErrorCode;

/**
 * @brief Represents the result of a parsing operation, including status and error info.
 */
typedef struct {
    bool ok;                      ///< True if parse succeeded
    ParseErrorCode code;          ///< Detailed error code if failed
    const char *msg;              ///< Optional human-readable error message
} ParseResult;

#endif