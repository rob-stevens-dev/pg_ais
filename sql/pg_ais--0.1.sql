-- sql/pg_ais--0.1.sql
-- Initial version of the pg_ais extension

CREATE FUNCTION pg_ais_debug(msg ais)
RETURNS jsonb
AS 'pg_ais', 'pg_ais_debug'
LANGUAGE C STRICT;
