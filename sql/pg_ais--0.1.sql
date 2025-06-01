-- Declare the input/output functions before defining the type
CREATE OR REPLACE FUNCTION ais_in(cstring)
RETURNS ais
AS 'pg_ais', 'ais_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ais_out(ais)
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
CREATE OR REPLACE FUNCTION pg_ais_parse(ais)
RETURNS jsonb
AS 'pg_ais', 'pg_ais_parse'
LANGUAGE C IMMUTABLE STRICT;

-- pg_ais_debug
CREATE OR REPLACE FUNCTION pg_ais_debug(sentence text, format text DEFAULT 'json')
RETURNS jsonb
AS 'MODULE_PATHNAME', 'pg_ais_debug'
LANGUAGE C STRICT;


-- Tabular support
CREATE OR REPLACE FUNCTION pg_ais_fields(text)
RETURNS TABLE (
    type integer,
    mmsi integer,
    nav_status integer,
    lat double precision,
    lon double precision,
    speed double precision,
    heading double precision,
    course double precision,
    timestamp integer,
    imo integer,
    callsign text,
    vessel_name text,
    ship_type integer,
    destination text,
    draught double precision,
    maneuver integer,
    fix_type integer,
    radio integer,
    repeat integer,
    raim boolean
) AS 'MODULE_PATHNAME', 'pg_ais_fields'
LANGUAGE C STRICT;

-- Extract lon/lat from AIS message.
CREATE OR REPLACE FUNCTION pg_ais_point(text)
RETURNS point
AS 'MODULE_PATHNAME', 'pg_ais_point'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION pg_ais_point(text)
RETURNS bytea
AS 'MODULE_PATHNAME', 'pg_ais_point_geom'
LANGUAGE C STRICT;

CREATE OR REPLACE FUNCTION pg_ais_get_text_field(sentence ais, fieldname text)
RETURNS text
AS 'MODULE_PATHNAME', 'pg_ais_get_text_field'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_ais_get_int_field(sentence ais, fieldname text)
RETURNS integer
AS 'MODULE_PATHNAME', 'pg_ais_get_int_field'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION pg_ais_get_float_field(sentence ais, fieldname text)
RETURNS double precision
AS 'MODULE_PATHNAME', 'pg_ais_get_float_field'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
