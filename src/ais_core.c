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


/**
 * @brief Append a numeric field to a JSONB object.
 *
 * Encodes an integer or long value into a JSONB object under the given key.
 *
 * @param key   Field name to emit
 * @param val   Numeric value to encode
 */
#define ADD_NUMERIC_FIELD(key, valexpr) \
    do { \
        JsonbValue _v = {.type = jbvNumeric}; \
        _v.val.numeric = int_to_numeric(valexpr); \
        pushJsonbValue(&state, WJB_KEY, &((JsonbValue){.type = jbvString, .val.string.val = key, .val.string.len = strlen(key)})); \
        pushJsonbValue(&state, WJB_VALUE, &_v); \
    } while(0)


/**
 * @brief Append a floating-point field to a JSONB object.
 *
 * Emits a key-value pair where the value is a double precision number.
 *
 * @param key   Field name
 * @param val   Float or double value
 */
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


/**
 * @brief Append a string field to a JSONB object.
 *
 * Inserts a UTF-8 string under the specified key. Skips null values.
 *
 * @param key   Key name
 * @param val   String to store (nullable)
 */
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


/**
 * @brief Parse a varlena-encoded AIS value into an AISMessage struct.
 *
 * This function acts as a shared backend for all pg_ais_get_*_field functions.
 * It unpacks the input, validates it, and populates an in-memory AISMessage.
 *
 * @param ais_input Pointer to PostgreSQL 'ais' type (varlena)
 * @param msg_out   Output struct to populate (caller must free fields)
 * @return ParseResult indicating parse status
 */
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


/**
 * @brief Input function for the custom 'ais' PostgreSQL type.
 *
 * Accepts a C-string representing an AIS sentence and wraps it as varlena.
 *
 * @param str C-string input from SQL
 * @return ais* PostgreSQL varlena datum
 */
PG_FUNCTION_INFO_V1(ais_in);
Datum
ais_in(PG_FUNCTION_ARGS) {
    char *str = PG_GETARG_CSTRING(0);
    PG_RETURN_POINTER(ais_from_cstring_external(str));
}


/**
 * @brief Output function for the custom 'ais' PostgreSQL type.
 *
 * Converts the varlena-wrapped AIS sentence back to a null-terminated C-string
 * for display or external serialization.
 *
 * @param val Internal ais datum (varlena)
 * @return C-string output (PostgreSQL palloc'd)
 */
PG_FUNCTION_INFO_V1(ais_out);
Datum
ais_out(PG_FUNCTION_ARGS) {
    ais *val = (ais *) PG_GETARG_POINTER(0);
    PG_RETURN_CSTRING(ais_to_cstring(val));
}


/**
 * @brief Return a debug JSONB object containing all parsed fields
 *
 * Usage: SELECT pg_ais_debug(sentence);
 */
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


/**
 * @brief Return a set of named fields for a given AIS message
 *
 * This is a set-returning function (SRF) that returns key/value pairs.
 */
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


/**
 * @brief Alias for pg_ais_point() for PostGIS geometry
 */
PG_FUNCTION_INFO_V1(pg_ais_point);
Datum
pg_ais_point(PG_FUNCTION_ARGS) {
    bytea *raw = PG_GETARG_BYTEA_P(0);
    AISMessage msg;

    if (!parse_ais_message(raw, &msg)) {
        PG_RETURN_NULL();
    }

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
/**
 * @brief Alias for pg_ais_point() for PostGIS geometry
 */
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


/**
 * @brief Return the specified string field from an AIS message
 *
 * Usage: pg_ais_get_text_field(sentence, 'shipname')
 */
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


/**
 * @brief Return the specified integer field from an AIS message
 *
 * Usage: pg_ais_get_int_field(sentence, 'mmsi')
 */
PG_FUNCTION_INFO_V1(pg_ais_get_int_field);
Datum
pg_ais_get_int_field(PG_FUNCTION_ARGS) {
    bytea *raw = PG_GETARG_BYTEA_P(0);
    text *fieldname = PG_GETARG_TEXT_P(1);
    char *cstr = text_to_cstring(fieldname);
    AISMessage msg;

    if (!parse_ais_message(raw, &msg)) {
        PG_RETURN_NULL();
    }

    if (strcmp(cstr, "mmsi") == 0 && msg.mmsi > 0) {
        PG_RETURN_INT32(msg.mmsi);
    } else if (strcmp(cstr, "heading") == 0 && msg.heading >= 0 && msg.heading <= 359) {
        PG_RETURN_INT32(msg.heading);
    } else if (strcmp(cstr, "nav_status") == 0 && msg.nav_status >= 0 && msg.nav_status <= 15) {
        PG_RETURN_INT32(msg.nav_status);
    } else if (strcmp(cstr, "course") == 0 && msg.course >= 0 && msg.course <= 360) {
        PG_RETURN_INT32((int)(msg.course));
    }

    PG_RETURN_NULL();
}


/**
 * @brief Return the specified floating point field from an AIS message
 *
 * Usage: pg_ais_get_float_field(sentence, 'speed')
 */
PG_FUNCTION_INFO_V1(pg_ais_get_float_field);
Datum
pg_ais_get_float_field(PG_FUNCTION_ARGS) {
    bytea *raw = PG_GETARG_BYTEA_P(0);
    text *fieldname = PG_GETARG_TEXT_P(1);
    char *cstr = text_to_cstring(fieldname);
    AISMessage msg;

    if (!parse_ais_message(raw, &msg)) {
        PG_RETURN_NULL();
    }

    if (strcmp(cstr, "speed") == 0 && msg.speed >= 0) {
        PG_RETURN_FLOAT8(msg.speed);
    } else if (strcmp(cstr, "course") == 0 && msg.course >= 0) {
        PG_RETURN_FLOAT8(msg.course);
    }

    PG_RETURN_NULL();
}


/**
 * @brief Return the specified boolean field from an AIS message
 *
 * Usage: pg_ais_get_bool_field(sentence, 'raim')
 */
PG_FUNCTION_INFO_V1(pg_ais_get_bool_field);
Datum
pg_ais_get_bool_field(PG_FUNCTION_ARGS) {
    bytea *raw = PG_GETARG_BYTEA_P(0);
    text *fieldname = PG_GETARG_TEXT_P(1);
    char *cstr = text_to_cstring(fieldname);
    AISMessage msg;

    if (!parse_ais_message(raw, &msg)) {
        PG_RETURN_NULL();
    }

    if (strcmp(cstr, "raim") == 0) {
        PG_RETURN_BOOL(msg.raim);
    } else if (strcmp(cstr, "accuracy") == 0) {
        PG_RETURN_BOOL(msg.accuracy);
    } else if (strcmp(cstr, "assigned") == 0) {
        PG_RETURN_BOOL(msg.assigned);
    }

    PG_RETURN_NULL();
}


/* Internal utility functions. */


/**
 * @brief Free heap-allocated components of an AISMessage struct
 *
 * Releases memory from dynamic fields (callsign, vessel_name, destination, bin_data).
 *
 * @param msg Pointer to AISMessage with heap fields to release
 */
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


/**
 * @brief Convert navigation status code to descriptive string
 *
 * Maps 0–15 navigation status codes to human-readable labels.
 *
 * @param code Navigation status integer code
 * @return Constant string label or "Unknown"
 */
const char* ais_nav_status_to_str(int code) {
    static const char *labels[] = {
        "Under way using engine", "At anchor", "Not under command", "Restricted manoeuverability",
        "Constrained by her draught", "Moored", "Aground", "Engaged in Fishing",
        "Under way sailing", "Reserved for future use", "Reserved for future use",
        "Power-driven vessel towing astern", "Power-driven vessel pushing ahead",
        "Power-driven vessel pushing ahead or towing alongside", "Reserved for future use", "Not defined"
    };
    return (code >= 0 && code <= 15) ? labels[code] : "Unknown";
}


/**
 * @brief Convert maneuver indicator code to descriptive string
 *
 * @param code Integer value (0–2)
 * @return Static description string or "Unknown"
 */
const char* ais_maneuver_to_str(int code) {
    static const char *labels[] = {
        "Not available", "No special maneuver", "Special maneuver"
    };
    return (code >= 0 && code <= 2) ? labels[code] : "Unknown";
}


/**
 * @brief Convert GPS fix type code to readable string
 *
 * @param code Fix type numeric value
 * @return Descriptive label (e.g. "GPS", "GLONASS")
 */
const char* ais_fix_type_to_str(int code) {
    static const char *labels[] = {
        "Undefined", "GPS", "GLONASS", "Combined GPS/GLONASS", "Loran-C", "Chayka",
        "Integrated navigation system", "Surveyed", "Galileo", "Reserved", "Other"
    };
    return (code >= 0 && code <= 10) ? labels[code] : "Unknown";
}


/**
 * @brief Convert ship type numeric code to vessel class string
 *
 * Uses ITU-R M.1371 Annex B codes.
 *
 * @param code Ship type numeric code
 * @return Descriptive string like "Tanker" or "Cargo"
 */
onst char* ais_ship_type_to_str(int code) {
    if (code >= 60 && code <= 69) return "Passenger";
    if (code >= 70 && code <= 79) return "Cargo";
    if (code >= 80 && code <= 89) return "Tanker";
    if (code >= 90 && code <= 99) return "Other";
    return "Unknown";
}


/**
 * @brief Append a JSONB object field with an enum value and its label.
 *
 * Adds a key to the current JSONB object being built, storing both the raw integer
 * code and the corresponding human-readable label from an enum converter.
 *
 * Example:
 *   "nav_status": { "code": 5, "label": "Moored" }
 *
 * @param state     Pointer to the JSONB parse state
 * @param key       Field name to add to the object
 * @param value     Enum value to encode
 * @param enum_func Function to convert value to label
 */
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