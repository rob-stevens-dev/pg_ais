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
typedef struct ais {
    struct varlena vl;  // must be struct-qualified here
} ais;

// AIS Message structures
typedef struct {
    char *payload;
    int total;
    int seq;
    char message_id[9];
    char channel;
    int fill_bits;
    char *raw;
} AISFragment;

typedef struct {
    AISFragment *parts[MAX_PARTS];
    int received;
} AISFragmentBuffer;

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


// C-string converters
char *ais_to_cstring(const ais *value);
ais *ais_from_cstring_external(const char *str);

Datum pg_ais_get_text_field(PG_FUNCTION_ARGS);

PGDLLEXPORT Datum pg_ais_debug(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum pg_ais_fields(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum pg_ais_point(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum pg_ais_point_geom(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum pg_ais_get_int_field(PG_FUNCTION_ARGS);

#endif