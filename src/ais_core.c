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

PG_FUNCTION_INFO_V1(pg_ais_debug);
Datum
pg_ais_debug(PG_FUNCTION_ARGS) {
    text *txt = PG_GETARG_TEXT_PP(0);
    text *fmt = PG_NARGS() > 1 ? PG_GETARG_TEXT_PP(1) : NULL;
    char *input = text_to_cstring(txt);
    char *format = fmt ? text_to_cstring(fmt) : "json";

    bool format_enum_output = (strcmp(format, "json_enum") == 0);

    AISMessage msg;
    if (!parse_ais_from_text(input, &msg)) {
        ereport(ERROR, (errmsg("invalid AIS message")));
    }

    JsonbParseState *state = NULL;
    pushJsonbValue(&state, WJB_BEGIN_OBJECT, NULL);

    if (format_enum_output) {
        append_jsonb_enum_field(&state, "nav_status", msg.nav_status, nav_status_enum);
    } else {
        JsonbValue val = {.type = jbvNumeric, .val.numeric = int_to_numeric(msg.nav_status)};
        pushJsonbValue(&state, WJB_KEY, &((JsonbValue){.type = jbvString, .val.string.val = "nav_status", .val.string.len = 10}));
        pushJsonbValue(&state, WJB_VALUE, &val);
    }

    pushJsonbValue(&state, WJB_END_OBJECT, NULL);
    Jsonb *result = JsonbValueToJsonb(pushJsonbValue(&state, WJB_END_OBJECT, NULL));

    free_ais_message(&msg);
    PG_RETURN_JSONB_P(result);
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
    if (msg->bin_data) {
        free(msg->bin_data);
        msg->bin_data = NULL;
    }
}

const char* nav_status_enum(int code) {
    switch (code) {
        case 0: return "Under way using engine";
        case 1: return "At anchor";
        case 2: return "Not under command";
        case 3: return "Restricted maneuverability";
        case 4: return "Constrained by her draught";
        case 5: return "Moored";
        case 6: return "Aground";
        case 7: return "Engaged in fishing";
        case 8: return "Under way sailing";
        case 9: return "Reserved for future use (9)";
        case 10: return "Reserved for future use (10)";
        case 11: return "Reserved for future use (11)";
        case 12: return "Reserved for future use (12)";
        case 13: return "Reserved for future use (13)";
        case 14: return "AIS-SART";
        case 15: return "Not defined (default)";
        default: return "Invalid";
    }
}

static void append_jsonb_enum_field(JsonbParseState **state, const char *key, int value, const char *(*enum_func)(int)) {
    pushJsonbValue(state, WJB_KEY, &((JsonbValue){.type = jbvString, .val.string.val = (char *)key, .val.string.len = strlen(key)}));
    pushJsonbValue(state, WJB_BEGIN_OBJECT, NULL);

    JsonbValue val = {.type = jbvNumeric};
    val.val.numeric = int_to_numeric(value);
    pushJsonbValue(state, WJB_KEY, &((JsonbValue){.type = jbvString, .val.string.val = "value", .val.string.len = 5}));
    pushJsonbValue(state, WJB_VALUE, &val);

    const char *label = enum_func(value);
    pushJsonbValue(state, WJB_KEY, &((JsonbValue){.type = jbvString, .val.string.val = "label", .val.string.len = 5}));
    pushJsonbValue(state, WJB_VALUE, &((JsonbValue){.type = jbvString, .val.string.val = (char *)label, .val.string.len = strlen(label)}));

    pushJsonbValue(state, WJB_END_OBJECT, NULL);
}