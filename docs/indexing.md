# Indexing Computed AIS Fields

pg_ais supports indexing via functional indexes:

```sql
CREATE INDEX ON ais_data ((pg_ais_get_int_field(sentence, 'mmsi')));
CREATE INDEX ON ais_data ((pg_ais_get_text_field(sentence, 'shipname')));
```

Use `SET enable_seqscan = OFF;` in testing to force index plan.

Pair this with EXPLAIN for visibility.
