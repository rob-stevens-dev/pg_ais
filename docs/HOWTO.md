# Howto Guide

## Create a Table

```sql
CREATE TABLE ais_raw (
  id serial PRIMARY KEY,
  sentence ais
);
```

## Create a Computed Index

```sql
CREATE INDEX ON ais_raw ((pg_ais_get_int_field(sentence, 'mmsi')));
```

## Create a View

```sql
CREATE VIEW ais_data_json AS
SELECT id, pg_ais_debug(sentence) AS payload FROM ais_raw;
```

## Check Metrics

```sql
SELECT * FROM pg_ais_metrics();
SELECT pg_ais_reset_metrics();
```

## TimescaleDB Schema

Use `timestamptz` + computed geometry point:
```sql
SELECT pg_ais_point(sentence); -- for PostGIS POINT
```

Create hypertable:
```sql
SELECT create_hypertable('ais_ts', 'received_time');
```


## Deduplication Example

Avoid storing duplicate AIS messages by adding a computed hash column and unique index:

```sql
ALTER TABLE ais_raw ADD COLUMN msg_hash bytea
  GENERATED ALWAYS AS (digest(ais_to_cstring(sentence), 'md5')) STORED;

CREATE UNIQUE INDEX dedup_idx ON ais_raw(msg_hash);
```

This strategy is COPY-friendly and requires no triggers or procedural logic.

## PostGIS Integration

Convert AIS coordinates to PostGIS geometry for spatial queries:

```sql
SELECT ST_SetSRID(pg_ais_point(sentence), 4326) FROM ais_raw;
```
