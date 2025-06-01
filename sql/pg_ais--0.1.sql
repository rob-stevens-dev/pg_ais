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

-- Tabular support
CREATE FUNCTION pg_ais_fields(text)
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
CREATE FUNCTION pg_ais_point(text)
RETURNS point
AS 'MODULE_PATHNAME', 'pg_ais_point'
LANGUAGE C STRICT;

CREATE FUNCTION pg_ais_point(text)
RETURNS bytea
AS 'MODULE_PATHNAME', 'pg_ais_point_geom'
LANGUAGE C STRICT;