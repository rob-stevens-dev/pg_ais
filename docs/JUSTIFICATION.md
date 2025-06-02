# Why Structured AIS Ingestion Matters

AIS messages are transmitted by ships and received by coastal radio receivers, satellites, or listening stations. In real-world deployments, these listeners write out raw AIS messages—one per line—in plain text files. Batches of these logs are then periodically sent to downstream systems.

But what happens next?

## The Flat File Problem

Text file dumps pose serious challenges:
- **No indexing** — Searching by MMSI, type, or timestamp is slow
- **Duplication** — The same message may be received by multiple stations
- **Storage waste** — Unstructured logs waste bytes across filesystem page boundaries
- **No schema** — It’s hard to enforce or validate message structure

Trying to analyze 100 million lines of raw AIS text becomes a nightmare of `grep`, `awk`, and pain.

## Real-World Example: Type 1 Storm

Class A vessels underway broadcast AIS Message Type 1 every 2–10 seconds.

Let’s say:
- One container ship broadcasts every 3 seconds
- It’s underway for 12 days
- That’s ~345,600 messages per vessel
- Multiply by hundreds of ships per day = **tens of millions** of messages per day

Now factor in:
- Redundant copies (10 receivers might hear the same message)
- Storage size of plain text vs PostgreSQL + compression

You’re looking at:
- GBs of unsearchable logs **vs.**
- A compressed, indexed, queryable TimescaleDB deployment

## What pg_ais Solves

- Efficient ingestion with `COPY`
- Support for `msg_hash`-based deduplication
- Spatial integration with PostGIS
- Typed access to every field
- JSONB debug views for developers
- Per-message metrics and benchmarking

**pg_ais turns a dump of raw AIS text into a structured, queryable dataset with analytics-grade access speed.**

This isn’t just an extension. It’s a blueprint for real-world ingest pipelines at scale.
