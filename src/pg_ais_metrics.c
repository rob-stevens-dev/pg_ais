#include "pg_ais_metrics.h"
#include "funcapi.h"
#include "utils/builtins.h"


static uint64_t total_messages_parsed = 0;
static uint64_t total_parse_failures = 0;
static uint64_t total_reassembly_attempts = 0;
static uint64_t total_reassembly_success = 0;


/**
 * @brief Increment metrics counters after each parse
 *
 * Should be called from inside pg_ais_parse to track success/failure.
 *
 * @param success True if parsing succeeded (PARSE_OK), false otherwise
 */
void pg_ais_record_parse_result(bool success) {
    total_messages_parsed++;
    if (!success) total_parse_failures++;
}


/**
 * @brief Record an attempt to reassemble a multipart AIS message
 *
 * Tracks how many reassembly attempts occurred and how many succeeded.
 * Should be called after parse_ais_payload from try_reassemble.
 *
 * @param success True if reassembly led to a successful parse
 */
void pg_ais_record_reassembly_attempt(bool success) {
    total_reassembly_attempts++;
    if (success) total_reassembly_success++;
}


/**
 * @brief SQL-accessible function to expose internal parse metrics
 *
 * Returns a single row with total parses and failures.
 */
PG_FUNCTION_INFO_V1(pg_ais_metrics);
Datum pg_ais_metrics(PG_FUNCTION_ARGS) {
    TupleDesc tupdesc;
    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        ereport(ERROR, (errmsg("invalid return type for pg_ais_metrics")));

    Datum values[4];
    bool nulls[4] = {false, false, false, false};

    values[0] = Int64GetDatum(total_messages_parsed);
    values[1] = Int64GetDatum(total_parse_failures);
    values[2] = Int64GetDatum(total_reassembly_attempts);
    values[3] = Int64GetDatum(total_reassembly_success);

    HeapTuple tuple = heap_form_tuple(tupdesc, values, nulls);
    PG_RETURN_DATUM(HeapTupleGetDatum(tuple));
}


/**
 * @brief Reset internal counters to zero (optional utility)
 */
PG_FUNCTION_INFO_V1(pg_ais_reset_metrics);
Datum pg_ais_reset_metrics(PG_FUNCTION_ARGS) {
    total_messages_parsed = 0;
    total_parse_failures = 0;
    total_reassembly_attempts = 0;
    total_reassembly_success = 0;
    PG_RETURN_VOID();
}

