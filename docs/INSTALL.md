# Installation

## Native Install

```bash
make clean install
psql -c 'CREATE EXTENSION pg_ais;'
```

## CMake

```bash
mkdir -p build && cd build
cmake ..
make install
```

## Docker

```bash
docker-compose up --build
```

Supports PostgreSQL 12–16.
