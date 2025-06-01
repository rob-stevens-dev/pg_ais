#include "pg_ais.h"
#include <string.h>

ais *ais_from_cstring_external(const char *str) {
    if (!str || str[0] != '!') return NULL;
    size_t len = strlen(str);
    ais *result = (ais *) AIS_ALLOC(VARHDRSZ + len);
    if (!result) return NULL;
    SET_VARSIZE(result, VARHDRSZ + len);
    memcpy(VARDATA(result), str, len);
    return result;
}

char *ais_to_cstring(const ais *val) {
    size_t len = VARSIZE(val) - VARHDRSZ;
    char *out = (char *) AIS_ALLOC(len + 1);
    if (!out) return NULL;
    memcpy(out, VARDATA(val), len);
    out[len] = '\0';
    return out;
}