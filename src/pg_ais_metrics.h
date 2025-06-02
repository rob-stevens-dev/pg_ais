#ifndef PG_AIS_METRICS_H
#define PG_AIS_METRICS_H

#include "postgres.h"
#include "fmgr.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/**
 * @brief Increment metrics counters after each parse
 *
 * Should be called from inside pg_ais_parse to track success/failure.
 *
 * @param success True if parsing succeeded (PARSE_OK), false otherwise
 */
void pg_ais_record_parse_result(bool success);


/**
 * @brief Record an attempt to reassemble a multipart AIS message
 *
 * Tracks how many reassembly attempts occurred and how many succeeded.
 * Should be called after parse_ais_payload from try_reassemble.
 *
 * @param success True if reassembly led to a successful parse
 */
void pg_ais_record_reassembly_attempt(bool success);


/**
 * @brief SQL-accessible function to expose internal parse metrics
 *
 * Returns a single row with total parses and failures.
 */
PGDLLEXPORT Datum pg_ais_metrics(PG_FUNCTION_ARGS);

/**
 * @brief Reset internal counters to zero (optional utility)
 */
PGDLLEXPORT Datum pg_ais_reset_metrics(PG_FUNCTION_ARGS);

#endif