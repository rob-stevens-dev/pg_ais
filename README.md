# pg_ais — AIS Message Type for PostgreSQL

**Store. Decode. Index. Integrate.**

`pg_ais` is a PostgreSQL extension that introduces a native `ais` type for ingesting and decoding AIS (Automatic Identification System) messages conforming to NMEA 0183 v4.0+. It is engineered for high-throughput time-series ingestion, spatial indexing, and advanced analytics — without relying on any external libraries.

> Built from scratch in C. Powered by PostgreSQL internals. Compatible with TimescaleDB and PostGIS.

---

## ✨ Features

- ⚙️ **Native `ais` type**: Store AIS messages directly in PostgreSQL.
- 🧠 **Full NMEA 0183 v4+ support**: Every official AIS message type is parsed.
- 🔍 **Field accessors**: Extract integers, floats, enums, booleans, and strings with `pg_ais_get_*_field(...)`.
- 🗺️ **Geospatial support**: Generate PostGIS `POINT` via `pg_ais_point(...)`.
- 🧪 **Fully tested**: Regression tests and unit tests running inside Docker.
- 📦 **High-performance deduplication**: Schema-level hash deduplication with unique indexes.
- 🔍 **Indexing support**: Efficient indexing on computed fields (e.g. MMSI, ship name).
- 🧰 **Built for TimescaleDB**: Copy-friendly ingestion, ideal for hypertables.
- 🔧 **Zero dependency C code**: Built for PostgreSQL extension authors who care about internals.

---

## 🚀 Getting Started

The fastest way to try `pg_ais` is via Docker:

```bash
git clone https://github.com/yourname/pg_ais.git
cd pg_ais
make docker-up
make docker-install
make docker-regression  # run tests
```

Then inside `psql`:

```sql
CREATE EXTENSION pg_ais;

CREATE TABLE ais_data (
  id SERIAL PRIMARY KEY,
  sentence ais
);

INSERT INTO ais_data(sentence) VALUES
  ('!AIVDM,1,1,,A,15Muq60001G?tTpE>Gbk0?wN0<0,0*7D');

SELECT pg_ais_get_int_field(sentence, 'mmsi') FROM ais_data;
```

---

## 🧪 Example: Extracting & Indexing Fields

```sql
-- Extract MMSI and position
SELECT
  pg_ais_get_int_field(sentence, 'mmsi') AS mmsi,
  pg_ais_point(sentence) AS position
FROM ais_data;

-- Index on extracted field
CREATE INDEX ON ais_data ((pg_ais_get_int_field(sentence, 'mmsi')));
```

---

## 🛡️ Recommended: Deduplication Strategy

When ingesting high-volume AIS data, use this schema pattern:

```sql
ALTER TABLE ais_data
  ADD COLUMN msg_hash BYTEA GENERATED ALWAYS AS (digest(sentence::text, 'md5')) STORED;

CREATE UNIQUE INDEX ON ais_data (msg_hash);
```

Then ingest with:
```sql
INSERT INTO ais_data(sentence) VALUES ('...')
ON CONFLICT DO NOTHING;
```

This avoids storing duplicates while remaining COPY-friendly and scalable. See [docs/deduplication.md](docs/deduplication.md).

---

## 🌐 Integration with PostGIS & TimescaleDB

`pg_ais` plays well with:

- **PostGIS**:
  ```sql
  SELECT ST_SetSRID(pg_ais_point(sentence), 4326) FROM ais_data;
  ```

- **TimescaleDB**:
  ```sql
  SELECT create_hypertable('ais_data', 'timestamp');
  ```

See [docs/integration.md](docs/integration.md) for full usage patterns.

---

## 🧭 Optional: JSONB Output for Debugging

If you want to explore AIS messages in external tools (e.g., Grafana):

```sql
CREATE VIEW ais_data_json AS
SELECT id, pg_ais_debug(sentence) AS msg FROM ais_data;
```

`pg_ais_debug(...)` returns a structured JSONB object with all decoded fields.

See [docs/usage_examples.md](docs/usage_examples.md).

---

## 🧠 Author & Purpose

This project showcases deep PostgreSQL internals experience — from custom types and parsing, to extension safety, regression testing, and production-ready deployment.

If you're a company working on:
- Maritime data platforms
- Real-time vessel tracking
- GIS or spatial analytics
- PostgreSQL extension development

...then this work demonstrates the capability to build low-level, scalable, performant PostgreSQL-native solutions with discipline and elegance.

Built with love and a hex editor.

---

## 📚 Additional Docs

See `docs/`:

- [Getting Started](docs/getting_started.md)
- [Usage Examples](docs/usage_examples.md)
- [Deduplication](docs/deduplication.md)
- [Indexing Guidance](docs/indexing.md)
- [PostGIS & Timescale Integration](docs/integration.md)

---

## 🧪 Testing & Contribution

To run tests:
```bash
make docker-test       # Run C unit tests
make docker-regression # Run SQL regression tests
```

PRs welcome. Bring your own vessel.
