CREATE EXTENSION IF NOT EXISTS pg_ais;

CREATE TABLE ais_messages (
    id serial PRIMARY KEY,
    received_at timestamptz DEFAULT now(),
    sentence ais
);

-- Example: Functional index on MMSI (via pg_ais_fields)
CREATE INDEX ON ais_messages ((pg_ais_fields(sentence)).mmsi);

-- Optional composite index
CREATE INDEX ON ais_messages (
    (pg_ais_fields(sentence)).mmsi,
    (pg_ais_fields(sentence)).timestamp
);

-- Example: Fulltext-like filtering
-- (for ship_type filtering, often used in dashboards)
CREATE INDEX ON ais_messages ((pg_ais_fields(sentence)).ship_type);

-- Example: Geospatial acceleration (if PostGIS is installed)
-- Requires helper like: pg_ais_point(sentence) RETURNS geometry(Point, 4326)
-- CREATE INDEX ON ais_messages USING GIST (pg_ais_point(sentence));

-- Optional: Raw JSONB projection
-- If you want hybrid support using pg_ais_debug
CREATE INDEX ON ais_messages ((pg_ais_debug(sentence, 'json_enum')->'nav_status'));