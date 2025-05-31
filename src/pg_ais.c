#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"


#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(pg_ais_test);

Datum
pg_ais_test(PG_FUNCTION_ARGS)
{
    PG_RETURN_TEXT_P(cstring_to_text("pg_ais initialized"));
}
