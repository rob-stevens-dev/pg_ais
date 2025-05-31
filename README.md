# pg_ais

PostgreSQL extension for parsing and generating NMEA 0183 v4+ AIS messages.

## Features
- Custom `ais` type
- VARLENA storage for raw sentence
- Future: full NMEA parser, generator, indexing

## Building
```bash
mkdir build && cd build
cmake ..
make
