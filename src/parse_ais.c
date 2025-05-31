#include "parse_ais.h"
#include "ais_core.h"
#include "pg_ais.h"
#include <utils/builtins.h>

char *ais_debug_json(const AISMessage *msg) {
    StringInfoData buf;
    initStringInfo(&buf);
    appendStringInfo(&buf,
        "{\"talker\":\"%s\",\"type\":\"%s\",\"total\":%d,\"num\":%d,\"seq_id\":\"%c\",\"channel\":\"%c\",\"payload\":\"%s\",\"fill_bits\":%d,\"message_id\":%u}",
        msg->talker, msg->type, msg->total, msg->num, msg->seq_id,
        msg->channel, msg->payload, msg->fill_bits, msg->message_id);
    return buf.data;
}

PG_FUNCTION_INFO_V1(pg_ais_debug);
Datum pg_ais_debug(PG_FUNCTION_ARGS) {
    ais *input = (ais *) PG_GETARG_POINTER(0);
    char *str = ais_to_cstring(input);

    AISMessage msg = {0};
    if (!parse_ais_sentence(str, &msg)) {
        pfree(str);
        PG_RETURN_NULL();
    }

    char *json = ais_debug_json(&msg);
    Datum result = DirectFunctionCall1(jsonb_in, CStringGetDatum(json));
    pfree(str);
    pfree(json);
    PG_RETURN_DATUM(result);
}
