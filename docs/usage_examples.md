# Using pg_ais: Field Extraction and Debugging

## Field Accessors

```sql
SELECT
  pg_ais_get_int_field(sentence, 'mmsi'),
  pg_ais_get_float_field(sentence, 'speed'),
  pg_ais_get_bool_field(sentence, 'raim')
FROM ais_data;
```

## Geospatial Output

```sql
SELECT ST_SetSRID(pg_ais_point(sentence), 4326) FROM ais_data;
```

## JSONB Debug View

```sql
SELECT pg_ais_debug(sentence) FROM ais_data;
```