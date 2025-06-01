# Integration with PostGIS and TimescaleDB

## PostGIS

```sql
SELECT ST_SetSRID(pg_ais_point(sentence), 4326) FROM ais_data;
```

## TimescaleDB

```sql
SELECT create_hypertable('ais_data', 'timestamp');
```

AIS data is time-series and spatial — pg_ais is ready for both.
