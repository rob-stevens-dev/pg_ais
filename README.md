# pg_ais

PostgreSQL extension for decoding, indexing, and analyzing raw AIS (Automatic Identification System) messages.

- High-performance NMEA-0183 AIS parser in C
- Full support for all message types (single & multipart)
- Optimized for COPY and TimescaleDB ingestion
- Compatible with PostGIS: expose AIS lat/lon as geometry points
- Includes built-in benchmarking and observability

## Quickstart

```bash
make install
psql -c 'CREATE EXTENSION pg_ais;'
```

See `docs/INSTALL.md` for full details.

## Documentation Index

- [Installation](docs/INSTALL.md)
- [Howto Guide](docs/HOWTO.md)
- [Metrics](docs/METRICS.md)
- [Developer](docs/DEVELOPER.md)
- [Administration](docs/ADMIN.md)
