# Getting Started with pg_ais

This guide walks you through setting up the development environment, compiling the extension, and loading it into PostgreSQL.

## Requirements

- Docker & docker-compose
- PostgreSQL 15+
- GNU Make and CMake

## Quickstart

```bash
git clone https://github.com/yourname/pg_ais.git
cd pg_ais
make docker-up
make docker-install
make docker-regression
```

## Loading the Extension

```sql
CREATE EXTENSION pg_ais;
```

## Basic Usage

```sql
CREATE TABLE ais_data (
  id SERIAL PRIMARY KEY,
  sentence ais
);

INSERT INTO ais_data(sentence) VALUES
  ('!AIVDM,1,1,,A,15Muq60001G?tTpE>Gbk0?wN0<0,0*7D');

SELECT pg_ais_get_int_field(sentence, 'mmsi') FROM ais_data;
```