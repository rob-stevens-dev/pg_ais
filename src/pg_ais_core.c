#include "pg_ais.h"
#include <string.h>

/**
 * @brief Wrap a raw AIS NMEA sentence as a PostgreSQL varlena value
 *
 * Validates that the input string starts with '!' and wraps it as an ais datum.
 *
 * @param str Null-terminated NMEA 0183 AIS sentence string
 * @return New ais varlena wrapper (palloc'd or malloc'd), or NULL on error
 */
ais *ais_from_cstring_external(const char *str) {
    if (!str || str[0] != '!') return NULL;
    size_t len = strlen(str);
    ais *result = (ais *) AIS_ALLOC(VARHDRSZ + len);
    if (!result) return NULL;
    SET_VARSIZE(result, VARHDRSZ + len);
    memcpy(VARDATA(result), str, len);
    return result;
}


/**
 * @brief Convert a PostgreSQL ais varlena value to a C-string
 *
 * This safely extracts the sentence content from an ais varlena wrapper.
 *
 * @param value Pointer to ais varlena value
 * @return Null-terminated string (caller must free), or NULL on failure
 */
char *ais_to_cstring(const ais *val) {
    if (!val || VARSIZE(val) <= VARHDRSZ) return NULL;
    size_t len = VARSIZE(val) - VARHDRSZ;
    char *out = (char *) AIS_ALLOC(len + 1);
    if (!out) return NULL;
    memcpy(out, VARDATA(val), len);
    out[len] = '\0';
    return out;
}
