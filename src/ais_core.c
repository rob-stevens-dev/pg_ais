// ais_core.c
// (Empty placeholder for future AIS processing logic)

// pg_ais.c
#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/jsonb.h"
#include "utils/varlena.h"
#include "lib/stringinfo.h"

#include "pg_ais.h"
#include "parse_ais.h"

PG_MODULE_MAGIC;

static AISFragmentBuffer frag_buffer;

PG_FUNCTION_INFO_V1(pg_ais_parse);
Datum
pg_ais_parse(PG_FUNCTION_ARGS) {
    if (PG_ARGISNULL(0)) PG_RETURN_NULL();

    const ais *input = (ais *) PG_GETARG_POINTER(0);
    if (VARSIZE_ANY_EXHDR(input) <= 0) PG_RETURN_NULL();

    char *cstr = ais_to_cstring(input);
    if (!cstr) PG_RETURN_NULL();

    AISFragment *frag = (AISFragment *) AIS_ALLOC(sizeof(AISFragment));
    if (!parse_ais_fragment(cstr, frag)) {
        AIS_FREE(frag);
        AIS_FREE(cstr);
        PG_RETURN_NULL();
    }
    frag->raw = AIS_STRDUP(cstr);

    int idx = frag->seq - 1;
    if (idx < 0 || idx >= MAX_PARTS || frag_buffer.parts[idx]) {
        AIS_FREE(frag);
        AIS_FREE(cstr);
        PG_RETURN_NULL();
    }

    frag_buffer.parts[idx] = frag;
    frag_buffer.received++;

    AISMessage msg;
    if (!try_reassemble(&frag_buffer, &msg)) {
        AIS_FREE(cstr);
        PG_RETURN_NULL();
    }

    reset_buffer(&frag_buffer);

    StringInfoData json;
    initStringInfo(&json);
    appendStringInfo(&json, "{\"mmsi\":%d,\"lat\":%f,\"lon\":%f,\"speed\":%f,\"heading\":%f}",
                     msg.mmsi, msg.lat, msg.lon, msg.speed, msg.heading);

    AIS_FREE(cstr);
    free_ais_message(&msg);

    Datum result = DirectFunctionCall1(jsonb_in, CStringGetDatum(json.data));
    PG_RETURN_DATUM(result);
}

PG_FUNCTION_INFO_V1(ais_in);
Datum
ais_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);
    PG_RETURN_POINTER(ais_from_cstring_external(str));
}

PG_FUNCTION_INFO_V1(ais_out);
Datum
ais_out(PG_FUNCTION_ARGS) {
    ais *val = (ais *) PG_GETARG_POINTER(0);
    PG_RETURN_CSTRING(ais_to_cstring(val));
}


/* Internal utility functions. */
void free_ais_message(AISMessage *msg) {
    if (msg->callsign) {
        free(msg->callsign);
        msg->callsign = NULL;
    }
    if (msg->vessel_name) {
        free(msg->vessel_name);
        msg->vessel_name = NULL;
    }
}