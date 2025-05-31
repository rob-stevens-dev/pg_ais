/* parse_ais.h */
#ifndef PARSE_AIS_H
#define PARSE_AIS_H

#include "ais_core.h"
#include <postgres.h>
#include <fmgr.h>
#include <utils/jsonb.h>

/* Wraps AISMessage as JSONB debug output */
char *ais_debug_json(const AISMessage *msg);
Datum pg_ais_debug(PG_FUNCTION_ARGS);

#endif /* PARSE_AIS_H */
