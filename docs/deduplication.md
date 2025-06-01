# Schema-Level Deduplication with md5 Hashing

To prevent duplicate AIS messages:

```sql
ALTER TABLE ais_data
  ADD COLUMN msg_hash BYTEA GENERATED ALWAYS AS (digest(sentence::text, 'md5')) STORED;

CREATE UNIQUE INDEX ON ais_data (msg_hash);
```

Use this during ingestion:

```sql
INSERT INTO ais_data(sentence) VALUES ('...')
ON CONFLICT DO NOTHING;
```

✅ COPY-friendly  
✅ No need for triggers or C code  
✅ Partition-friendly
