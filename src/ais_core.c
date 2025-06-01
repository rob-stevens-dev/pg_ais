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


#define ADD_NUMERIC_FIELD(key, valexpr) \
    do { \
        JsonbValue _v = {.type = jbvNumeric}; \
        _v.val.numeric = int_to_numeric(valexpr); \
        pushJsonbValue(&state, WJB_KEY, &((JsonbValue){.type = jbvString, .val.string.val = key, .val.string.len = strlen(key)})); \
        pushJsonbValue(&state, WJB_VALUE, &_v); \
    } while(0)

#define ADD_FLOAT_FIELD(key, valexpr) \
    do { \
        char _buf[32]; \
        snprintf(_buf, sizeof(_buf), "%.6f", valexpr); \
        JsonbValue _v = {.type = jbvString}; \
        _v.val.string.val = _buf; \
        _v.val.string.len = strlen(_buf); \
        pushJsonbValue(&state, WJB_KEY, &((JsonbValue){.type = jbvString, .val.string.val = key, .val.string.len = strlen(key)})); \
        pushJsonbValue(&state, WJB_VALUE, &_v); \
    } while(0)

#define ADD_STRING_FIELD(key, valexpr) \
    do { \
        if (valexpr) { \
            JsonbValue _v = {.type = jbvString}; \
            _v.val.string.val = (char *)valexpr; \
            _v.val.string.len = strlen(valexpr); \
            pushJsonbValue(&state, WJB_KEY, &((JsonbValue){.type = jbvString, .val.string.val = key, .val.string.len = strlen(key)})); \
            pushJsonbValue(&state, WJB_VALUE, &_v); \
        } \
    } while(0)


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
    const char *format = "json";

    if (PG_NARGS() == 2 && !PG_ARGISNULL(1)) {
        format = text_to_cstring(PG_GETARG_TEXT_PP(1));
    }

    char *input = text_to_cstring(txt);
    AISMessage msg;
    if (!parse_ais_from_text(input, &msg)) {
        ereport(ERROR, (errmsg("invalid AIS message")));
    }

    JsonbParseState *state = NULL;
    pushJsonbValue(&state, WJB_BEGIN_OBJECT, NULL);

    #define JSONB_BOOL(field, val) \
        do { \
            pushJsonbValue(&state, WJB_KEY, &((JsonbValue){.type=JB_STRING, .val.stringVal=field})); \
            pushJsonbValue(&state, WJB_VALUE, &((JsonbValue){.type=JB_BOOL, .val.boolean=(val != 0)})); \
        } while(0)

    #define JSONB_INT(field, val) \
        do { \
            pushJsonbValue(&state, WJB_KEY, &((JsonbValue){.type=JB_STRING, .val.stringVal=field})); \
            pushJsonbValue(&state, WJB_VALUE, &((JsonbValue){.type=JB_NUMERIC, .val.numeric=int64_to_numeric(val)})); \
        } while(0)

    #define JSONB_FLOAT(field, val) \
        do { \
            pushJsonbValue(&state, WJB_KEY, &((JsonbValue){.type=JB_STRING, .val.stringVal=field})); \
            pushJsonbValue(&state, WJB_VALUE, &((JsonbValue){.type=JB_NUMERIC, .val.numeric=float8_to_numeric(val)})); \
        } while(0)

    JSONB_INT("type", msg.type);
    JSONB_INT("mmsi", msg.mmsi);
    JSONB_FLOAT("lat", msg.lat);
    JSONB_FLOAT("lon", msg.lon);
    JSONB_FLOAT("speed", msg.speed);
    JSONB_FLOAT("heading", msg.heading);

    if (strcmp(format, "json_bool") == 0) {
        JSONB_BOOL("raim", msg.raim);
        JSONB_BOOL("accuracy", msg.accuracy);
        JSONB_BOOL("alt_sensor", msg.alt_sensor);
        JSONB_BOOL("retransmit", msg.retransmit);
    } else {
        JSONB_INT("raim", msg.raim);
        JSONB_INT("accuracy", msg.accuracy);
        JSONB_INT("alt_sensor", msg.alt_sensor);
        JSONB_INT("retransmit", msg.retransmit);
    }

    pushJsonbValue(&state, WJB_END_OBJECT, NULL);

    Jsonb *result = JsonbValueToJsonb(pushJsonbValue(&state, WJB_DONE, NULL));
    free_ais_message(&msg);
    PG_RETURN_JSONB_P(result);
}


PG_FUNCTION_INFO_V1(pg_ais_fields);
Datum
pg_ais_fields(PG_FUNCTION_ARGS) {
    text *txt = PG_GETARG_TEXT_PP(0);
    char *input = text_to_cstring(txt);

    AISMessage msg;
    if (!parse_ais_from_text(input, &msg)) {
        ereport(ERROR, (errmsg("invalid AIS message")));
    }

    TupleDesc tupdesc;
    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        ereport(ERROR, (errmsg("return type must be a row type")));

    Datum values[20];
    bool nulls[20] = {false};

    values[0] = Int32GetDatum(msg.type);
    values[1] = Int32GetDatum(msg.mmsi);
    values[2] = Int32GetDatum(msg.nav_status);
    values[3] = Float8GetDatum(msg.lat);
    values[4] = Float8GetDatum(msg.lon);
    values[5] = Float8GetDatum(msg.speed);
    values[6] = Float8GetDatum(msg.heading);
    values[7] = Float8GetDatum(msg.course);
    values[8] = Int32GetDatum(msg.timestamp);
    values[9] = Int32GetDatum(msg.imo);
    values[10] = CStringGetTextDatum(msg.callsign ? msg.callsign : "");
    values[11] = CStringGetTextDatum(msg.vessel_name ? msg.vessel_name : "");
    values[12] = Int32GetDatum(msg.ship_type);
    values[13] = CStringGetTextDatum(msg.destination ? msg.destination : "");
    values[14] = Float8GetDatum(msg.draught);
    values[15] = Int32GetDatum(msg.maneuver);
    values[16] = Int32GetDatum(msg.fix_type);
    values[17] = Int32GetDatum(msg.radio);
    values[18] = Int32GetDatum(msg.repeat);
    values[19] = BoolGetDatum(msg.raim);

    HeapTuple tuple = heap_form_tuple(tupdesc, values, nulls);
    free_ais_message(&msg);
    PG_RETURN_DATUM(HeapTupleGetDatum(tuple));
}


PG_FUNCTION_INFO_V1(pg_ais_point);
Datum
pg_ais_point(PG_FUNCTION_ARGS) {
    text *txt = PG_GETARG_TEXT_PP(0);
    char *input = text_to_cstring(txt);

    AISMessage msg;
    if (!parse_ais_from_text(input, &msg)) {
        ereport(ERROR, (errmsg("invalid AIS message")));
    }

    // Validate lat/lon range
    if (msg.lat < -90 || msg.lat > 90 || msg.lon < -180 || msg.lon > 180) {
        free_ais_message(&msg);
        PG_RETURN_NULL();
    }

    POINT *point = palloc(sizeof(POINT));
    point->x = msg.lon;
    point->y = msg.lat;

    free_ais_message(&msg);
    PG_RETURN_POINT_P(point);
}

// WKB currently big-endian; future support for LE should be considered
// TODO: Add byte-order handling (endianness detection) if needed
PG_FUNCTION_INFO_V1(pg_ais_point_geom);
Datum
pg_ais_point_geom(PG_FUNCTION_ARGS) {
    text *txt = PG_GETARG_TEXT_PP(0);
    char *input = text_to_cstring(txt);

    AISMessage msg;
    if (!parse_ais_from_text(input, &msg)) {
        ereport(ERROR, (errmsg("invalid AIS message")));
    }

    if (msg.lat < -90 || msg.lat > 90 || msg.lon < -180 || msg.lon > 180) {
        free_ais_message(&msg);
        PG_RETURN_NULL();
    }

    // WKB format: https://en.wikipedia.org/wiki/Well-known_text_representation_of_geometry#Well-known_binary
    // SRID: 4326, GeometryType: 1 (Point)
    uint8_t wkb[21] = {0x00}; // Big endian
    wkb[0] = 0;               // big endian
    wkb[1] = 0; wkb[2] = 0; wkb[3] = 0; wkb[4] = 0; // type placeholder
    wkb[1] = 0x00; wkb[2] = 0x00; wkb[3] = 0x00; wkb[4] = 0x01; // WKBPoint

    memcpy(&wkb[5], &msg.lon, 8);
    memcpy(&wkb[13], &msg.lat, 8);

    free_ais_message(&msg);
    PG_RETURN_BYTEA_P(cstring_to_text_with_len((const char *)wkb, sizeof(wkb)));
}


PG_FUNCTION_INFO_V1(pg_ais_get_text_field);
Datum
pg_ais_get_text_field(PG_FUNCTION_ARGS)
{
    bytea *raw = PG_GETARG_BYTEA_P(0);
    text *fieldname = PG_GETARG_TEXT_P(1);
    AISMessage msg;
    char *cstr = text_to_cstring(fieldname);

    if (!parse_ais_message(raw, &msg)) {
        PG_RETURN_NULL();
    }

    if (strcmp(cstr, "shipname") == 0 && msg.shipname) {
        PG_RETURN_TEXT_P(cstring_to_text(msg.shipname));
    } else if (strcmp(cstr, "callsign") == 0 && msg.callsign) {
        PG_RETURN_TEXT_P(cstring_to_text(msg.callsign));
    }

    PG_RETURN_NULL();
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
    if (msg->destination) {
        free(msg->destination);
        msg->destination = NULL;
    }
    if (msg->bin_data) {
        free(msg->bin_data);
        msg->bin_data = NULL;
    }
}

const char* ais_nav_status_to_str(int code) {
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
        case 9 ... 13: return "Reserved for future use";
        case 14: return "AIS-SART";
        case 15: return "Not defined (default)";
        default: return "Unknown";
    }
}

const char* ais_maneuver_to_str(int code) {
    switch (code) {
        case 0: return "Not available";
        case 1: return "No special maneuver";
        case 2: return "Special maneuver";
        default: return "Unknown";
    }
}

const char* ais_fix_type_to_str(int code) {
    switch (code) {
        case 0: return "Undefined";
        case 1: return "GPS";
        case 2: return "GLONASS";
        case 3: return "Combined GPS/GLONASS";
        case 4: return "Loran-C";
        case 5: return "Chayka";
        case 6: return "Integrated Navigation System";
        case 7: return "Surveyed";
        default: return "Unknown";
    }
}

const char* ais_ship_type_to_str(int code) {
    switch (code) {
        case 0: return "Not available (default)";
        case 30: return "Fishing";
        case 31: return "Towing";
        case 32: return "Towing exceeds 200m or wide";
        case 33: return "Dredging or underwater ops";
        case 34: return "Diving ops";
        case 35: return "Military ops";
        case 36: return "Sailing";
        case 37: return "Pleasure Craft";
        case 70: return "Cargo";
        case 80: return "Tanker";
        case 90: return "Other type";
        default: return "Unknown or Reserved";
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