# Administration Guide

## Extension Creation

```sql
CREATE EXTENSION pg_ais;
```

## COPY Optimized

This extension is designed for high-throughput ingestion using:
- `COPY FROM STDIN`
- Deduplication via `msg_hash` + computed column
- Indexing via functional indexes

## Backpressure / Ingestion

Use RabbitMQ -> worker ingestion pattern (future work).

See: `pg_ais_metrics()` for operational metrics.
