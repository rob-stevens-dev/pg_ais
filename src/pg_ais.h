#ifndef PG_AIS_H
#define PG_AIS_H

#include "postgres.h"
#include "fmgr.h"
#include "utils/varlena.h"

#define MAX_PARTS 5

#ifndef AIS_ALLOC
    #ifdef PG_EXTENSION
        #define AIS_ALLOC palloc
        #define AIS_FREE pfree
        #define AIS_STRDUP pstrdup
    #else
        #include <stdlib.h>
        #include <string.h>
        #define AIS_ALLOC malloc
        #define AIS_FREE free
        #define AIS_STRDUP strdup
    #endif
#endif


// PostgreSQL varlena wrapper
/**
 * @brief PostgreSQL varlena wrapper for AIS messages
 *
 * This struct is the PostgreSQL-compatible representation of an AIS sentence
 * stored as a custom type. It wraps the varlena header and provides access
 * to the raw message payload for parsing and indexing.
 */
typedef struct ais {
    struct varlena vl;  // must be struct-qualified here
} ais;


// AIS Message structures
/**
 * @brief Structure representing a single AIS NMEA fragment
 *
 * Stores metadata and payload from one !AIVDM sentence. Used as part of
 * fragment reassembly when messages span multiple parts.
 */
typedef struct {
    char *payload;
    int total;
    int seq;
    char message_id[9];
    char channel;
    int fill_bits;
    char *raw;
} AISFragment;


/**
 * @brief Buffer to hold fragments of a multipart AIS message
 *
 * Used during message reassembly. Holds up to MAX_PARTS fragment pointers
 * and tracks how many have been received.
 */
typedef struct {
    AISFragment *parts[MAX_PARTS];
    int received;
} AISFragmentBuffer;


/**
 * @brief Decoded representation of a full AIS message
 *
 * Parsed result after reassembling and decoding one or more AIS fragments.
 * Contains fields commonly used in vessel tracking and analytics.
 */
typedef struct {
    int type;
    int mmsi;
    float lat;
    float lon;
    float speed;
    float heading;
    uint32_t imo;
    char *callsign;
    char *vessel_name;
} AISMessage;


/**
 * @brief Convert a PostgreSQL ais varlena value to a C-string
 *
 * This safely extracts the sentence content from an ais varlena wrapper.
 *
 * @param value Pointer to ais varlena value
 * @return Null-terminated string (caller must free), or NULL on failure
 */
char *ais_to_cstring(const ais *value);


/**
 * @brief Wrap a raw AIS NMEA sentence as a PostgreSQL varlena value
 *
 * Validates that the input string starts with '!' and wraps it as an ais datum.
 *
 * @param str Null-terminated NMEA 0183 AIS sentence string
 * @return New ais varlena wrapper (palloc'd or malloc'd), or NULL on error
 */
ais *ais_from_cstring_external(const char *str);


/**
 * @brief Return the specified string field from an AIS message
 *
 * Usage: pg_ais_get_text_field(sentence, 'shipname')
 */
Datum pg_ais_get_text_field(PG_FUNCTION_ARGS);


/**
 * @brief Return a debug JSONB object containing all parsed fields
 *
 * Usage: SELECT pg_ais_debug(sentence);
 */
PGDLLEXPORT Datum pg_ais_debug(PG_FUNCTION_ARGS);


/**
 * @brief Return a set of named fields for a given AIS message
 *
 * This is a set-returning function (SRF) that returns key/value pairs.
 */
PGDLLEXPORT Datum pg_ais_fields(PG_FUNCTION_ARGS);


/**
 * @brief Return a PostGIS-compatible geometry Point (lon, lat)
 *
 * Usage: SELECT pg_ais_point(sentence);
 */
PGDLLEXPORT Datum pg_ais_point(PG_FUNCTION_ARGS);


/**
 * @brief Alias for pg_ais_point() for PostGIS geometry
 */
PGDLLEXPORT Datum pg_ais_point_geom(PG_FUNCTION_ARGS);


/**
 * @brief Return the specified integer field from an AIS message
 *
 * Usage: pg_ais_get_int_field(sentence, 'mmsi')
 */
PGDLLEXPORT Datum pg_ais_get_int_field(PG_FUNCTION_ARGS);


/**
 * @brief Return the specified floating point field from an AIS message
 *
 * Usage: pg_ais_get_float_field(sentence, 'speed')
 */
PGDLLEXPORT Datum pg_ais_get_float_field(PG_FUNCTION_ARGS);


/**
 * @brief Return the specified boolean field from an AIS message
 *
 * Usage: pg_ais_get_bool_field(sentence, 'raim')
 */
PGDLLEXPORT Datum pg_ais_get_bool_field(PG_FUNCTION_ARGS);

#endif