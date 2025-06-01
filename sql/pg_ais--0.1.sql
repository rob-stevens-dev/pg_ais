-- Declare the input/output functions before defining the type
CREATE FUNCTION ais_in(cstring)
RETURNS ais
AS 'pg_ais', 'ais_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION ais_out(ais)
RETURNS cstring
AS 'pg_ais', 'ais_out'
LANGUAGE C IMMUTABLE STRICT;

-- Define the custom 'ais' type
CREATE TYPE ais (
    INPUT = ais_in,
    OUTPUT = ais_out,
    INTERNALLENGTH = VARIABLE,
    STORAGE = EXTENDED
);

-- Functional entrypoint: AIS parsing
CREATE FUNCTION pg_ais_parse(ais)
RETURNS jsonb
AS 'pg_ais', 'pg_ais_parse'
LANGUAGE C IMMUTABLE STRICT;

-- Debug support
CREATE FUNCTION pg_ais_debug(text, text DEFAULT 'json') RETURNS jsonb
AS 'MODULE_PATHNAME', 'pg_ais_debug'
LANGUAGE C STRICT;
