#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"
#include "pg_ais.h"

PG_MODULE_MAGIC;

// Parse NMEA string and store as varlena
ais *ais_from_cstring(const char *str) {
    if (str == NULL || strlen(str) == 0)
        ereport(ERROR, (errmsg("AIS input must not be null or empty")));

    if (str[0] != '!')
        ereport(ERROR, (errmsg("AIS sentence must start with '!'")));

    size_t len = strlen(str);
    ais *result = (ais *) palloc(VARHDRSZ + len);
    SET_VARSIZE(result, VARHDRSZ + len);
    memcpy(VARDATA(result), str, len);
    return result;
}

// Convert ais varlena to null-terminated C string
char *ais_to_cstring(const ais *value) {
    size_t len = VARSIZE(value) - VARHDRSZ;
    char *result = (char *) palloc(len + 1);
    memcpy(result, VARDATA(value), len);
    result[len] = '\0';
    return result;
}

PG_FUNCTION_INFO_V1(ais_in);
Datum ais_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);
    ais *parsed = ais_from_cstring(str);
    PG_RETURN_POINTER(parsed);
}

PG_FUNCTION_INFO_V1(ais_out);
Datum ais_out(PG_FUNCTION_ARGS) {
    ais *value = (ais *) PG_GETARG_POINTER(0);
    PG_RETURN_CSTRING(ais_to_cstring(value));
}

PG_FUNCTION_INFO_V1(ais_recv);
Datum ais_recv(PG_FUNCTION_ARGS) {
    StringInfo buf = (StringInfo) PG_GETARG_POINTER(0);
    int32 len = pq_getmsgint(buf, 4);
    ais *result = (ais *) palloc(VARHDRSZ + len);
    SET_VARSIZE(result, VARHDRSZ + len);
    pq_copymsgbytes(buf, VARDATA(result), len);
    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(ais_send);
Datum ais_send(PG_FUNCTION_ARGS) {
    ais *val = (ais *) PG_GETARG_POINTER(0);
    StringInfoData buf;
    pq_begintypsend(&buf);
    pq_sendint32(&buf, VARSIZE(val) - VARHDRSZ);
    pq_sendbytes(&buf, VARDATA(val), VARSIZE(val) - VARHDRSZ);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(ais_eq);
Datum ais_eq(PG_FUNCTION_ARGS) {
    ais *a = (ais *) PG_GETARG_POINTER(0);
    ais *b = (ais *) PG_GETARG_POINTER(1);

    int lena = VARSIZE(a) - VARHDRSZ;
    int lenb = VARSIZE(b) - VARHDRSZ;

    if (lena != lenb)
        PG_RETURN_BOOL(false);

    PG_RETURN_BOOL(memcmp(VARDATA(a), VARDATA(b), lena) == 0);
}
